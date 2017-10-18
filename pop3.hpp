/*
** VUT FIT 2017 - ISA
** Klient POP3 s podporou TLS
** Eliska Kadlecova
** login: xkadle34
*/
#include <openssl/bio.h>

class POP3{
  public:
    POP3();
    bool connect_server(const char* server, int port);
  private:
    BIO *bio;
    SSL_CTX *ctx;
};
