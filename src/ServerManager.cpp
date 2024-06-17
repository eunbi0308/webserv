#include "Webserv.hpp"

ServerManager::ServerManager()
{
    std::cout << "ServerManager constructor is called" << std::endl;
}

ServerManager::~ServerManager()
{
    std::cout << "ServerManager destructor is called" << std::endl;
}

void ServerManager::printServers() const
{
    std::cout << "*****************************" << std::endl;
    for (const auto &server: servers)
        std::cout << *server << std::endl;
    std::cout << "*****************************" << std::endl;
}

void ServerManager::printPollFds() const
{
    std::cout << "*****************************" << std::endl;
    for (const auto &pollfd: pollfds)
        std::cout << "FD in POLL: " << pollfd.fd << std::endl;
    std::cout << "*****************************" << std::endl;
}

const std::vector<std::unique_ptr<Server>>& ServerManager::getServers() const
{
    return this->servers;
}

// std::unique_ptr<Server>	ServerManager::getServer(std::string host) const
// {
//     auto &servers = this->getServers();

//     for (auto &server: this->servers)
//     {
//         if (std::string::compare(host, server->getHost()) == 0)
//             //return *this;
//     }
// }

//When you call this->servers.push_back(server);
//you are trying to copy the std::unique_ptr.
//However, copying std::unique_ptr is not allowed because
//it would lead to multiple std::unique_ptr instances
//trying to manage the same resource, violating the unique ownership guarantee.
//Using this->servers.push_back(std::move(server));
//explicitly moves the std::unique_ptr into the vector.
//This transfer of ownership ensures that the vector now
//owns the resource, and the original std::unique_ptr becomes empty.
void ServerManager::addServer(std::unique_ptr<Server> server)
{
    //this->servers.push_back(server);
    this->servers.push_back(std::move(server));
}

void ServerManager::startServerManager(ServerManager& serverManager)
{
    auto &servers = serverManager.getServers();
    
    tempConfigServer(serverManager);
    startSockets();
    for (const auto &server: servers)
    {
        std::cout << "SERVER---- " << *server << std::endl;
        //std::cout << "serverFd: " << server->getServerFd() << std::endl;
    }
}

void ServerManager::startSockets()
{
    //make a loop here, start socket for each server in servers;
    //assign pollfd for each, then push it to pollfds array
    auto &servers = this->getServers();
    for (auto &server: servers)
    {
        server->createSocket();
        server->setSocketAddr();
        server->setSocketOption();
        server->bindSocket();
        server->listenSocket();
        addFdToPollFds(server->getServerFd(), POLLIN); //monitor incoming connections
       
        //get() returns a pointer to the managed object (server);   
        mapServerFd.emplace(server->getServerFd(), server.get());

    }
    std::cout << "SIZE OF MAP IS: " << mapServerFd.size() << std::endl;
    std::cout << "SIZE OF POLLFD VECTOR IS: " << pollfds.size() << std::endl;
    printServers();
}

// struct pollfd {
//     int fd; /* descriptor to check */
//     short events; /* events of interest on fd */
//     short revents; /* events that occurred on fd */
// };
void ServerManager::addFdToPollFds(int fd, int events)
{
    struct pollfd PollFd;

    PollFd.fd = fd;
    PollFd.events = events;
    this->pollfds.push_back(PollFd);
}

void ServerManager::acceptClient(int serverFd, Server& server)
{
    struct sockaddr_in cliAddr;

    (void)server;
    std::memset(&cliAddr, 0, sizeof(cliAddr));
    unsigned int cliLen = sizeof(cliAddr);
    int clientFd = accept(serverFd, (struct sockaddr *)&cliAddr, &cliLen);
    if (clientFd == -1)
    {
        throw std::runtime_error("Error: accept()");
    }
    //server.getConnectedClientFds().push_back(clientFd);
    server.connectedClientFds.push_back(clientFd);
    server.printConnectedClientFds();
    addFdToPollFds(clientFd, (POLLIN | POLLOUT));
}


