#pragma once

#include "Webserv.hpp"

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
		HttpRequest		    *Request;
		std::string		    version;
		int				    statusCode;
		std::string		    body;

		std::string			index;
		std::string			resource;
		enum e_resourceType	resourceType; // A directory or a file
		std::string			content;
		std::string			contentType;
		std::string			MIMEtype;

		bool				completed; // check if a reponse is already created.

	public:
		HttpResponse();
		HttpResponse(const HttpResponse &response);
		HttpResponse &operator=(const HttpResponse &other);
		virtual ~HttpResponse();

		//Getter
		const	std::string	&getVersion() const { return version; }
		const	int			&getStatusCode()	const { return statusCode; }
		const	std::string	&getBody() const { return body; }
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
		
		//Response
		void		createResponse(enum e_statusCode code);
		void		createResponse(enum e_statusCode code, const std::string content);
		void		createResponse_File(std::string filename);
	
		//Checkers
		void		checkMethod();
		void		checkURI();
		void		checkResourceType();
		bool		checkResourcePermission(const std::string path);

		void		printDirectoryListing(const std::string &path);

		//Methods
		void		methodGet();
		void		methodPost();
		void		methodDelete();
		void		deleteFile();
		void		deleteDir();
        void        postFile();
};
