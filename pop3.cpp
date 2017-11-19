/*
** VUT FIT 2017 - ISA
** Klient POP3 s podporou TLS
** Eliska Kadlecova
** login: xkadle34
*/
#include <sys/socket.h>

#include "main.hpp"

POP3::POP3(){ //konstruktor
/* Initializing OpenSSL */
  SSL_load_error_strings();
  ERR_load_BIO_strings();
  OpenSSL_add_all_algorithms();
  SSL_library_init();
}

POP3::~POP3(){
}

/*
* Sifrovane pripojeni k serveru
*/
bool POP3::connect_server_sec(std::string server, int port, std::string cert_dir, std::string cert_file){
  secured = true;
  ctx = SSL_CTX_new(SSLv23_client_method());
  if(!ctx){
    error("Chyba pri CTX", 7);
  }

  if (cert_file != "") {
    if(!SSL_CTX_load_verify_locations(ctx, cert_file.c_str(), NULL)){
      error("Soubor s TLS certifikaty nenalezen", 8);
    }
  }
  else if (cert_dir != "") {
    if(!SSL_CTX_load_verify_locations(ctx, NULL, cert_dir.c_str())){
      error("Slozka s TLS certifikaty nenalezena", 8);
    }
  }
  else{
    SSL_CTX_set_default_verify_paths(ctx);
  }

  bio = BIO_new_ssl_connect(ctx);
  BIO_get_ssl(bio, & ssl);
  SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

  std::string hostname = server + ":" + std::to_string(port);
  BIO_set_conn_hostname(bio, hostname.c_str());
  if(BIO_do_connect(bio) <= 0){
    error("BIO pripojeni se nezdarilo", 8);
  }
  if(SSL_get_peer_certificate(ssl) == NULL){
    error("Server nevratil zadny SSL certifkat", 8);
  }
  if(SSL_get_verify_result(ssl) != X509_V_OK){
    error("Overeni platnosti certifikatu SSL se nezdarilo.", 8);
  }
  get_response();
  // std::cout << "S: " << message;
  return true;
}

/*
* Pripojeni k serveru
*/
bool POP3::connect_server(std::string server, int port){
  secured = false;
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
  // std::cout << "S: " << message;
  if(isOK() == true){
    return true; //connect se podarilo
  }
  else{
    return false; //connect se nepodarilo
  }
}

bool POP3::isOK(){
  if(message.find("+OK ") != std::string::npos){
    return true;
  }
  else{
    return false;
  }
}

/*
* Posle prikazy k prihlaseni, vraci true, kdyz se podarilo
*/
bool POP3::login(std::string username, std::string password){
  std::string sending_user = "USER ";
  sending_user += username;

  if(secured){
    send_command_sec(sending_user);
  }
  else{
    send_command(sending_user);
  }
  get_response();
  if(isOK() == true){

      // std::cout << "S: " << message;
      std::string sending_pass = "PASS ";
      sending_pass += password;

      if(secured){
        send_command_sec(sending_pass);
      }
      else{
        send_command(sending_pass);
      }
      get_response();
      // std::cout << "S: " << message;
      if(isOK() == true){
        return true;
      }
      else{
        return false;
      }
  }
  else{
    return false;
  }
}

/*
* Posle prikaz QUIT
*/
bool POP3::quit(){
  if(isdele == true){ //zpravy se mely smazat ze serveru
    std::cout << "Stazeno a smazano zprav: " << numMsg <<'\n';
  }
  else{
    std::cout << "Stazeno zprav: " << numMsg <<'\n';
  }

  if(secured){
    send_command_sec("QUIT");
  }
  else{
    send_command("QUIT");
  }
  get_response();
  // std::cout << "S: " << message;
  if(isOK() == true){
    return true;
  }
  else{
    return false;
  }
}

void POP3::finish(){
  if(secured){
    BIO_reset(bio);
    SSL_CTX_free(ctx);
    BIO_free_all(bio);
    bio = NULL;
  }
}

