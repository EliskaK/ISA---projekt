/*
** VUT FIT 2017 - ISA
** Klient POP3 s podporou TLS
** Eliska Kadlecova
** login: xkadle34
*/
#include <sys/socket.h>

#include "pop3.hpp"

POP3::POP3(){ //konstruktor

}

/*
* Sifrovane pripojeni k serveru
*/
bool POP3::connect_server_sec(){

}

/*
* Pripojeni k serveru
*/
bool POP3::connect_server(const char* server, int port){
  struct hostent *host;
  host = gethostbyname(server);
  if (host == NULL){
    error("Server se nepodarilo nacist", 4);
  }

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1){
    error("Nepodarilo se vytvorit socket", 4);
  }

  struct sockaddr_in server_socket;
  server_socket.sin_family = AF_INET;
  server_socket.sin_port = htons(port);

}
