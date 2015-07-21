#ifndef GAME_ACTION_H
#define GAME_ACTION_H

#include"game_info.h"
#include"game_socket.h"
#include"game_compare.h"

#define GOOD 100   
#define BAD  50    
#define BBCHENG  1 
#define CHENGPAI 2 
#define TINGPAI  3
#define FEIPAI   4 

typedef struct action_infomation{
    int hold_small;
    int hold_middle;
    int made_hand;
    int ready_hand;
    int is_all_check_before;
}action_info;

void action_call(int * action,int * bet,game_info inform);
#endif
