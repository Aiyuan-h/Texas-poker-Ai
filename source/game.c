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
#include "game_socket.h"
#include "game_action.h"
#include "game_info.h"
333333333333333333333333
444444444444444444444444444444444

int main(int argc,char * argv[])
{
    startup_info s_info;
    game_info g_info;
    int action;
    int bet;

    int sockfd = 0;

    open_sdt_stream(argv[5]);

    if(extr_check_info(argc,argv,&s_info) != 0){
        printf("File:%s Line:%d Function %s:  ",__FILE__,__LINE__,__FUNCTION__);
        printf("extr_check_info Error!!\n");
        close_sdt_stream();
        return -1;
    }

    if(open_connect_socket(&sockfd,&s_info) != 0){
        printf("File:%s Line:%d Function %s:  ",__FILE__,__LINE__,__FUNCTION__);
        printf("open_connect_socket Error!!\n");
        close_sdt_stream();
        return -1;
    }

    if(reg_player(sockfd,(const startup_info *)&s_info) !=0){
        printf("File:%s Line:%d Function %s:  ",__FILE__,__LINE__,__FUNCTION__);
        printf("reg_player Error!!\n");
        close_sdt_stream();
        return -1;
    }
    setvbuf(stdout,NULL,_IONBF,0);
    printf("\nBEGIN Now!!!!!!\n");


    init_game_info(&g_info);

    while(1){

        switch (collect_info(sockfd,&g_info,&s_info)) {
        case RE_COLL_ACTION:
            action = ACTION_CALL;
            bet = 0;
            action_call(&action,&bet,g_info);            
            att_action(sockfd,action,bet);
            break;
        default:            
            close_socket(sockfd);
            close_sdt_stream();
	    del_game_info(&g_info);
            return 0;
        }
    }
}