/*
* Posle prikaz STAT, overi, zda se provedl v poradku. Kdyz ne, konci, kdyz ano, vypreparuje pocet zprav ve schrance
*/
bool POP3::stat (){
  if(secured){
    send_command_sec("STAT");
  }
  else{
    send_command("STAT");
  }
  get_response();
  // std::cout << "S: " << message;
  if(isOK() == true){
    numMsg = stoi(message.substr(4, message.find_last_of(" ")));
    return true;
  }
  else{
    return false; //prikaz STAT vratil -ERR
  }
}

/*
* Posle prikaz DELE n
*/
bool POP3::dele(){
  isdele = true;
  if(stat() == true){ //ziskani numMsg
    for(int a = 1; a <= numMsg; a++){ //cyklus, ktery posila prikaz DELE n pro smazani postupne vsech zprav

      if(secured){
        send_command_sec("DELE ", a);
      }
      else{
        send_command("DELE ", a);
      }
      get_response();
      // std::cout << "S: " << message;
      if(isOK() == true){
        return true;
      }
      else{
        return false;
      }
    }
  }
  return false;
}

/*
* Posle prikaz RETR n
*/
bool POP3::retr (int a){
  isretr = true;
  if(secured){
    send_command_sec("RETR ", a);
  }
  else{
    send_command("RETR ", a);
  }
  get_response();
  // std::cout << "S: " << message;
  isretr = false;
  if(isOK() == true){
    return true;
  }
  else{
    return false;
  }

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
    for (int a = 1; a <= numMsg; a++) {
      if(retr(a) == true){
        path = out_dir;
        filename = std::string("mail_").append(std::to_string(a)).append(".txt");
        path.append("/").append(filename);
        std::ofstream myFile(path); //vytvoreni souboru
        message = message.erase(0, message.find_first_of("\r\n") + 2); //odstraneni odpovedi serveru (+OK atd...)
        size_t pos = message.find(".\r\n");
        message = message.erase(pos, pos + 3);
        myFile << message;
      }
      else{
        return false;
      }
    }
    return true;
  }
  return false;
}

/*
* Posle dany prikaz na server
*/
bool POP3::send_command(std::string command){
  command = command.append("\r\n");
  //std::cout << "C: " << command;
  int n = write(sock, command.c_str(), command.length());
  if(n < 0){
    error("Nepodarilo se poslat prikaz na server", 5);
  }
  return true;
}

/*
* Posle dany prikaz na server
*/
bool POP3::send_command_sec(std::string command){
  command = command.append("\r\n");
  //std::cout << "C: " << command;
  int n = BIO_write(bio, command.c_str(), (int)command.length());
  if(n <= 0){
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
  //std::cout << "C: " << command;
  int n = write(sock, command.c_str(), command.length());
  if(n < 0){
    error("Nepodarilo se poslat prikaz na server", 5);
  }
  return true;
}

/*
* Posle dany prikaz (s identifikatorem zpravy) na server
*/
bool POP3::send_command_sec(std::string command, int num){
  command = command.append(std::to_string(num));
  command = command.append("\r\n");
  //std::cout << "C: " << command;
  int n = BIO_write(bio, command.c_str(), (int)command.length());
  if(n <= 0){
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
    if(secured){
      reply = read_from_server_sec();
    }
    else{
      reply = read_from_server();
    }
    pos = is_end_of_message(reply);
    if(pos != std::string::npos){ //konec zpravy nalezen
      is_ok = false;
    }
    message += reply;
  }
  return true;
}

/*
* Cte ze serveru do bufferu, vraci nactene znaky
*/
std::string POP3::read_from_server(){
  size_t num = 0;
  std::string reply;
  char *buff;
  buff = new char[1024];
  memset(buff, 0, 1024); // vyprazdneni bufferu
  num = read(sock, buff, 1024);
  if(num <= 0){
    error("Nepodarilo se ziskat odpoved serveru", 5);
  }

  reply = buff;
  delete [] buff;
  return reply;
}

/*
* Cte ze serveru do bufferu, vraci nactene znaky
*/
std::string POP3::read_from_server_sec(){
  int num = 0;
  std::string reply;
  char *buff;
  buff = new char[1024];
  memset(buff, 0, 2048); // vyprazdneni bufferu
  num = BIO_read(bio, buff, 1024);
  if(num == 0){
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
  }
  return position;
}
