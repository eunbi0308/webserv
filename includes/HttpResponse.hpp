#pragma once


#include <unordered_map>
#include "utils.hpp"
#include <cstdlib> // remove(), std::system()
#include <fstream> // ifstream
#include "HttpRequest.hpp"
#include "Webserv.hpp"
#include <exception>


enum	e_resourceType
{
	RESOURCE_FILE = 0,
	RESOURCE_DIR
};

class HttpRequest;
class Cgi;

class HttpResponse
{
	protected:
		HttpRequest		*Request;
		std::string		version;
		int				statusCode;
		std::string		body;
		std::unordered_map<std::string, HttpHeader> headers;

		std::string			index;
		std::string			resource;
		enum e_resourceType	resourceType;
		std::string			content;
		std::string			contentType;
		std::string			MIMEtype;

		bool				completed; // check if request 

	public:
		HttpResponse();
		HttpResponse(const HttpResponse &response);
		HttpResponse &operator=(const HttpResponse &other);
		virtual ~HttpResponse();

		//Getter
		const	std::string	&getVersion() const { return version; }
		const	int			&getStatusCode()	const { return statusCode; }
		// const	std::string	&getStatusMessage() const { statusMessage = returnStatusMessage(statusCode); return  statusMessage; }
		const	std::string	&getBody() const { return body; }
		const	std::unordered_map<std::string, HttpHeader> &getHeaders() const { return headers; }
		HttpRequest			*getRequest() const { return Request; }
		const	std::string	getResource() const { return resource + index; }
		const	std::string	&getContent() { return content; }
		const	bool		&getCompleted() { return completed; }
		const	std::string	getMIMEtype() const { return MIMEtype; }

		//Setter
		void		setRequest(HttpRequest	*request) { Request = request; }
        void    	setResource();
		void		setResourceType(enum e_resourceType type) { resourceType = type; }
		void		setContent(std::string str) { content = str; }
		void		setCompleted(bool value) { completed = value; }
		void		setMIMEtype(const std::string &filename);
		
		void		createResponse(enum e_statusCode code);
		void		createResponse_File(std::string filename);
	
		void		checkMethod();
		void		checkURI();
		void		checkResourceType();
        bool        checkAllowedMethod();
		bool		checkIndexFileExists(const std::string path, const std::string filename);
		void		methodGet();
		void		methodPost();
		void		methodDelete();
		void		deleteFile();
		void		deleteDir();
        void        postFile();

		void		runCgi();
};
