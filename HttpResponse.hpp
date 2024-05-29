#pragma once

#include <unordered_map>
#include "utils.hpp"
#include <cstdlib> // remove(), std::system()


enum	resourceType
{
	RESOURCE_FILE,
	RESOURCE_DIR
};

class HttpRequest;

class HttpResponse
{
	protected:
		std::string		version;
		int				statusCode;
		std::string		statusMessage;
		std::string		body;
		std::unordered_map<std::string, HttpHeader> headers;

		HttpRequest			*Request;
		std::string			index;
		std::string			resource;
		enum resourceType	resourceType;
		std::string			content;
		std::string			contentType;
		std::string			location;
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
		const	std::string	&getStatusMessage() const { return statusMessage; }
		const	std::string	&getBody() const { return body; }
		const	std::unordered_map<std::string, HttpHeader> &getHeaders() const { return headers; }

		HttpRequest					*getRequest() const { return Request; }
		const	 std::string		getResource() const { return resource + index; }
		const	std::string			&getContent() { return content; }

		std::string	getStatusMessage();
		void		checkMethod();
		void		createResponse(enum e_statusCode code);
		void		createResponse_File(std::string filename);
		std::string	getMIMEtype() const;
		void		checkURI();
		void		methodGet();
		void		methodPost();
		void		methodDelete();
		void		deleteFile();
		void		deleteDir();
};
