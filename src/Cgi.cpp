#include "../includes/Cgi.hpp"

#define BUFFER_SIZE 4096
#define WRITE_SIZE 4096

/*Because you won’t call the CGI directly, use the full path as PATH_INFO.
∗ Just remember that, for chunked request, your server needs to unchunk
it, the CGI will expect EOF as end of the body.
∗ Same things for the output of the CGI. If no content_length is returned
from the CGI, EOF will mark the end of the returned data.
∗ Your program should call the CGI with the file requested as first argument.
∗ The CGI should be run in the correct directory for relative path file access.
∗ Your server should work with one CGI (php-CGI, Python, and so forth)*/

Cgi::Cgi(){}

Cgi::Cgi(HttpRequest& req, Location& loc, Server& ser, ServerManager &sManager)
: pass(NULL), postBody(NULL), contentLen(0), manager(&sManager), pipeRead(-1), pipeWrite(-1), childPid(-1), finishReading(false), appendBytes(0)
{
    cgiPass = loc.getCgiPass();
    setCgiFile("."+loc.getRoot()+req.getURI());
    setCgiEnv(req, loc, ser);
    // setPostBody(req);
    setContentLen(req);
}

Cgi::~Cgi(){
    #ifdef STRUCTOR
		std::cout << GREY << "Cgi : Default destructor called" << DEFAULT << std::endl; 
	#endif
    delete[] this->cgiFile;
    for (int i = 0; this->env[i] != NULL; ++i) {
        delete[] this->env[i];
    }
    delete[] this->env;
    delete[] this->pass;
    if (this->pipeRead != -1)
        close (this->pipeRead);
    if (this->pipeWrite != -1)
        close (this->pipeWrite);
    if (isRunningCgi() == true)
        kill(this->childPid, SIGKILL);
    this->manager->rmFdFromPollfd(pipeRead);
    this->manager->rmFdFromPollfd(pipeWrite);
}


void Cgi::setCgiFile(std::string s)
{
    std::size_t query_pos = s.find('?');
    if (query_pos != std::string::npos)
    {
        s = s.substr(0, query_pos);
    }
    cgiFile = new char[s.size() + 1];
    std::strcpy(cgiFile, s.c_str());
}


void Cgi::setContentLen(HttpRequest& req) {
    contentLen = req.getContentLength();
}

// parse in tmp and copy it to char* env
void Cgi::setCgiEnv(HttpRequest& req, Location& loc, Server& ser){
    std::vector<std::string> tmp;

    tmp.push_back("GATEWAY_INTERFACE=CGI/1.1");
    tmp.push_back("SERVER_NAME=" + ser.getHost()); //server hostname
    tmp.push_back("SERVER_SOFTWARE=webserv/1.0");
    tmp.push_back("SERVER_PROTOCOL=HTTP/1.1");
    tmp.push_back("SERVER_PORT=" + std::to_string(ser.getPort())); //server port
    tmp.push_back("REQUEST_METHOD=" + req.getMethod()); //request method
    tmp.push_back("PATH_INFO=" + loc.getRoot() + req.getURI()); // <<< full path to the cgi file
    tmp.push_back("SCRIPT_NAME=/index.py"); //cgi pass
    tmp.push_back("DOCUMENT_ROOT=" + loc.getRoot()); //location getRoot()
    tmp.push_back("QUERY_STRING=" + req.getQueryString()); //getQuery
    tmp.push_back("CONTENT_TYPE=" + req.getContentType()); // ex. text/html
    tmp.push_back("CONTENT_LENGTH=" + std::to_string(req.getContentLength()));
    this->env = new char*[tmp.size() + 1];
    int i = 0;
    for (std::vector<std::string>::iterator t = tmp.begin(); t != tmp.end(); ++t){
        this->env[i] = new char[(*t).size() + 1];
        strcpy(this->env[i], (*t).c_str());
        ++i;
    }
    this->env[tmp.size()] = NULL;
}


void    Cgi::childDup(int ToCgi[2], int FromCgi[2]){
    close(ToCgi[1]);
    if (dup2(ToCgi[0], STDIN_FILENO) < 0)
        throw std::runtime_error("Read read pipe failed");
    close(ToCgi[0]);
    close(FromCgi[0]); 
    if (dup2(FromCgi[1], STDOUT_FILENO) < 0)
        throw std::runtime_error("Write write pipe failed");
    close(FromCgi[1]); 
}

void    Cgi::initParentPipe(int ToCgi[2], int FromCgi[2]){
    #ifdef CGI
		std::cout << PINK << "[ Cgi ] initParentPipe" << DEFAULT << std::endl; 
	#endif
    close(ToCgi[0]);
    close(FromCgi[1]);
    pipeWrite = ToCgi[1];
    pipeRead = FromCgi[0];
     #ifdef CGI
		std::cout << PINK << "pipeWrite is " << pipeWrite << "   pipeRead is " << pipeRead << DEFAULT << std::endl; 
	#endif
}

