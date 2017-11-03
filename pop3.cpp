/*
** VUT FIT 2017 - ISA
** Klient POP3 s podporou TLS
** Eliska Kadlecova
** login: xkadle34
*/
#include <sys/socket.h>

#include "main.hpp"

POP3::POP3(){ //konstruktor
//std::cout << "konstruktor" << '\n';
}

POP3::~POP3(){


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
  //std::cout << "server: " << server << '\n';
  host = gethostbyname(server.c_str());
  if (!host){
    error("Server se nepodarilo nacist", 4);
  }
  //std::cout << "host: " << host << '\n';
  addresses = (struct in_addr **) host->h_addr_list;
  for(int i = 0; addresses[i] != NULL; i++) {
    server_addr.sin_addr = *addresses[i];
    break;
  }

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  //std::cout << "sockfd: " << sockfd << '\n';
  if (sockfd == -1){
    error("Nepodarilo se vytvorit socket", 4);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons((uint16_t)port);
  memcpy(&(server_addr.sin_addr), host->h_addr, host->h_length);
  int a = connect(sockfd, (sockaddr*) &server_addr, sizeof(server_addr));
  //std::cout << "a: "<< a << '\n';
  if(a < 0){
    error("Nepodarilo se pripojit k serveru", 4);
  }
  sock = sockfd;
  get_response();
  printf("S: %s",buff);
  return true; //connect se podarilo
}

/*
* Posle prikazy k prihlaseni, vraci true, kdyz se podarilo
*/
bool POP3::login(std::string username, std::string password){
  std::string sending_user = "USER ";
  sending_user += username;

  send_command(sending_user);
  get_response();

  printf("S: %s",buff);
  std::string sending_pass = "PASS ";
  sending_pass += password;

  send_command(sending_pass);
  get_response();
  printf("S: %s",buff);

  return true;
}

/*
* Posle prikaz LOGOUT
*/
bool POP3::logout(){
  send_command("QUIT");
  get_response();
  printf("S: %s",buff);
  return true;
}

/*
* Posle prikaz LIST k vypsani poctu zprav a jejich velikosti
*/
void POP3::messageList (bool new_only, std::string out_dir){
  send_command("STAT");
  get_response();
  printf("S: %s",buff);
}

bool POP3::stat (){
  send_command("STAT");
  get_response();
  printf("S: %s",buff);
  char* temp = strstr(buff, "+OK ");
  if (temp == NULL){
    return false;
  }
  else{
    std::cout << "temp:" << temp << '\n';
    sscanf(temp, "%d", &numMsg);
    //TODO: vypreparovat z temp jen pocet zprav
    std::cout << "numMsg: " << numMsg <<'\n';
    return true;
  }
}

void POP3::del (){
  send_command("DELE 1");
  get_response();
  printf("S: %s",buff);
}

void POP3::top (){
  send_command("TOP 2 100");
  get_response();
  printf("S: %s",buff);
}

void POP3::retr (){
  send_command("RETR 2");
  //char buff[2048] = {0};
  get_response();
  printf("S: %s",buff);
}

bool POP3::downloadMsg(std::string out_dir){
  char *temp;
  temp = new char[out_dir.length() + 1];
  strcpy(temp, out_dir.c_str());
  DIR *dir;
  dir = opendir(temp);
  if(dir){
    closedir(dir);
  }
  else {
    error("Otevreni slozky pro vystup se nezdrailo", 5);
  }
  stat();


  return true;
}

/*
* Posle dany prikaz na server
*/
bool POP3::send_command(std::string command){
  command = command.append("\r\n");
  std::cout << "C: " << command;
  int n = write(sock, command.c_str(), command.length());
  if(n < 0){
    error("Nepodarilo se poslat prikaz na server", 5);
  }
  return true;
}

/*
* Precte odpoved ze serveru
*/
 bool POP3::get_response(){
  memset(buff, 0, 2048); // vyprazdneni bufferu
  int n;
  n = read(sock, buff, sizeof(buff));
  if(n < 0){
   error("Nepodarilo se ziskat odpoved serveru", 5);
  }
  // while((n = read(sock, buff, sizeof(buff))) > 0){
  //   printf("S: %s",buff);
  // }
  // if(n < 0){
  //   error("Nepodarilo se ziskat odpoved serveru", 5);
  // }



  //std::cout << buff << "\n";
  return true;
}
