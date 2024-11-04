#ifndef ENCRYPTORWRAPPER_H
#define ENCRYPTORWRAPPER_H

#ifdef __cpluscplus
extern "C" {
#endif

void init(const char* logpath);
int encrypt(const char* inputfile, const char* outputfile);

#ifdef __cpluscplus
}
#endif

#endif
