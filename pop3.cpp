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
  //printf("S: %s",buff);
  std::cout << "S: " << message;
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

  //printf("S: %s",buff);
  std::cout << "S: " << message;
  std::string sending_pass = "PASS ";
  sending_pass += password;

  send_command(sending_pass);
  get_response();
  //printf("S: %s",buff);
  std::cout << "S: " << message;

  return true;
}

/*
* Posle prikaz QUIT
*/
bool POP3::quit(){
  if(isdele == true){ //zpravy se mely smazat ze serveru
    std::cout << numMsg << " message(s) downloaded and deleted from server." << '\n';
  }
  else{
    std::cout << numMsg << " message(s) downloaded." <<'\n';
  }
  send_command("QUIT");
  get_response();
  //printf("S: %s",buff);
  std::cout << "S: " << message;
  return true;
}

/*
* Posle prikaz LIST k vypsani poctu zprav a jejich velikosti
*/
void POP3::messageList (bool new_only, std::string out_dir){
  send_command("STAT");
  get_response();
  //printf("S: %s",buff);
  std::cout << "S: " << message;
}

/*
* Posle prikaz STAT, overi, zda se provedl v poradku. Kdyz ne, konci, kdyz ano, vypreparuje pocet zprav ve schrance
*/
bool POP3::stat (){
  send_command("STAT");
  get_response();
  //printf("S: %s",buff);
  std::cout << "S: " << message;
  //char* temp = strstr(buff, "+OK ");
  std::size_t found = message.find("+OK ");
  if (found == std::string::npos){ //prikaz STAT vratil -ERR
    //std::cout << "not found" << '\n';
    return false;
  }
  else{
    //std::cout << "found +OK" << '\n';
    numMsg = stoi(message.substr(4, message.find_last_of(" "))); //ziskani cisla oznacujici pocet zprav
    //std::cout << "numMsg: " << numMsg <<'\n';
    return true;
  }
}

/*
* Posle prikaz DELE n
*/
void POP3::dele(){
  isdele = true;
  if(stat() == true){ //ziskani numMsg
    for(int a = 1; a <= numMsg; a++){ //cyklus, ktery posila prikaz DELE n pro smazani postupne vsech zprav
      send_command("DELE ", a);
      get_response();
      std::cout << "S: " << message;
    }
  }
}

/*
* Posle prikaz RETR n
*/
void POP3::retr (int a){
  isretr = true;
  send_command("RETR ", a);
  get_response();
  std::cout << "S: " << message;
  isretr = false;
}

/*
* Zjisti, zda je adresar platny, zavola funkci stat() pro zjisteni poctu prijatych zprav,
* v cyklu vola retr() pro nacitani zprav, ktere pak uklada do souboru do adresare
*/
bool POP3::downloadMsg(std::string out_dir){
  //std::cout << "downloadMsg" << '\n';
  char *temp;
  std::string path;
  std::string filename;

  temp = new char[out_dir.length() + 1];
  strcpy(temp, out_dir.c_str());

  DIR *dir;
  dir = opendir(temp);
  if(dir){
    closedir(dir);
  }
  else {
    error("Otevreni slozky pro vystup se nezdarilo", 5);
  }
  if(stat() == true){
    //std::cout << "stat is true" << '\n';
    for (int a = 1; a <= numMsg; a++) {
      retr(a);
      path = out_dir;
      filename = std::string("mail_").append(std::to_string(a)).append(".txt");
      path.append("/").append(filename);
      std::ofstream myFile(path); //vytvoreni souboru
      //std::cout << "saving this message: " << message << '\n';
      message = message.erase(0, message.find_first_of("\r\n") + 2); //odstraneni odpovedi serveru (+OK atd...)
      myFile << message;
    }
    //std::cout << numMsg << " message(s) downloaded." <<'\n';
    return true;
  }
  return false;
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
* Posle dany prikaz (s identifikatorem zpravy) na server
*/
bool POP3::send_command(std::string command, int num){
  command = command.append(std::to_string(num));
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
  //std::cout << "get_response" << '\n';
  message.clear();
  std::string reply;
  std::size_t pos;
  bool is_ok = true;
  while(is_ok){
    reply = read_from_server();
    //std::cout << "new reply: "<< reply << '\n';
    pos = is_end_of_message(reply);
    if(pos != std::string::npos){ //konec zpravy nalezen
      //std::cout << "END OF MESSAGE" << '\n';
      is_ok = false;
    }
    message += reply;
  }
  //std::cout << "MESSAGE: "<< message << '\n';
  return true;
}

/*
* Cte ze serveru do bufferu, vraci nactene znaky
*/
std::string POP3::read_from_server(){
  size_t num = 0;
  std::string reply;
  char *buff;
  buff = new char[BUFFSIZE];
  memset(buff, 0, 2048); // vyprazdneni bufferu
  num = read(sock, buff, BUFFSIZE);
  if(num <= 0){
    error("Nepodarilo se ziskat odpoved serveru", 5);
  }

  reply = buff;
  delete [] buff;
  return reply;
}

/*
* Overuje, zda uz je konec zpravy
* - pokud je isretr = true -> pak hleda tecku
* - pokud neni, jedna se o obycejnou zpravu od serveru a hleda CRLF
*/
size_t POP3::is_end_of_message(std::string msg){
  size_t position;
  if(isretr == true){
    position = msg.find("\r\n.\r\n");
    if(position != std::string::npos){
      //std::cout << "FOUND DOT" << '\n';
    }
  }
  else{
    position = msg.find("\r\n");
    //std::cout << "!!obycejna odpoved!!" << '\n';
  }
  return position;
}
