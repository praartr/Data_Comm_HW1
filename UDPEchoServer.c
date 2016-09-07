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

void DieWithError(char *errorMessage);  /* External error handling function */


char Version[] = "1.1";   

int main(int argc, char *argv[])
{
    int sock;                        /* Socket */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int cliAddrLen;         /* Length of incoming message */
    char echoBuffer[ECHOMAX];        /* Buffer for echo string */
    unsigned short echoServPort;     /* Server port */
    int recvMsgSize;                 /* Size of received message */
    double avgLossRate;
    unsigned int debugFlag;
    char * clientAddr[MSGMAX];
    unsigned short clientPort[MSGMAX];
    unsigned int numberOfClients;
    if (argc < 3 || argc > 4)         /* Test for correct number of parameters */
    {
        fprintf(stderr,"Usage:  %s <UDP SERVER PORT>\n", argv[0]);
        exit(1);
    }
     signal (SIGINT, serverCNTCCode);
    echoServPort = atoi(argv[1]);  /* First arg:  local port */
    
//$A0
    printf("UDPEchoServer(version:%s): Port:%d\n",(char *)Version,echoServPort);    
    if (argc == 3) {
       averageLossRate = atoi(argv[2]);
       debugFlag = 0;
    }
   else {
       averageLossRate = atof(argv[2]);
       debugFlag = atoi(argv[3]);
    }
    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
//        DieWithError("socket() failed");
      printf("Failure on socket call , errno:%d\n",errno);
    }

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0) {
//        DieWithError("bind() failed");
          printf("Failure on bind, errno:%d\n",errno);
    }
    
    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        cliAddrLen = sizeof(echoClntAddr);
        ServerMsg s_smsg, s_rmsg;
        /* Block until receive message from a client */
        if ((recvMsgSize = recvfrom(sock, &s_rmsg, sizeof(s_rmsg), 0,
            (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
        {
//           DieWithError("recvfrom() failed");
          printf("Failure on recvfrom, client: %s, errno:%d\n", inet_ntoa(echoClntAddr.sin_addr),errno);
        }
        // Storing Client's IP address
        clientAddr[numberOfClients++] = inet_ntoa(echoClntAddr.sin_addr);
        clientPort[numberOfClients++]]ntohs(echoClntAddr.sin_port);

        s_smsg.MessageSize = ntohs(s_rmsg.MessageSize);
        s_smsg.SessionMode = ntohs(s_rmsg.SessionMode);
        s_smsg.SequenceNumber = ntohl(s_rmsg.SequenceNumber); 
        s_smsg.sec = ntohl(s_rmsg.sec);
        s_smsg.msec = ntohl(s_rmsg.msec);
       // if Sequence number contains all 1's -  need to change this code
        if(s_rsmg_SequenceNumber == 11 )
         signal (SIGINT, serverCNTCCode);
        
        if(s_rmsg.SessionMode == 0) {
        /* Send received datagram back to the client */
        if (sendto(sock, echoBuffer, recvMsgSize, 0,  
             (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != recvMsgSize) {
//            DieWithError("sendto() sent a different number of bytes than expected");
          printf("Failure on sendTo, client: %s, errno:%d\n", inet_ntoa(echoClntAddr.sin_addr),errno);
        }
       }
     
    }
    /* NOT REACHED */
}


void serverCNTCCode() {
  unsigned int avgRate;

  
  if (numberOfTrials != 0) 
    avgRate = (numberOfTrials/(unsigned int)(totalRTT/1000000));
  else 
    avgRate = 0;
  if( mode == 0) 
   printf("\nTotal number of messages: %d \t Avg Sending Rate: %d bits/sec \t Min RTT: %f sec \t Max RTT: %f sec \t Mean RTT %f sec \t Standard Deviation RTT %f sec \n", numberOfTrials, avgRate, minRTT/1000000 , maxRTT/1000000, meanRTT/1000000, stdRTT/1000000);
  
  else
 printf("\nTotal number of messages: %d \t Avg Sending Rate: %d bits/sec \n", numberOfTrials, avgRate);
}
