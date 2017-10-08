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

/*
* Struktura, kde jsou uchovany vsechny prepinace
*/
struct prepinac {
  std::string server;
  int port;
  std::string certfile;
  std::string certaddr;
  bool new_only;
  bool del; //delete
  std::string auth_file;
  std::string out_dir;
}

/*
*
*/
int main(int argc, char *argv[]) {
  //test na pocet argumentu
  if (argc < 2){
    error("Zadano malo argumentu. Pouzijte parametr --help pro zobrazeni napovedy", 1);
  }
  if (parseArg(argv, argv + argc, "--help", false)){
    help();
  }
  //TODO poznat server
  //pocet argumentu je OK
  else{
    //TODO
  }
  return 0;
}

/*
*
*/


/*
* Parsovani zadanych argumentu
*/
char* parseArg(char ** begin, char ** end, const std::string & opt, bool value){
  /*char ** end = argv + argc;
  char ** begin = argv;*/
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
