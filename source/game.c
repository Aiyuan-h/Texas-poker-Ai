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
#include "game_info.h"

int main(int argc,char * argv[])
{
    startup_info s_info;
    game_info g_info;

    int sockfd = 0;

//  open_sdt_stream(argv[5]);

    if(extr_check_info(argc,argv,&s_info) != 0){
        printf("File:%s Line:%d Function %s:  ",__FILE__,__LINE__,__FUNCTION__);
        printf("extr_check_info Error!!\n");
//        close_sdt_stream();
        return -1;
    }

    if(open_connect_socket(&sockfd,&s_info) != 0){
        printf("File:%s Line:%d Function %s:  ",__FILE__,__LINE__,__FUNCTION__);
        printf("open_connect_socket Error!!\n");
 //       close_sdt_stream();
        return -1;
    }

    if(reg_player(sockfd,(const startup_info *)&s_info) !=0){
        printf("File:%s Line:%d Function %s:  ",__FILE__,__LINE__,__FUNCTION__);
        printf("reg_player Error!!\n");
 //       close_sdt_stream();
        return -1;
    }
    printf("\nBEGIN\n");

    //初始化结构体
    init_game_info(&g_info);

    while(1){

        switch (collect_info(sockfd,&g_info,&s_info)) {
        case RE_COLL_ACTION:
            att_action(sockfd,ACTION_CALL,0);
            break;
        default:
            close_socket(sockfd);
//            close_sdt_stream();
            return 0;
        }
    }
}
int main1(int argc,char * argv[])
{
    game_info g_info;
    p_node player;
    int i;

    init_game_info(&g_info);
    for(i=0;i<10;i++){
        player.pid = i;
        player.seating_orde = i;
        updata_player_by_pid(&g_info,player);
    }
    test_print(&g_info);
    printf("\n\n\n");

    player.pid = 5;
    player.seating_orde = 50;
    updata_player_by_pid(&g_info,player);
    test_print(&g_info);

    mask_all_seating_order_with(0,&g_info);
    test_print(&g_info);

    for(i=5;i<10;i++){
        player.pid = i;
        player.seating_orde = i-4;
        updata_player_by_pid(&g_info,player);
    }
    test_print(&g_info);

    del_all_seating_order_with(0,&g_info);
    test_print(&g_info);
    return 0;
}
