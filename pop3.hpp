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

//#include "main.hpp"
#define BUFFSIZE 2048 //velikost bufferu pro odpoved serveru

class POP3{
  public:
    POP3();
    ~POP3();
    bool connect_server(std::string server, int port);
    bool login(std::string username, std::string password);
    bool logout();
    bool send_command(std::string command);
    bool send_command(std::string command, int num);
    bool get_response();
    std::string read_from_server();
    //bool get_multiline();
    void messageList (bool new_only, std::string out_dir);
    bool downloadMsg(std::string out_dir);
    void del();
    void top ();
    void retr (int a);
    bool stat ();
  private:
    BIO *bio;
    SSL_CTX *ctx;
    int sock;
    std::string message;
    std::string buffer; //buffer pro ukladani mailu
    // char buff[2048] = {0}; //inicializace na nulu
    int numMsg; //pocet zprav
    bool isretr = false;
};
