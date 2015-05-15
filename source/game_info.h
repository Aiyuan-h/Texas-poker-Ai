#ifndef GAME_INFO_H
#define GAME_INFO_H

#define UNUSED          0x00

#define POINT_2_P   2
#define POINT_3_P   3
#define POINT_4_P   5
#define POINT_5_P   7
#define POINT_6_P   11
#define POINT_7_P   13
#define POINT_8_P   17
#define POINT_9_P   19
#define POINT_10_P  23
#define POINT_J_P   29
#define POINT_Q_P   31
#define POINT_K_P   37
#define POINT_A_P   41
#define POINT_MASK_P   0x3f

#define POINT_2_R   0x00
#define POINT_3_R   0x01
#define POINT_4_R   0x02
#define POINT_5_R   0x03
#define POINT_6_R   0x04
#define POINT_7_R   0x05
#define POINT_8_R   0x06
#define POINT_9_R   0x07
#define POINT_10_R  0x08
#define POINT_J_R   0x09
#define POINT_Q_R   0x0a
#define POINT_K_R   0x0b
#define POINT_A_R   0x0c
#define POINT_MASK_R   0x0F

#define COLOR_SPADES        0x10
#define COLOR_HEARTS        0x20
#define COLOR_DIAMONDS      0x40
#define COLOR_CLUBS         0x80
#define COLOR_MASK          0xF0

#define POINT_2_B    0x0001
#define POINT_3_B    0x0002
#define POINT_4_B    0x0004
#define POINT_5_B    0x0008
#define POINT_6_B    0x0010
#define POINT_7_B    0x0020
#define POINT_8_B    0x0040
#define POINT_9_B    0x0080
#define POINT_10_B   0x0100
#define POINT_J_B    0x0200
#define POINT_Q_B    0x0400
#define POINT_K_B    0x0800
#define POINT_A_B    0x1000
#define POINT_MASK_B 0x1ffff


#define STAGE_SEAT      0x01
#define STAGE_BLIND     0x02
#define STAGE_HLOD      0x03
#define STAGE_FLOP      0x04
#define STAGE_TURN      0x05
#define STAGE_RIVER     0x06

#define ROLE_BUTTON     1
#define ROLE_BLIND      2
#define ROLE_SBLIND     3
#define ROLE_BBLIND     4
#define ROLE_PLAYER     5

typedef struct play_node{
    int pid;
    int seating_orde;
    int jetton;
    int money;
    struct play_node *next;
} p_node;


typedef struct collected_inform{
    //轮数记录
    int round;    
    unsigned char stage;
    unsigned char my_role;
    unsigned char my_seating;
    unsigned char total_player;
    unsigned int c_hold[2];
    unsigned int c_flop[3];
    unsigned int c_turn;
    unsigned int c_river;
    p_node *player_list;
}game_info;


int init_game_info(game_info *g_info);
int mask_all_seating_order_with(int seating_order,game_info *g_info);
int del_all_seating_order_with(int seating_order,game_info *g_info);
int updata_player_by_pid(game_info *g_info,p_node player);
int get_seating_order_by_pid(int pid,game_info *g_info);
p_node * get_player_by_pid(int pid,game_info *g_info);
void test_print(game_info *g_info);

#endif // GAME_INFO_H