//Serverfd && POLLIN indicates incoming CONNECTION,
//Clientfd && POLLIN indicates incoming data from CONNECTED CLIENT.
//(the client has data ready to be read by the server)
void ServerManager::startPoll()
{
    //Copying original pollfds for safety.
    //std::unique_ptr<Server> selectedServer;
    //std::vector<struct pollfd> pollfds = this->pollfds;
    //The poll() function expects its first argument to be
    //a pointer to an array of pollfd structures. In my case, fds is a vector,
    // not a raw array. However, the data() method provides a way
    //to access the raw memory where the vector elements (the pollfd structures)
    //are stored, essentially treating it like an array.
    while (1)
    {
        this->printPollFds();
        int num_readyFds = poll(pollfds.data(), pollfds.size(), -1);  // Wait indefinitely
        if (num_readyFds == -1)
        {
            // throw std::runtime_error("Error: setsockopt()");
            // break;
            //'continue' is from the book, not sure.
            continue ;
        }
        std::cout << pollfds.size() << std::endl;
        
        //counter is to see how many time poll loop turns.
        int counter = 0;
        // Process ready file descriptors
        //for (int i = 0; i < num_readyFds; ++i)
        for (size_t i = 0; i < pollfds.size(); ++i)
        {
            int fd = pollfds[i].fd;
            int revents = pollfds[i].revents;
            
            //The bitwise AND operation allows you to check if a specific event
            //bit (POLLIN) is present in the revents field.
            //In networking, bits are crucial for representing data packets efficiently and reliably.
            
            //If a client attempts to connect to one of your listener
            //server sockets, this will trigger the POLLIN event on that socket's file descriptor.
            //When the loop iterates through the pollfd vector and
            //encounters a listener socket, the revents & POLLIN condition
            //will evaluate to true because the POLLIN bit is set in the
            //revents flags.

            //std::cout << "THE FISH IS " << fd << std::endl;
            counter = counter+ 1;
            //std::cout << "counter = " << counter << std::endl;
            if (revents & POLLIN)
            {
                // if a server received a request. let's accept a client
                if (isFdInMap(fd, mapServerFd)) //fd is one of the server's fd
                {
                    acceptClient(fd, *mapServerFd[fd]); //that client is accepted by
                                    // *mapServerFd[fd] server
                    //std::cout << pollfds.size() << std::endl;

                    break ;
                }
                else //continue reading operations on connected clients
                {    //Request.readRequest(fd); fd will be client's
                    std::cout << "REQUESTTTTTT" << std::endl;
                    readRequest(fd);
                }

                //std::cout << "LALALO" << std::endl;
                //std::cout << *(mapServerFd[fd]) << std::endl;
            }

            // Here check writing operation's klaarheid.
            else if (revents & POLLOUT)
            {
                std::cout << "RESPONSEEEEE" << std::endl;
            }
            std::cout << "ENDDDDDD" << std::endl;
            // Handle events for accepted connections (read/write data)
            // You'll need to iterate over other servers and their connections here
            // ...
        }
    }

    //std::unique_ptr<Server>	getServer(std::string host) const; 
}

// if (_pollFds[i].revents & POLLIN)
//                 {
//                     if (_pollFds[i].fd == _serverSocket)
//                         acceptConnection();
//                     else
//                         handleClientData(i);
//                 }
//                 else if (_pollFds[i].revents & POLLOUT)
//                     sendClientData(i);



bool ServerManager::isFdInMap(int fd, std::map<int, Server*>& mapServerFd)
{
  // Find any element in the map where the key (file descriptor) matches the provided 'fd'
  auto it = std::find_if(mapServerFd.begin(), mapServerFd.end(),
                         [fd](const auto& pair) { return pair.first == fd; });

  // Return true if a matching element is found, false otherwise
  bool a = it != mapServerFd.end();
  std::cout << "BOOL = " << a << std::endl;
  return it != mapServerFd.end();
}

bool isFdConnected(int fd, std::vector<int>& connectedClientFds)
{
    if (std::find(connectedClientFds.begin(), connectedClientFds.end(), fd) != connectedClientFds.end())
        return true;
    else
        return false;
}

void readRequest(int clientFd)
{
    //Response response(clientFd);
	//response.handle_request();

    // int byteRead;
    // char buf[1064];

	// if ((byteRead = read(clientFd, &buf, 1064)) > 0)
    // {
    //     if (byteRead != write(clientFd, "KOLONYAAAA", byteRead))
    //         exit(1);
    // }

  const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 7\r\n\r\nKOLONYA";

  // Send the response to the client
  if (write(clientFd, response, strlen(response)) == -1) {
    perror("write");
    // Consider closing the client socket (optional)
  }

    std::cout << "KOLONYAAAA" << std::endl;
    if (close(clientFd) == -1)
        exit(1);
}

std::ostream& operator<<(std::ostream& out, const ServerManager& serverManager)
{
    (void)serverManager;
    return out;
}