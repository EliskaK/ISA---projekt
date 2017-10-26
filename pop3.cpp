/*
** VUT FIT 2017 - ISA
** Klient POP3 s podporou TLS
** Eliska Kadlecova
** login: xkadle34
*/
#include <sys/socket.h>

#include "main.hpp"

POP3::POP3(){ //konstruktor
std::cout << "konstruktor" << '\n';
}

/*
* Sifrovane pripojeni k serveru
*/
/*bool POP3::connect_server_sec(){

}*/

/*
* Pripojeni k serveru
*/
bool POP3::connect_server(std::string server, int port){

  struct hostent *host;
  struct sockaddr_in server_addr;
  struct in_addr **addresses;
  std::cout << "server: " << server << '\n';
  host = gethostbyname(server.c_str());
  if (!host){
    error("Server se nepodarilo nacist", 4);
  }
  std::cout << "host: " << host << '\n';
  addresses = (struct in_addr **) host->h_addr_list;
  for(int i = 0; addresses[i] != NULL; i++) {
    server_addr.sin_addr = *addresses[i];
    break;
  }

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  std::cout << "sockfd: " << sockfd << '\n';
  if (sockfd == -1){
    error("Nepodarilo se vytvorit socket", 4);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons((uint16_t)port);
  memcpy(&(server_addr.sin_addr), host->h_addr, host->h_length);
  int a = connect(sockfd, (sockaddr*) &server_addr, sizeof(server_addr));
  std::cout << "a: "<< a << '\n';
  if(a < 0){
    error("Nepodarilo se pripojit k serveru", 4);
  }
  sock = sockfd;
  return true; //connect se podarilo
}

/*
* Posle prikazy k prihlaseni, vraci true, kdyz se podarilo
*/
bool POP3::login(std::string username, std::string password){
  std::string sending_user = "USER ";
  sending_user += username;
  std::cout << sending_user << '\n';
  send_command(sending_user);
  get_response();
  std::string sending_pass = "PASS ";
  sending_pass += password;
  std::cout << sending_pass << '\n';
  send_command(sending_pass);
  get_response();

  return true;
}

/*
* Posle prikaz LOGOUT
*/
bool POP3::logout(){
  return true;
}

bool POP3::send_command(std::string command){
  command = command.append("\r\n");
  int n = write(sock, command.c_str(), command.length());
  if(n < 0){
    error("Nepodarilo se poslat prikaz na server", 5);
  }
  return true;
}

bool POP3::get_response(){
  char buff[201];
  int n = read(sock, buff, 200);
  if(n < 0){
    error("Nepodarilo se ziskat odpoved serveru", 5);
  }
  printf("%s\n",buff);
  return true;
}
