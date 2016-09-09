/*********************************************************
* Module Name: UDP Echo client source 
*
* File Name:    UDPEchoClient2.c
*
* Summary:
*  This file contains the echo Client code.
*
* Revisions:
*
* $A0: 6-15-2008: misc. improvements
*
*********************************************************/
#include "UDPEcho.h"
#include <netdb.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>


void clientCNTCCode();
void CatchAlarm(int ignored);
int numberOfTimeOuts=0;
int numberOfTrials=0;
double totalRTT;
int bStop;
int mode;
double minRTT, maxRTT, meanRTT, stdRTT;

char Version[] = "1.1";   

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    struct sockaddr_in fromAddr;     /* Source address of echo */
   /*2.*/ unsigned short echoServPort;     /* Echo server port */
    unsigned int fromSize;           /* In-out of address size for recvfrom() */
   /*1.*/ char *servIP;                    /* IP address of server */
   // char *echoString;                /* String to send to echo server */
   // char echoBuffer[ECHOMAX+1];      /* Buffer for receiving echoed string */
   // int echoStringLen;               /* Length of string to echo */
    int respStringLen;               /* Length of received response */
    struct hostent *thehost;	     /* Hostent from gethostbyname() */
    double delay;		             /* Iteration delay in seconds */
    int packetSize;                  /* PacketSize*/
    struct timeval *theTime1;
    struct timeval *theTime2;
    struct timeval TV1, TV2;
    //int i;
    struct sigaction myaction;
    long usec1, usec2, curRTT;
    //int *seqNumberPtr;
    unsigned int seqNumber = 1;
    //unsigned int RxSeqNumber = 1;
    struct timespec reqDelay, remDelay;
    //int nIterations;
    unsigned int avgRate=0;//,loss;

    theTime1 = &TV1;
    theTime2 = &TV2;

    //Initialize values
    numberOfTimeOuts = 0;
    numberOfTrials = 0;
    totalRTT =0;
    bStop = 0;
  
    unsigned int averageRate;
    unsigned int bucketSize;
    unsigned int tokenSize;	
    unsigned int messageSize;
    
    unsigned int numberIterations;
    unsigned int debugFlag;


    if (argc < 3 || argc > 10 )    /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage: %s <Server IP> [<Server Port>] [<averageRate>][<bucketSize>][<tokenSize>][<messageSize>][<mode>][<numberIterations>] [<debugFlag>]\n", argv[0]);
        exit(1);
    }

    signal (SIGINT, clientCNTCCode);

    servIP = argv[1];           /* First arg: server IP address (dotted quad) */
    echoServPort = atoi(argv[2]);
    /* get info from parameters , or default to defaults if they're not specified */
    if (argc == 3) {
       
       averageRate = 1000000;
       bucketSize = 4*1472;
       tokenSize = 4*1472;
       messageSize = 1472;
       mode = 0;
       numberIterations = 1;
       debugFlag = 0;


    }
    
    else if (argc == 4) {
       averageRate = atoi(argv[3]);
       bucketSize = 4*1472;
       tokenSize = 4*1472;
       messageSize = 1472;
       mode = 0;
       numberIterations = 0;
       debugFlag = 0;
    }
    else if (argc == 5) {
     //  echoServPort = atoi(argv[2]);
       averageRate = atoi(argv[3]);
       bucketSize = atoi(argv[4]);
       tokenSize = 4*1472;
       messageSize = 1472;
       mode = 0;
       numberIterations = 0;
       debugFlag = 0;
    }
    else if (argc == 6) {
      //echoServPort = atoi(argv[2]);
      averageRate = atoi(argv[3]);
      bucketSize = atoi(argv[4]);
      tokenSize = atoi(argv[5]);
      messageSize = 1472;
      mode = 0;
      numberIterations = 0;
      debugFlag = 0;
    }
    else if (argc == 7) {
     // echoServPort = atoi(argv[2]);
      averageRate = atoi(argv[3]);
      bucketSize = atoi(argv[4]);
      tokenSize = atoi(argv[5]);
      messageSize =  atoi(argv[6]);
      mode = 0;
      numberIterations = 0;
      debugFlag = 0;
      
    }
    else if (argc == 8) {
     // echoServPort = atoi(argv[2]);
      averageRate = atoi(argv[3]);
      bucketSize = atoi(argv[4]);
      tokenSize = atoi(argv[5]);
      messageSize =  atoi(argv[6]);
      mode = atoi(argv[7]);
      numberIterations = 0;
      debugFlag = 0;
      
    }
    else if (argc == 9) {
      //echoServPort = atoi(argv[2]);
      averageRate = atoi(argv[3]);
      bucketSize = atoi(argv[4]);
      tokenSize = atoi(argv[5]);
      messageSize =  atoi(argv[6]);
      mode = atoi(argv[7]);
      numberIterations = atoi(argv[8]);
      debugFlag = 0;
    }
    else if (argc == 10) {
     // echoServPort = atoi(argv[2]);
      averageRate = atoi(argv[3]);
      bucketSize = atoi(argv[4]);
      tokenSize = atoi(argv[5]);
      messageSize =  atoi(argv[6]);
      mode = atoi(argv[7]);
      numberIterations = atoi(argv[8]);
      debugFlag = atoi(argv[9]);
      
    }
       
    myaction.sa_handler = CatchAlarm;
    if (sigfillset(&myaction.sa_mask) < 0)
       DieWithError("sigfillset() failed");

    myaction.sa_flags = 0;

    if (sigaction(SIGALRM, &myaction, 0) < 0)
       DieWithError("sigaction failed for sigalarm");

    /* Set up the echo string */
    /*
    echoStringLen = packetSize;
    echoString = (char *) echoBuffer;

    for (i=0; i<packetSize; i++) {
       echoString[i] = 0;
    }
    */
    double RTT[numberIterations];
    ClientMsg *c_smsg = malloc(sizeof(ClientMsg));
    ClientMsg *c_rmsg =  malloc(sizeof(ClientMsg));
    c_smsg->MessageSize = htons((unsigned short)messageSize);
    c_smsg->SessionMode = htons((unsigned short)mode);
    
    //seqNumberPtr = (int *)echoString;
   // echoString[packetSize-1]='\0';


    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
    
    /* If user gave a dotted decimal address, we need to resolve it  */
    if (echoServAddr.sin_addr.s_addr == -1) {
        thehost = gethostbyname(servIP);
	    echoServAddr.sin_addr.s_addr = *((unsigned long *) thehost->h_addr_list[0]);
    }
    
    echoServAddr.sin_port   = htons(echoServPort);     /* Server port */
