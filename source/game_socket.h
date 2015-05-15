#ifndef GAME_SOCKET_H
#define GAME_SOCKET_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include "game_info.h"

#define COMM_ERROR      -1
#define COMM_OK          0

#define COLL_ERROR      -1
#define COLL_COLSED      0
#define COLL_SEAT        1
#define COLL_GAME_OVER   2
#define COLL_BLIND       3
#define COLL_HOLD        4
#define COLL_INQUIRE     5
#define COLL_FLOP        6
#define COLL_TURN        7
#define COLL_RIVER       8
#define COLL_SHOWDOWN    9
#define COLL_POT_WIN     10

#define RE_COLL_ERROR   -1
#define RE_COLL_COLSED   0
#define RE_COLL_ACTION   1

#define ACTION_CHECK    0x01
#define ACTION_CALL     0x02
#define ACTION_RAISE    0x03
#define ACTION_ALL_IN   0x04
#define ACTION_FOLD     0x05

#define RE_ACTION_ERROR  -1
#define RE_ACTION_OK     0

#define CARD_2   2
#define CARD_3   2
#define CARD_4   2
#define CARD_5   2
#define CARD_6   2
#define CARD_7   2
#define CARD_8   2
#define CARD_9   2
#define CARD_10  2
#define CARD_J   2
#define CARD_Q   2
#define CARD_K   2
#define CARD_A   2

typedef struct startup_inform{
        struct sockaddr_in client_addr,service_addr;
        int pid_i;
        char pid[20];
        char pname[20];	
}startup_info;



int extr_check_info(int n,char *para[],startup_info * info);
int open_sdt_stream(char *log_name);
int close_sdt_stream(void);
int open_connect_socket(int *sockfd,startup_info * info);
int reg_player(int sockfd, const startup_info *info);
int collect_info(int sockfd, game_info *g_info, startup_info *s_info);
int att_action(int sockfd,int action,int num);
int close_socket(int sockfd);
#endif // GAME_SOCKET_H

