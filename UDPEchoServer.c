/*********************************************************
*
* Module Name: UDP Echo server 
*
* File Name:    UDPEchoServer.c	
*
* Summary:
*  This file contains the echo server code
*
* Revisions:
*  $A0:  6/15/2008:  don't exit on error
*
*********************************************************/
#include "UDPEcho.h"
#include <netdb.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <arpa/inet.h>

void serverCNTCCode();
void CatchAlarm(int ignored);

void DieWithError(char *errorMessage);  /* External error handling function */

double currRTT;
unsigned int TotalBytes;
char *clientAddr;
unsigned int clientPort;
char Version[] = "1.1";   

int main(int argc, char *argv[])
{
    int sock;                        /* Socket */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    int cliAddrLen;         /* Length of incoming message */
    char echoBuffer[ECHOMAX];        /* Buffer for echo string */
    unsigned short echoServPort;     /* Server port */
    int recvMsgSize;                 /* Size of received message */
    unsigned int averageLossRate;
    unsigned int debugFlag;
    double usec1, usec2;
    struct timeval *theTime1;
    struct timeval *theTime2;
    unsigned int numberOfClients;
    if (argc < 2 || argc > 4)         /* Test for correct number of parameters */
    {
        fprintf(stderr,"Usage:  %s <UDP SERVER PORT>\n", argv[0]);
        exit(1);
    }
     signal (SIGINT, serverCNTCCode);
printf("hi");    
    if(argc ==2){
      echoServPort = atoi(argv[1]);  /* First arg:  local port */
      averageLossRate = 0;
    }
//$A0
    printf("UDPEchoServer(version:%s): Port:%d\n",(char *)Version,echoServPort);    
    if (argc == 3) {
       averageLossRate = atoi(argv[2]);
       debugFlag = 0;
    }
   else if(argc ==4) {
       averageLossRate = atoi(argv[2]);
       debugFlag = atoi(argv[3]);
    }
   printf("After parameter\n");
    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
//        DieWithError("socket() failed");
      printf("Failure on socket call , errno:%d\n",errno);
    }
    printf("after socket\n");
    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    printf("after memset\n");    
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = malloc(sizeof(unsigned long));
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0) {
//        DieWithError("bind() failed");
          printf("Failure on bind, errno:%d\n",errno);
    }
    printf("after bind\n");
    Server *s_smsg = (Server*)malloc(sizeof(Server));
    Server *s_rmsg = (Server*)malloc(sizeof(Server));
    for (;;) /* Run forever */
    {  printf("entering for loop\n");
        /* Set the size of the in-out parameter */
       cliAddrLen = sizeof(echoClntAddr);
       printf("After cliaddrlen\n");
      //  ServerMsg s_smsg, s_rmsg;
        /* Block until receive message from a client */
       
       gettimeofday(theTime1, NULL);
       printf("%d\n",sizeof(s_rmsg));
       printf("before recv from\n"); 
       alarm(2);
       
       if(recvMsgSize = recvfrom(sock, s_rmsg, sizeof(*s_rmsg), 0,
            (struct sockaddr *) &echoClntAddr, &cliAddrLen) < 0)
        {
          printf("hi\t hi");
          DieWithError("recvfrom() failed");
         printf("Failure on recvfrom, client: %s, errno:%d\n", inet_ntoa(echoClntAddr.sin_addr),errno);
        }
        alarm(0);
        
        printf("AFTER RECVFROM: recvMsgSize = %d",recvMsgSize);
        // Storing Client's IP address
        clientAddr = inet_ntoa(echoClntAddr.sin_addr);
        clientPort = ntohs(echoClntAddr.sin_port);

        s_smsg->MessageSize = ntohs(s_rmsg->MessageSize);
        TotalBytes += s_smsg->MessageSize;
        s_smsg->SessionMode = ntohs(s_rmsg->SessionMode);
        s_smsg->SequenceNumber = ntohl(s_rmsg->SequenceNumber); 
        s_smsg->sec = ntohl(s_rmsg->sec);
        s_smsg->msec = ntohl(s_rmsg->msec);
       // if Sequence number contains all 1's -  need to change this code
        if(ntohl(s_rmsg->SequenceNumber) == 11 )
         signal (SIGINT, serverCNTCCode);
        
        if(s_rmsg->SessionMode == 0) {
        /* Send received datagram back to the client */
        if (sendto(sock,s_smsg,sizeof(*s_smsg), 0,  
             (struct sockaddr *) &echoClntAddr, cliAddrLen) != sizeof(*s_smsg)) {
//            DieWithError("sendto() sent a different number of bytes than expected");
          printf("Failure on sendTo, client: %s, errno:%d\n", inet_ntoa(echoClntAddr.sin_addr),errno);
        }
		alarm(0);            //clear the timeout 
    gettimeofday(theTime2, NULL);

    usec1 = (theTime2->tv_sec) * 1000000 + (theTime2->tv_usec);
    usec2 = (theTime1->tv_sec) * 1000000 + (theTime1->tv_usec);

    currRTT = (usec2 - usec1);
       }
     
    }
    /* NOT REACHED */
}


void serverCNTCCode() {
  
     printf("\n %d \t  %d  \n  %s \t %d \t %l \t %d \t %l \n", 1,TotalBytes, clientAddr, clientPort, currRTT, TotalBytes,(TotalBytes*8)/currRTT);
exit(0);  
}
