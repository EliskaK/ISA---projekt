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

//#include "main.hpp"

class POP3{
  public:
    POP3();
    bool connect_server(std::string server, int port);
  private:
    BIO *bio;
    SSL_CTX *ctx;
};
