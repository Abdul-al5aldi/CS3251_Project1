/*///////////////////////////////////////////////////////////
 *
 * FILE:		server.c
 * AUTHOR:	Abdulrahman Khalid Alkhaldi
 * PROJECT:	CS 3251 Project 1 - Professor Jun Xu
 * DESCRIPTION:	Network Server Code
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

/* Additional libraries */
#include <netinet/in.h>
#include <sys/types.h>
#include <time.h>

/* Constants */
#define RCVBUFSIZE 512 /* The receive buffer size */
#define SNDBUFSIZE 512 /* The send buffer size */
#define BUFSIZE 40     /* Your name can be as many as 40 chars*/
int withdrawTimes[3] = {-1, -1, -1};

/* Terminate program function */
int end() {
  printf("Connection terminated...\n");
  exit(0);
  return 0;
}

/* Time handler function */
int getWithdrawsCount() {
  time_t timeNow = time(NULL);

  for (int i = 0; i < 3; i++) {
    int t = withdrawTimes[i];
    if (timeNow - t >= 60) {
      withdrawTimes[i] = -1;
    }
  }

  for (int i = 0; i < 3; i++) {
    for (int j = i + 1; j < 3; j++) {
      if (withdrawTimes[i] > withdrawTimes[j]) {
        int tmp = withdrawTimes[i];
        withdrawTimes[i] = withdrawTimes[j];
        withdrawTimes[j] = tmp;
      }
    }
  }

  for (int i = 0; i < 3; i++) {
    if (withdrawTimes[i] != -1) {
      return 3 - i;
    }
  }
}

struct account {
  char name[BUFSIZE];
  int balance;
  int index;
};

/* The main function */
int main(int argc, char *argv[]) {

  int serverSock;                    /* Server Socket */
  int clientSock;                    /* Client Socket */
  struct sockaddr_in changeServAddr; /* Local address */
  struct sockaddr_in changeClntAddr; /* Client address */
  unsigned short changeServPort;     /* Server port */
  unsigned int clntLen;              /* Length of address data struct */

  char nameBuf[BUFSIZE];   /* Buff to store account name from client */
  char sndBuf[SNDBUFSIZE]; /* Send Buffer */
  int balance1;            /* Place to record account balance result */
  int balance2;

  int account1Index, account2Index = -1, amount, withdrawsCount;
  char *accountName1, *accountName2;

  struct account a1 = {"myChecking", 200, 0};
  struct account a2 = {"mySavings", 200, 1};
  struct account a3 = {"myCD", 200, 2};
  struct account a4 = {"my401k", 200, 3};
  struct account a5 = {"my529", 200, 4};

  struct account accounts[5] = {a1, a2, a3, a4, a5};

  /* Create new TCP Socket for incoming requests*/
  serverSock = socket(AF_INET, SOCK_STREAM, 0);

  /* Construct local address structure*/
  changeServPort = atoi(argv[1]);
  changeServAddr.sin_family = AF_INET;
  changeServAddr.sin_port = htons(changeServPort);
  changeServAddr.sin_addr.s_addr = INADDR_ANY;

  /* Bind to local address structure */
  if (bind(serverSock, (struct sockaddr *)&changeServAddr, sizeof(changeServAddr)) != 0)
    end();

  /* Listen for incoming connections */
  if (listen(serverSock, 5) != 0) end();

  /* Loop server forever*/
  while (1) {
    /* Accept incoming connection */
    clientSock = accept(serverSock, NULL, NULL);
    if (clientSock < 0) end();

    /* Extract the account name from the packet, store in nameBuf */
    read(clientSock, nameBuf, sizeof(nameBuf));

    char space[] = "-";
    char *msgType = strtok(nameBuf, space);
    account1Index = atoi(strtok(NULL, space));
    account2Index = atoi(strtok(NULL, space));
    amount = atoi(strtok(NULL, space));

    accountName1 = accounts[account1Index].name;
    balance1 = accounts[account1Index].balance;

    /* BAL */
    if (strcmp(msgType, "BAL") == 0) {
      printf("- Request for balance of account [%s]. / succeed, balance = [$%d].\n", accountName1, balance1);
      snprintf(sndBuf, sizeof(sndBuf), "Account [%s] has balance of = [$%d].\n", accountName1, balance1);
    }

    /* WITHDRAW */
    else if (strcmp(msgType, "WITHDRAW") == 0) {
      printf("- Request for withdraw [$%d] from account [%s]. / ", amount, accountName1);

      int withdrawsCount = getWithdrawsCount();
      if (withdrawsCount >= 3) {
        printf("failed, too many withdraws in a minute.\n");
        snprintf(sndBuf, sizeof(sndBuf), "Error: too many withdraws in a minute, please try again later.\n");
      } else {
        if (amount > balance1) {
          printf("failed, insufficient funds.\n");
          snprintf(sndBuf, sizeof(sndBuf), "Error: insufficient funds.\n");
        } else {
          time_t timeNow = time(NULL);
          for (int i = 0; i < 3; i++) {
            if (withdrawTimes[i] == -1) {
              withdrawTimes[i] = timeNow;
              break;
            }
          }

          accounts[account1Index].balance = balance1 - amount;
          balance1 = accounts[account1Index].balance;
          printf("succeed, new balance = [$%d].\n", balance1);
          snprintf(sndBuf, sizeof(sndBuf), "Withdraw done successfully, new balance for account [%s] = [$%d].\n", accountName1, balance1);
        }
      }
    }

    /* TRANSFER */
    else if (strcmp(msgType, "TRANSFER") == 0) {
      accountName2 = accounts[account2Index].name;
      balance2 = accounts[account2Index].balance;
      printf("- Request for transfer [$%d] from account [%s] to [%s]. / ", amount, accountName1, accountName2);

      if (amount > balance1) {
        printf("failed, insufficient funds.\n");
        snprintf(sndBuf, sizeof(sndBuf), "Error: insufficient funds.\n");
      } else {
        accounts[account1Index].balance = balance1 - amount;
        accounts[account2Index].balance = balance2 + amount;

        balance1 = accounts[account1Index].balance;
        balance2 = accounts[account2Index].balance;

        printf("succeed, new balance for account [%s] = [$%d] & for [%s] = [$%d].\n", accountName1, balance1, accountName2, balance2);
        snprintf(sndBuf, sizeof(sndBuf), "Transfer done successfully, new balance for account [%s] = [$%d] & for [%s] = [$%d].\n", accountName1, balance1, accountName2, balance2);
      }
    } else
      end();

    /* Return to client */
    send(clientSock, sndBuf, sizeof(sndBuf), 0);
  }
}
