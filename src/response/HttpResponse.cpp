#include "HttpResponse.hpp"

HttpResponse::HttpResponse()
	: statusCode(0), body(""), index(""), resource(""), content(""), contentType(""), MIMEtype(""), completed(false)
{
	#ifdef STRUCTOR
		std::cout << GREY << "HttpResponse : Default constructor called" << DEFAULT << std::endl; 
	#endif
}

HttpResponse::HttpResponse(const HttpResponse &other)
{
	#ifdef STRUCTOR
		std::cout << GREY << "HttpResponse : Copy constructor called" << DEFAULT << std::endl; 
	#endif
	*this = other;
}

HttpResponse &HttpResponse::operator=(const HttpResponse &other)
{
	#ifdef STRUCTOR
		std::cout << GREY << "HttpResponse : Copy assigment operator called" << DEFAULT << std::endl;
	#endif
	if (this != &other)
	{
		this->statusCode = other.statusCode;
		this->body = other.body;
		this->headers = other.headers;

		this->Request = other.Request;
		this->index = other.index;
		this->resource = other.resource;
		this->resourceType = other.resourceType;
		this->content = other.content;
		this->contentType = other.contentType;
		this->MIMEtype = other.MIMEtype;
	}
	return *this;

}

HttpResponse::~HttpResponse()
{
	#ifdef STRUCTOR
		std::cout << GREY << "HttpResponse : Destructor called" << DEFAULT << std::endl; 
	#endif
}

void	HttpResponse::createResponse(enum e_statusCode code)
{
	this->createResponse(code, "");
}

void	HttpResponse::createResponse(enum e_statusCode code, const std::string content)
{
    #ifdef FUNC
	    std::cout << YELLOW << "[FUNCTION] createResponse" << DEFAULT << std::endl;
	#endif
	this->statusCode = code;

	if (this->statusCode == STATUS_SUCCESS
		|| this->statusCode == STATUS_MOVED
		|| this->statusCode == STATUS_NO_CONTENT
		|| this->statusCode == STATUS_CREATED)
	{
		std::ostringstream	ostream;
		ostream << "HTTP/1.1 " << this->statusCode << " " << returnStatusMessage(this->statusCode) << "\r\n";
		if (this->statusCode == STATUS_MOVED)
		{
			// Remove leading '.' from resource.
			ostream << "Location: " << this->resource.substr(1, this->resource.length() - 1) + "/" << "\r\n";
		}
 		if (!content.empty())
		{
			ostream << "Content-Length: " << content.length() << "\r\n";
			ostream << "Content-Type: text/html\r\n";
		}
		else
			ostream << "Content-Length: 0\r\n";
		ostream << "\r\n";
		if (!content.empty())
			ostream << content;
		this->content = ostream.str(); // a string a copy of ostream
	}
	if (this->statusCode >= 400)
		createErrorResponse(code);
	this->completed = true;
}

void	HttpResponse::createResponse_File(std::string filename)
{
    #ifdef FUNC
	    std::cout << YELLOW << "[FUNCTION] createResponse_File" << DEFAULT << std::endl;
	#endif
	std::ifstream	file(filename.c_str());

	if (file.is_open())
	{
		file.seekg(0, std::ios::end); // Move to the end of the file.
		if (file.tellg() > 0) // file is not empty.
		{
			file.seekg(0, std::ios::beg);
			std::ostringstream	ostream;
			std::ostringstream	fileContent;
			fileContent << file.rdbuf();

			ostream << "HTTP/1.1 200 OK\r\n";
			ostream << "Content-Length: " << fileContent.str().length() << "\r\n";
			setMIMEtype(filename);
			ostream << "Content-Type: " << this->getMIMEtype() << "\r\n";
			ostream << "\r\n";
			// print body part
			ostream << fileContent.str();
			file.close();
			this->content = ostream.str();
		}
		else
			createResponse(STATUS_NO_CONTENT);
	}
	else
		createResponse(STATUS_INTERNAL_ERR);
	this->completed = true;
}

void    HttpResponse::setResource()
{
	#ifdef FUNC
	    std::cout << YELLOW << "[FUNCTION] setResource" << DEFAULT << std::endl;
	#endif
	Server	*Server = this->Request->ReqServer;

	if (this->Request->ReqLocation)
	{
		Location	*Location = this->Request->ReqLocation;
		// Check redirection
		if	(!(Location->getRedirect().empty()))
		{
			this->resource = Location->getRedirect();
			createResponse(STATUS_FOUND);
		}
		this->resource = "." + this->Request->getURI();
	}
	else // No selected Location
	{
		std::cout << BLUE << "ORIGIN URI :::" << this->Request->getURI() << std::endl;
		if (this->Request->getURI() == "/")
			this->resource = "." + Server->getRoot() + '/' + Server->getIndex();
		else
			this->resource = "." + this->Request->getURI();
	}
	#ifdef FUNC
		std::cout << YELLOW  << "RESOURCE : " << this->resource << DEFAULT << std::endl;
	#endif
}

//Yuka added
void    HttpResponse::runCgi(){
    if (Request->getCgi() != nullptr){
		std::cout << YEL << Request->getCgi()->getCgiFile() << RES << std::endl;
		std::vector<char> cgiReturnedBody = Request->getCgi()->execCgi();
        content = std::string(cgiReturnedBody.begin(), cgiReturnedBody.end());
        if (content[0] == '\0')
            perror("CGI not being executed");//error need to change
        else
            completed = true;
    }
}
