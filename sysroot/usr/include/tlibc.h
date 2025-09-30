//FROM FILE COMES FROM TLIBC (tayoky 2025)
//SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef TLIBC_H
#define TLIBC_H

//this is reserved for internal use
//this should not be used by common program that use crt0.o

void __init_heap(void);
void __init_environ(int envc,char **envp);
void __init_tlibc(int argc,char **argv,int envc,char **envp);

#endif

_END_C_HEADER
