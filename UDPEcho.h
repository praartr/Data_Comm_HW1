/*********************************************************
*
* Module Name: UDP Echo client/server header file
*
* File Name:    UDPEcho.h	
*
* Summary:
*  This file contains common stuff for the client and server
*
* Revisions:
*
*********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>     /* for memset() */
#include <netinet/in.h> /* for in_addr */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <unistd.h>     /* for close() */


#define ECHOMAX 10000     /* Longest string to echo */
#define MSGMAX 60000
#ifndef LINUX
#define INADDR_NONE  0xffffffff
#endif

typedef struct Client
{
unsigned int SequenceNumber;
unsigned int sec;
unsigned int msec;
unsigned short MessageSize;
unsigned short SessionMode;
}ClientMsg;

typedef struct Server
{
unsigned int SequenceNumber;
unsigned int sec;
unsigned int msec;
unsigned short MessageSize;
unsigned short SessionMode;
}ServerMsg;

void DieWithError(char *errorMessage);  /* External error handling function */

