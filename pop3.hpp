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
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
/* OpenSSL headers */
#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

//#include "main.hpp"
#define BUFFSIZE 2048 //velikost bufferu pro odpoved serveru

class POP3{
  public:
    POP3();
    ~POP3();
    bool connect_server(std::string server, int port);
    bool connect_server_sec(std::string server, int port, std::string cert_dir, std::string cert_file);
    bool login(std::string username, std::string password);
    bool quit();
    bool send_command(std::string command);
    bool send_command_sec(std::string command);
    bool send_command(std::string command, int num);
    bool send_command_sec(std::string command, int num);
    bool get_response();
    std::string read_from_server();
    std::string read_from_server_sec();
    size_t is_end_of_message(std::string msg);
    bool downloadMsg(std::string out_dir);
    void dele();
    void retr (int a);
    bool stat ();
    void finish();

  private:
    BIO *bio;
    SSL_CTX *ctx;
    SSL *ssl;
    int sock;
    std::string message;
    std::string buffer; //buffer pro ukladani mailu
    int numMsg; //pocet zprav
    bool isretr = false;
    bool isdele = false;
    bool secured;
};