void    Cgi::writeToCgi(){
    #ifdef CGI
		std::cout << PINK << "[ Cgi ] writeToCgi" << DEFAULT << std::endl; 
	#endif
    if (cgiInput.empty()){
        manager->addFdToPollFds(pipeRead, POLLIN);
        manager->rmFdFromPollfd(pipeWrite);        // manager.removeEvent(pipeWrite, POLLOUT);
        close(pipeWrite);
        return ;
    }
	size_t writeSize = WRITE_SIZE;

	if (WRITE_SIZE >= this->cgiInput.size()) {
		this->cgiInput.push_back('\0');
		writeSize = this->cgiInput.size();
	}
    ssize_t bytes = write(this->pipeWrite, this->cgiInput.data(), writeSize); //cgiInput stores a whole request incl
    if (bytes < 0)
		throw std::runtime_error ("Writing to CGI has failed" );
    cgiInput.erase(cgiInput.begin(),  cgiInput.begin() + bytes);
    if (cgiInput.empty())
    {
        manager->rmFdFromPollfd(pipeWrite);
        manager->addFdToPollFds(pipeRead, POLLIN);
    }
}

void    Cgi::readFromCgi(){
    #ifdef CGI
		std::cout << PINK << "[ Cgi ] readFromCgi" << DEFAULT << std::endl; 
	#endif
    std::vector<char> buf(BUFFER_SIZE);
    ssize_t bytes = 1;

    bytes = read(this->pipeRead, buf.data(), BUFFER_SIZE);
    if (bytes < 0)
        std::runtime_error("Reading from CGI has failed.");
    if (bytes == 0)
    {
        std::cout << "\n\nbytes == 0 " << std::endl;
        finishReading = true;
        this->appendBytes = 0;
    }
    else
    {
        cgiOutput.insert(cgiOutput.end(), buf.begin(), buf.begin() + bytes);
        if (this->appendBytes == 0 && manager->isFdInPollfds(pipeRead) == false)
            manager->addFdToPollFds(pipeRead, POLLIN);
        this->appendBytes += bytes;
    }
}

void    Cgi::execCGI()
{
    #ifdef CGI
		std::cout << PINK << "[ Cgi ] execCGI" << DEFAULT << std::endl; 
	#endif
    int r_pip[2]; // pipe to cgi
    int w_pip[2]; // pipe from cgi
    pid_t pid;

    pass = new char[cgiPass.size() + 1];
    std::strcpy(pass, cgiPass.c_str());
    char *argv[3] = {pass, cgiFile, NULL};

    //Initialize pipes
    if (pipe(r_pip) < 0)
		throw std::runtime_error("pipe failed");
    if (pipe(w_pip) < 0){
        close (r_pip[0]);
        close (r_pip[1]);
		throw std::runtime_error("pipe failed");
    }
    // Executing cgi script
	pid = fork();
    this->childPid = pid;
	if (pid < 0){
        close (w_pip[0]);
        close (w_pip[1]);
        close (r_pip[0]);
        close (r_pip[1]);
		throw std::runtime_error("fork failed"); 
    }
    else if (pid == 0){ //child process
        childDup(r_pip, w_pip);
        // if (access(cgiFile,F_OK) != 0)
		// 	throw std::runtime_error("STATUS_NOT_FOUND");
		// if (access(cgiFile,X_OK) != 0)
		// 	throw std::runtime_error("STATUS_FORBIDDEN");
        if (execve(cgiFile, argv, env) < 0){
            exit(1);
            // throw std::runtime_error("child");
        }
    }
    else {
        //parent process
        initParentPipe(r_pip, w_pip);
        if (cgiInput.size() > 0) // if there is any data to send to CGI
        {
            manager->addFdToPollFds(pipeWrite, POLLOUT); // keep
        }
        else { 
            close(pipeWrite);
            pipeWrite = -1;
            manager->addFdToPollFds(pipeRead, POLLIN);
        }
    }
}

void    Cgi::putRequestIntoCgiInput(const std::string rawRequest)
{
    #ifdef CGI
    	std::cout << PINK << "[ Cgi ] putRequestIntoCgiInput" << DEFAULT << std::endl; 
    #endif
    this->cgiInput.clear();
    for (size_t i = 0; i < rawRequest.size(); ++i)
        this->cgiInput.push_back(rawRequest[i]);
}

bool	Cgi::isRunningCgi()
{
	return (childPid != -1 && waitpid(childPid, NULL, WNOHANG) == 0);
}

void Cgi::rmPipesFromPollfd()
{
    std::vector<struct pollfd> pollfds = this->manager->getPollfds();
    for (std::vector<struct pollfd>::iterator it = pollfds.begin();
		it != pollfds.end(); it++)
	{
		if (it -> fd == pipeWrite)
		{
			pollfds.erase(it);
		}
        else if (it -> fd == pipeRead)
		{
			pollfds.erase(it);
		}
	}
}

void    Cgi::setCgiInput(std::string postBody)
{ 
    std::vector<char> charVector;
    
    for (size_t i = 0; i < postBody.size(); ++i)
    {
        charVector.push_back(postBody[i]);
    }
    this->cgiInput = charVector;
}
