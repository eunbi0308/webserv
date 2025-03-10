#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <string>
#include <sstream> // std::stringstream
#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h> // getaddrinfo()
#include <sys/types.h> //accept()
#include <cstdlib> // remove(), std::system()
#include <map>
#include <poll.h> // poll() and struct
#include <fcntl.h>
#include <memory> // std::unique_ptr c++11, std::make_unique c++14
#include <fstream>
#include <istream>
#include <filesystem>
#include <algorithm>
#include <iterator>
#include <cctype>
#include <csignal>
#include <cstring>
#include <functional>
#include <sys/wait.h>
#include <chrono>
#include <ctime>
#include <unordered_map>
#include <exception>
#include <optional>

#include "Config.hpp"
#include "Server.hpp"
#include "ServerManager.hpp"
#include "Client.hpp"
#include "Location.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Cgi.hpp"
#include "utils.hpp"

#define BOLD "\033[1m"
#define RES "\033[0m"
#define RED "\033[31m"
#define GRE "\033[32m"
#define YEL "\033[33m"
#define BLU "\033[34m"
#define MAG "\033[35m"
#define CYA "\033[36m"
#define WHI "\033[37m"

#endif
