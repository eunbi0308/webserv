
#ifndef CGI_HPP
# define CGI_HPP

#include "Webserv.hpp"

class Server;
class HttpRequest;
class HttpResponse;
class Location;

class SafeString {
public:
  SafeString(const std::string& str) : data_(new char[str.size() + 1]), length_(str.size()) {
    std::copy(str.begin(), str.end(), data_.get());
    data_[length_] = '\0'; // Ensure null termination
  }

//   ~SafeString() {
//     delete[] data_;
//   }

  const char* getData() const { return data_.get(); }
  size_t getLength() const { return length_; }

private:
  std::unique_ptr<char[]> data_;
  size_t length_;
};

class Cgi
{
private:
    std::string cgiPass;
    char *cgiFile;
    char **env;
    char *postBody;
    int contentLen;
    
public:
    Cgi();
    Cgi(HttpRequest& req, Location& loc, Server& ser);
    ~Cgi();
    Cgi(Cgi& a);
    Cgi& operator=(const Cgi a);

    std::string getCgiPass() const {return cgiPass; };
    char* getCgiFile() const {return cgiFile; };
    char* getPostBody() const {return postBody; };
    char** getEnv() const {return env; };
    int     getContentLen() const {return contentLen; };

    void    setCgiFile(std::string s);
    void    setPostBody(HttpRequest& req);
    void    setContentLen(HttpRequest& req);
    void    setCgiEnv(HttpRequest& req, Location& loc, Server& ser);
    std::string    execCgi();
};

#endif
