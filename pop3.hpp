/*
** VUT FIT 2017 - ISA
** Klient POP3 s podporou TLS
** Eliska Kadlecova
** login: xkadle34
*/
#include <iostream>
#include <string>
#include <algorithm>
#include <arpa/inet.h>
#include <openssl/bio.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

//#include "main.hpp"

class POP3{
  public:
    POP3();
    bool connect_server(std::string server, int port);
    bool login(std::string username, std::string password);
    bool logout();
    bool send_command(std::string command);
    bool get_response();
  private:
    BIO *bio;
    SSL_CTX *ctx;
    int sock;
};