printf("before while\n");
  while (numberIterations > 0) {

    //*seqNumberPtr = htonl(seqNumber++); 
    
    c_smsg->SequenceNumber = htonl(seqNumber++);
    /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");


    /* Send the string to the server */
    //printf("UDPEchoClient: Send the string: %s to the server: %s \n", echoString,servIP);
    gettimeofday(theTime1, NULL);
    c_smsg->sec = htonl((unsigned int)(theTime1->tv_sec*1000000));
    c_smsg->msec = htonl((unsigned int)(theTime1->tv_usec*1000000));
    printf("beforesendtoo\n");   
 if (sendto(sock, c_smsg, sizeof(*c_smsg), 0, (struct sockaddr *)
               &echoServAddr, sizeof(echoServAddr)) != sizeof(*c_smsg))
      DieWithError("sendto() sent a different number of bytes than expected");
 printf("AFTER SENDTO\n"); 
    /* Recv a response */

    fromSize = sizeof(fromAddr);
    alarm(4);            //set the timeout for 2 seconds
    printf("before recvfrom\n");
    if ((respStringLen = recvfrom(sock, c_rmsg, sizeof(*c_rmsg), 0,
         (struct sockaddr *) &fromAddr, &fromSize)) != sizeof(*c_smsg)) {
        if (errno == EINTR) 
        { 
           printf("Received a  Timeout !!!!!\n"); 
           numberOfTimeOuts++; 
           continue; 
        }
    }
printf("after recvfrom\n");
   // RxSeqNumber = ntohl(*(int *)echoBuffer);

    alarm(0);            //clear the timeout 
    gettimeofday(theTime2, NULL);

    usec2 = (theTime2->tv_sec) * 1000000 + (theTime2->tv_usec);
    usec1 = (theTime1->tv_sec) * 1000000 + (theTime1->tv_usec);

    curRTT = (usec2 - usec1);
   // printf("Ping(%d): %ld microseconds\n",RxSeqNumber,curPing);
    RTT[numberOfTrials] = curRTT;
    totalRTT += curRTT;
    if(totalRTT < minRTT) {
    minRTT = totalRTT;
    } 
   if(totalRTT > maxRTT) {
    maxRTT = totalRTT;
    } 
    numberOfTrials++;
    close(sock);

    reqDelay.tv_sec = delay;
    remDelay.tv_nsec = 0;
    nanosleep((const struct timespec*)&reqDelay, &remDelay);
    numberIterations--;
  }
  
  if (numberOfTrials != 0) {
    avgRate = (numberOfTrials/(unsigned int)(totalRTT/1000000));
    meanRTT = (totalRTT/numberOfTrials);
    float sum= 0.0;
    // Calculate Standard Deviation
	int i;
    for(i=0; i<numberOfTrials; i++)
    sum += (RTT[i]-meanRTT) * (RTT[i]-meanRTT);

    sum /= numberOfTrials;

    stdRTT = sum;
  }
  else 
    avgRate = 0;
  /*if (numberOfTimeOuts != 0) 
   // loss = ((numberOfTimeOuts*100)/numberOfTrials);
  else 
   // loss = 0;
*/
  printf("\nTotal number of messages: %d Avg Sending Rate: %d bits/sec\n", numberOfTrials, avgRate);
  
  exit(0);
}

void CatchAlarm(int ignored) { }

void clientCNTCCode() {
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
