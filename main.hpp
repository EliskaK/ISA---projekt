/*
** VUT FIT 2017 - ISA
** Klient POP3 s podporou TLS
** Eliska Kadlecova
** login: xkadle34
*/
#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <string.h>

#include "pop3.hpp"

int main(int argc, char *argv[]);
struct prepinace savingParams(int argc, char *argv[]);
std::string getoutdir();
char *parseArg(char ** begin, char ** end, const std::string &opt, bool value);
void error (std::string zprava, int error_code);
void help();
