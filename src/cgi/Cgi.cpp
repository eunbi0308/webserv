#include "../../includes/Cgi.hpp"

/*Because you won’t call the CGI directly, use the full path as PATH_INFO.
∗ Just remember that, for chunked request, your server needs to unchunk
it, the CGI will expect EOF as end of the body.
∗ Same things for the output of the CGI. If no content_length is returned
from the CGI, EOF will mark the end of the returned data.
∗ Your program should call the CGI with the file requested as first argument.
∗ The CGI should be run in the correct directory for relative path file access.
∗ Your server should work with one CGI (php-CGI, Python, and so forth)*/


Cgi::Cgi(){}

Cgi::Cgi(HttpResponse& response){
    this->env = initCgiEnv(response);
}


Cgi::~Cgi(){

}

Cgi::Cgi(Cgi& a){

}

Cgi& Cgi::operator=(const Cgi a){

}

// parse in tmp and copy it to char* env
char **Cgi::initCgiEnv(HttpResponse& response){
    std::vector<std::string> tmp;
    std::vector<char *> env;

    tmp.push_back("GATEWAY_INTERFACE=cgi/1.1");
    tmp.push_back("SERVER_NAME="); //server hostname
    tmp.push_back("SERVER_SOFTWARE=webserv/1.0");
    tmp.push_back("SERVER_PROTOCOL=HTTP/1.1");
    tmp.push_back("SERVER_PORT="); //server port
    tmp.push_back("REQUEST_METHOD="); //request method
    tmp.push_back("SCRIPT_NAME="); //cgi pass
    tmp.push_back("DOCUMENT_ROOT="); //location getRoot()
    tmp.push_back("QUERY_STRING="); //getQuery
    tmp.push_back("CONTENT_TYPE=");
    tmp.push_back("CONTENT_LENGTH="); //location-maxbodysize

    for (std::string s : tmp)
        env.push_back(&s.front());
    return (env.data());
}

void    Cgi::execCgi(){
    int pip[2];
    pid_t pid;

    if (pipe(pip) < 0)
		perror("pipe failed"); //error
	pid = fork();
	if (pid < 0)
		perror("fork failed"); //error
	if (pid == 0){
        if (execve(, , env) < 0)
            //error
    }
}

/*
1. check if the file requested is located in the location directory with cgi pass (maybe before checkMethods() in main)
2. if yes, initialize Cgi class. parse a full path for cgi program (= cgi path??)
3. make a child process by fork() and open pipe

4. Execute CGI in child process.
    av[0] = cgi file name 
    av[1] = path to the cgi file
    av[2] = env or NULL (need to figure out)

env has parsed request and some more variables according to RFC3875

Meta-variables with names beginning with "HTTP_" contain values read 
from the client request header fields, if the protocol used is HTTP.
The HTTP header field name is converted to upper case, has all    
occurrences of "-" replaced with "_" and has "HTTP_" prepended to give the meta-variable name.

5. Pass requested data through pipe in Parent Process.



if config file contains following:
location *.php {
methods GET POST
cgi_pass /usr/bin/php-cgi
return https:www.google.com
}

*.php is passed as the first parameter of execve() and /usr/bin/php-cgi will be used as the second parameter (third parameter is env)

So the CGI steps will be...


    Apache executes the CGI program, passing the parameters from the request to the CGI program in the environment (as environment variables).

    The CGI program gets the parameters from the environment, performs any required processing, and writes the web page on standard output.

    Apache receives the web page from the CGI program's standard output and transmits it to the web client (usually your web browser).


*/