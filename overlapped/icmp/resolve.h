/*********************
Common routines for resolving address and hostnames

Files:
      resolv.h - header file for common routines

Description:
      This file contains common name resolution and name printing
      routines and is used by many of samples.

Compile:
      See ping.cpp

Usage:
      See ping.cpp
 *********************/

#ifndef _RESOLVE_H_
#define _RESOLVE_H_

#ifdef _cpluscplus
extern "C"{
#endif

  int PrintAddress(SOCKADDR *sa, int salen);
  int FormatAdderss(SOCKADDR *sa, int salen, char *addrbuf, int addrbuflen);
  int ReverseLookup(SOCKADDR *sa, int salen, char *namebuf, int namebuflen);
  struct addrinfo *ResolveAddress(char *addr, char *port, int af, int type, int proto);

#ifdef _cplusplus
}
#endif

#endif
