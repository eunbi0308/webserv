#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Webserv.hpp"

class Server;
class HttpRequest;
class HttpResponse;
class Cgi;

enum e_readyTo
{
	READ,
	WRITE
};

class Client
{
private:
    int clientFd;
	int readyTo;
	HttpRequest		*Request;
	HttpResponse	*Response;
	Cgi				*cgi;
	std::time_t lastActivity;
	std::time_t connectionTime;

public:
    Client(int fd, int readyTo);
    ~Client();

    int 			getClientFd();
	std::time_t		getLastActivity();
	std::time_t		getConnectionTime();
	HttpRequest		*getRequest() const { return Request; }
	HttpResponse	*getResponse() const { return Response; }
	Cgi				*getCgi() const { return cgi; }
	int 			getReadyToFlag() const;

	void	setRequest(HttpRequest	*request) { Request = request; }
	void	setResponse(HttpResponse	*response) { Response = response; }	
	void 	setReadyToFlag(int readyTo);
	void	setClientFdEvent(std::vector<struct pollfd>& pollfds, short events);

	void 	handleCgiRequest(ServerManager *ServerManager);
	void	finishCgi();
};

std::ostream& operator<<(std::ostream& out, const Client& client);

#endif
