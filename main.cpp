/*
** VUT FIT 2017 - ISA
** Klient POP3 s podporou TLS
** Eliska Kadlecova
** login: xkadle34
*/
#include <iostream>
#include <string>
#include <algorithm>

#include "main.hpp"
#include "pop3.hpp"

/*
* Struktura, kde jsou uchovany vsechny prepinace
*/
struct prepinace {
  std::string server;
  int port;
  bool pop3s = false; //-T
  bool stls = false; //-S
  std::string certfile;
  std::string certaddr;
  bool new_only = false;
  bool del = false; //delete
  std::string auth_file;
  std::string out_dir;
};

/*
* Podle zvolenych prepinacu se udela konfigurace
*/
struct prepinace savingParams(int argc, char *argv[]) {
  struct prepinace prepinac;

  //test na pocet argumentu
  if (argc < 2){
    error("Zadano malo argumentu. Pouzijte parametr --help pro zobrazeni napovedy", 1);
  }
  if (parseArg(argv, argv + argc, "--help", false)){
    help();
  }
  prepinac.server = argv[1];
  std::cout << "server: " << prepinac.server <<'\n';

  char * cf; //certfile
  char * ca; //certaddr
  //parametr -T
  if(parseArg(argv, argv + argc, "-T", false)){
    prepinac.pop3s = true;
    cf = parseArg(argv, argv + argc, "-c", true);
    ca = parseArg(argv, argv + argc, "-C", true);
    if(cf){
      prepinac.certfile = cf;
    }
    if(ca){
      prepinac.certaddr = ca;
    }
    else{
      //TODO - SSL_CTX_set_default_verify_paths()
    }
  }
  std::cout << "T: " << prepinac.pop3s <<'\n';

  //parametr -S
  if(parseArg(argv, argv + argc, "-S", false)){
    prepinac.stls = true;
    cf = parseArg(argv, argv + argc, "-c", true);
    ca = parseArg(argv, argv + argc, "-C", true);
    if(cf){
      prepinac.certfile = cf;
    }
    if(ca){
      prepinac.certaddr = ca;
    }
    else{
      //TODO - SSL_CTX_set_default_verify_paths()
    }
  }
  std::cout << "S: " << prepinac.stls <<'\n';

  std::cout << "certfile: " << prepinac.certfile <<'\n';
  std::cout << "certaddr: " << prepinac.certaddr <<'\n';

  //port:
  char *p;
  p = parseArg(argv, argv + argc, "-p", true);
  if (p){
    prepinac.port = std::atoi(p);
  }
  else if (prepinac.pop3s == true){ //pokud je zadan parametr -T
    prepinac.port = 995;
  }
  else{
    prepinac.port = 110;
  }

  std::cout << "port: " << prepinac.port <<'\n';

  //nove zpravy
  if (parseArg(argv, argv + argc, "-n", false)){
    prepinac.new_only = true;
  }
  std::cout << "new_only: " << prepinac.new_only <<'\n';

  if (parseArg(argv, argv + argc, "-d", false)){
    prepinac.del = true;
  }
  std::cout << "del: " << prepinac.del <<'\n';

  //auth_file
  char * auth;
  auth = parseArg(argv, argv + argc, "-a", true);
  if (auth){
    prepinac.auth_file = auth;
  }
  else{
    error("Povinny parametr -a chybi", 2);
  }
  std::cout << "auth_file: " << prepinac.auth_file <<'\n';

  //out_dir
  char * out;
  out = parseArg(argv, argv + argc, "-o", true);
  if (out){
    prepinac.out_dir = out;
  }
  else{
    error("Povinny parametr -o chybi", 2);
  }
  std::cout << "out_dir: " << prepinac.out_dir <<'\n';

  return prepinac;
}

/*
* Parsovani zadanych argumentu
*/
char* parseArg(char ** begin, char ** end, const std::string & opt, bool value){
  char ** itr = std::find(begin, end, opt);
  if(value == true){
    if (itr != end){
      if (++itr != end){
        return *itr;
      }
      else {
        error("Argument " + opt + " ma zadano malo parametru.", 2);
      }
    }
  }
  else if( itr != end){
    return (char *) 1;
  }
  return 0;
}

/*
* Tiskne chybovou hlasku na err a ukonci se s danym chybovym kodem
*/
void error (std::string zprava, int error_code){
  std::cerr << zprava << std::endl;
  exit(error_code);
}

/*
* Vytiskne napovedu a ukonci se
*/
void help(){
  std::cout << "Klient POP3 s podporou TLS" << '\n';
  std::cout << "Pouziti:" << '\n';
  std::cout << "popcl --help ---> zobrazi tuto napovedu a ukonci se" << '\n';
  std::cout << "popcl server  [-p <port>] [-T|-S [-c <certfile>]" << '\n';
  std::cout << "[-C <certaddr>]] [-d] [-n] -a <auth_file> -o <out_dir>" << '\n';
  std::cout << "server          - povinny parametr (IP adresa nebo domenove jmeno)" << '\n';
  std::cout << "-p <port>       - specifikuje cislo portu na serveru" << '\n';
  std::cout << "-T              - zapina sifrovani" << '\n';
  std::cout << "-S              - navaze nesifrovane spojeni se serverem a pomoci prikazu" << '\n';
  std::cout << "                  STLS prejde na sifrovanou verzi protokolu" << '\n';
  std::cout << "-c <certfile>   - definuje soubor s certifikaty" << '\n';
  std::cout << "-C <certaddr>   - definuje adresar ve kterem se maji vyhledavat certifikaty" << '\n';
  std::cout << "-d              - smaze zpravy" << '\n';
  std::cout << "-n              - stahnou se pouze nove zpravy" << '\n';
  std::cout << "-a <auth_file>  - povinny parametr, vynucuje autentizaci" << '\n';
  std::cout << "-o <out_dir>    - povinny parametr, specifikuje vystupni adresar" << '\n';
  exit(0);
}

int main(int argc, char *argv[]) {
  struct prepinace konfigurace = savingParams(argc, argv);
  //std::cout << konfigurace.port << '\n';
  POP3 pop = POP3();

  if(konfigurace.pop3s == true){ // -T
    pop.connect_server_sec();
  }
  else{
    pop.connect_server(konfigurace.server, konfigurace.port);
  }
  return 0;
}
