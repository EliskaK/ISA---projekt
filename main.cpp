/*
** VUT FIT 2017 - ISA
** Klient POP3 s podporou TLS
** Eliska Kadlecova
** login: xkadle34
*/
#include <iostream>

int main(int argc, char const *argv[]) {
  //test na pocet argumentu
  if (argc < 2){
    error("Zadano malo argumentu. Pouzijte parametr --help pro zobrazeni napovedy", 1);
  }
  //pocet argumentu je OK
  else{
    //TODO
  }
  return 0;
}

/*
* Parsovani zadanych argumentu
*/
char *parseArg(const std::string &opt, bool value){
  char ** end = argv + argc;
  char ** begin = argv;
  char ** itr = std::find(begin, end, opt);
  if(value == true){
    if (itr != end){
      if (++itr != end){
        return *itr;
      }
      else {
        error("Argument " + opt + "ma zadano malo parametru.", 2);
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
  exit(0);
}
