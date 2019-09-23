/*///////////////////////////////////////////////////////////
 *
 * FILE:		client.c
 * AUTHOR:	Abdulrahman Khalid Alkhaldi
 * PROJECT:	CS 3251 Project 1 - Professor Jun Xu
 * DESCRIPTION:	Network Client Code
 * CREDIT:	Adapted from Professor Traynor
 *
 */
///////////////////////////////////////////////////////////

/* Included libraries */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* supports all sorts of functionality */
#include <string.h>     /* support any string ops */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <unistd.h>     /* for close() */

/* Constants */
#define RCVBUFSIZE 512 /* The receive buffer size */
#define SNDBUFSIZE 512 /* The send buffer size */
#define REPLYLEN 32

/* Terminate program function */
int end() {
  printf("Connection terminated...\n");
  exit(0);
  return 0;
}

/* The main function */
int main(int argc, char *argv[]) {
  int clientSock;               /* socket descriptor */
  struct sockaddr_in serv_addr; /* server address structure */

  // For storing arguments
  char *msgType;
  char *accountName1, *accountName2;
  int account1Index, account2Index = -1, amount = 1;

  char *servIP;            /* Server IP address  */
  unsigned short servPort; /* Server Port number */

  char sndBuf[SNDBUFSIZE]; /* Send Buffer */
  char rcvBuf[RCVBUFSIZE]; /* Receive Buffer */

  memset(&sndBuf, 0, SNDBUFSIZE);
  memset(&rcvBuf, 0, RCVBUFSIZE);

  /* Get the addditional parameters from the command line */
  msgType = argv[1];

  accountName1 = argv[2];

  if (strcmp(accountName1, "myChecking") == 0) {
    account1Index = 0;
  } else if (strcmp(accountName1, "mySavings") == 0) {
    account1Index = 1;
  } else if (strcmp(accountName1, "myCD") == 0) {
    account1Index = 2;
  } else if (strcmp(accountName1, "my401k") == 0) {
    account1Index = 3;
  } else if (strcmp(accountName1, "my529") == 0) {
    account1Index = 4;
  } else {
    account1Index = -1;  
    printf("Error: invalid account name.\n");
    end();
  }

  /* BAL */
  if (strcmp(msgType, "BAL") == 0 && argc == 5) {
    // servIP = gethostbyname(argv[3]);
    servPort = atoi(argv[4]);
  } 
  /* WITHDRAW */
  else if (strcmp(msgType, "WITHDRAW") == 0 && argc == 6) {
    amount = atoi(argv[3]);
    // servIP = gethostbyname(argv[4]);
    servPort = atoi(argv[5]);
  }
  /* TRANSFER */
  else if (strcmp(msgType, "TRANSFER") == 0 && argc == 7) {
    accountName2 = argv[3];

    if (strcmp(accountName2, "myChecking") == 0) {
      account2Index = 0;
    } else if (strcmp(accountName2, "mySavings") == 0) {
      account2Index = 1;
    } else if (strcmp(accountName2, "myCD") == 0) {
      account2Index = 2;
    } else if (strcmp(accountName2, "my401k") == 0) {
      account2Index = 3;
    } else if (strcmp(accountName2, "my529") == 0) {
      account2Index = 4;
    } else {
      account2Index = -1;
    }

    if (strcmp(accountName1, accountName2) == 0) {
      printf("Error: you can't transfer money to the same account.\n");
      end();
    }

    amount = atoi(argv[4]);
    // servIP = gethostbyname(argv[5]);
    servPort = atoi(argv[6]);
  } else {
    printf("Error: invalid arguments.\n");
    end();
  }

  if ( (strcmp(msgType, "WITHDRAW") == 0 || strcmp(msgType, "TRANSFER") == 0) && amount <= 0) {
      printf("Error: invalid amount.\n");
      end();
  }

  snprintf(sndBuf, sizeof(sndBuf), "%s-%d-%d-%d", msgType, account1Index, account2Index, amount);

  /* Create a new TCP socket*/
  clientSock = socket(AF_INET, SOCK_STREAM, 0);

  /* Construct the server address structure */
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(servPort);
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  /* Establish connecction to the server */
  int connection_status = connect(clientSock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (connection_status == -1) end();

  /* Send the string to the server */
  send(clientSock, sndBuf, sizeof(sndBuf), 0);

  /* Receive and print response from the server */
  recv(clientSock, &rcvBuf, sizeof(rcvBuf), 0);
  printf("%s \n", rcvBuf);

  return 0;
}