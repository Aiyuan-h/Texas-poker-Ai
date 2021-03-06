#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_info.h"
#include "game_socket.h"

int init_game_info(game_info *g_info)
{
    p_node * head;

    head = NULL;
    head = malloc(sizeof(p_node));
    if(head == NULL){
        return -1;
    }

    bzero(head,(sizeof(p_node)));
    head->next = NULL;
    g_info->player_list = head;
    g_info->total_player = 0;
    g_info->round = 0;
    g_info->stage = UNUSED;
    g_info->my_role = UNUSED;

    return 0;
}
int del_game_info(game_info *g_info)
{
	//gege
}
int mask_all_seating_order_with(int seating_order,game_info *g_info)
{
    int i = 0;
    p_node *p;
    p = g_info->player_list;

    while(p->next != NULL){
        i++;
        p->next->seating_orde = seating_order;
        p = p->next;
    }
    return i;
}

int del_all_seating_order_with(int seating_order,game_info *g_info)
{
    int i = 0;
    p_node *p,*q;
    p = g_info->player_list;

    while(p->next != NULL){
        if(p->next->seating_orde == seating_order){
            i++;
            q = p->next;
            p->next = p->next->next;
            free(q);
        }else{
           p = p->next;
        }
    }

    return i;
}

int updata_player_by_pid(game_info *g_info,p_node player)
{
    p_node *p,*q;
    p = g_info->player_list;

    while(p->next != NULL){
        if(p->next->pid == player.pid ){
            player.next = p->next->next;
            memcpy(p->next,&player,sizeof(p_node));
            return 0;
        }else{
           p = p->next;
        }
    }

    q = malloc(sizeof(p_node));
    if(q == NULL){
        return -1;
    }

    memcpy(q,&player,sizeof(p_node));
    q->next = NULL;

    p->next = q;
    return 0;
}

int get_seating_order_by_pid(int pid,game_info *g_info)
{
    p_node *p,*q;
    p = g_info->player_list;

    while(p->next != NULL){
        if(p->next->pid == pid ){
            return p->next->seating_orde;
        }else{
           p = p->next;
        }
    }
    return -1;
}
p_node * get_player_by_pid(int pid,game_info *g_info)
{
    p_node *p,*q;
    p = g_info->player_list;

    while(p->next != NULL){
        if(p->next->pid == pid ){
            return p->next;
        }else{
           p = p->next;
        }
    }

    q = malloc(sizeof(p_node));
    if(q == NULL){
        return NULL;
    }

    bzero(q,(sizeof(p_node)));
    q->next = NULL;
    q->pid = pid;

    p->next = q;
    return p->next;
}

p_node * get_player_by_seating_order_no_add(int seating_order,game_info *g_info)
{
    p_node *p,*q;
    p = g_info->player_list;

    while(p->next != NULL){
        if(p->next->seating_orde == seating_order ){
            return p->next;
        }else{
           p = p->next;
        }
    }

    return NULL;
}

int get_bet_by_seating_order(int seating_order,game_info *g_info)
{
    p_node *p;
    p = g_info->player_list;

    while(p->next != NULL){
        if(p->next->seating_orde == seating_order){
            return p->next->bet;
        }else{
           p = p->next;
        }
    }

    return -1;
}


static int card_byte_to_s(unsigned char *s2,unsigned char * data)
{
    unsigned char d = data[1]&0xF0;

    if(((d&COLOR_SPADES) != 0)&&((d&(~((unsigned char)COLOR_SPADES))) == 0)){
        *s2 = 'S';
    }else if(((d&COLOR_HEARTS) != 0)&&((d&(~((unsigned char)COLOR_HEARTS))) == 0)){
        *s2 = 'H';
    }else if(((d&COLOR_DIAMONDS) != 0)&&((d&(~((unsigned char)COLOR_DIAMONDS))) == 0)){
        *s2 = 'D';
    }else if(((d&COLOR_CLUBS) != 0)&&((d&(~((unsigned char)COLOR_CLUBS))) == 0)){
        *s2 = 'C';
    }else{
        *s2 = 'E';
    }
    d = data[1]&0x0F;
    d += 2;
    if(d == 14){
        d =1;
    }

    s2[1] = d;
    return 0;
}

void test_print(game_info *g_info)
{
    char data[10];
    p_node *p;

    printf("--------------------game-info----------------------\n");
    printf("Round:%d\n",g_info->round);
    printf("Ranking:%d\n",g_info->my_ranking);
    printf("Total player:%d\n",g_info->total_player);
    printf("Seating Order:%d\n",g_info->my_seating);
    printf("Min cost %d\n",g_info->min_cost);
    printf("My bet:%d\n",g_info->my_bet);
    printf("Total Raise and all in stage:%d \n",g_info->total_raise_all_in_in_stage);
    printf("Total flod:%d\n",g_info->total_flod);

    switch(g_info->my_role){
    case ROLE_BUTTON:
        printf("Role: button\n");
        break;
    case ROLE_BLIND:
        printf("Role: blind\n");
        break;
    case ROLE_SBLIND:
        printf("Role: small blind\n");
        break;
    case ROLE_BBLIND:
        printf("Role: big blind\n");
        break;
    case ROLE_PLAYER:
        printf("Role: player\n");
        break;
    default:
        printf("Role: ERROR\n");
        break;
    }
    switch(g_info->stage){
    case STAGE_SEAT:
        printf("Stage: seat\n");
        break;
    case STAGE_BLIND:
        printf("Stage: blind\n");
        break;
    case STAGE_HLOD:
        printf("Stage: hold \nRound in stage:%d\n",\
               g_info->stage_round[g_info->stage]);
        card_byte_to_s(data,(unsigned char *)&g_info->c_hold[0]);
        printf("Card:%c %d ",data[0],data[1]);
        card_byte_to_s(data,(unsigned char *)&g_info->c_hold[1]);
        printf("Card:%c %d\n",data[0],data[1]);
        break;
    case STAGE_FLOP:
        printf("Stage: flop \nRound in stage:%d\n",\
               g_info->stage_round[g_info->stage]);
        card_byte_to_s(data,(unsigned char *)&g_info->c_hold[0]);
        printf("Card:%c %d ",data[0],data[1]);
        card_byte_to_s(data,(unsigned char *)&g_info->c_hold[1]);
        printf("Card:%c %d ",data[0],data[1]);
        card_byte_to_s(data,(unsigned char *)&g_info->c_flop[0]);
        printf("Card:%c %d ",data[0],data[1]);
        card_byte_to_s(data,(unsigned char *)&g_info->c_flop[1]);
        printf("Card:%c %d ",data[0],data[1]);
        card_byte_to_s(data,(unsigned char *)&g_info->c_flop[2]);
        printf("Card:%c %d\n",data[0],data[1]);
        break;
    case STAGE_TURN:
        printf("Stage: turn \nRound in stage:%d\n",\
               g_info->stage_round[g_info->stage]);
        card_byte_to_s(data,(unsigned char *)&g_info->c_hold[0]);
        printf("Card:%c %d ",data[0],data[1]);
        card_byte_to_s(data,(unsigned char *)&g_info->c_hold[1]);
        printf("Card:%c %d ",data[0],data[1]);
        card_byte_to_s(data,(unsigned char *)&g_info->c_flop[0]);
        printf("Card:%c %d ",data[0],data[1]);
        card_byte_to_s(data,(unsigned char *)&g_info->c_flop[1]);
        printf("Card:%c %d ",data[0],data[1]);
        card_byte_to_s(data,(unsigned char *)&g_info->c_flop[2]);
        printf("Card:%c %d ",data[0],data[1]);
        card_byte_to_s(data,(unsigned char *)&g_info->c_turn);
        printf("Card:%c %d\n",data[0],data[1]);
        break;
    case STAGE_RIVER:
        printf("Stage: river \nRound in stage:%d\n",\
               g_info->stage_round[g_info->stage]);
        card_byte_to_s(data,(unsigned char *)&g_info->c_hold[0]);
        printf("Card:%c %d ",data[0],data[1]);
        card_byte_to_s(data,(unsigned char *)&g_info->c_hold[1]);
        printf("Card:%c %d ",data[0],data[1]);
        card_byte_to_s(data,(unsigned char *)&g_info->c_flop[0]);
        printf("Card:%c %d ",data[0],data[1]);
        card_byte_to_s(data,(unsigned char *)&g_info->c_flop[1]);
        printf("Card:%c %d ",data[0],data[1]);
        card_byte_to_s(data,(unsigned char *)&g_info->c_flop[2]);
        printf("Card:%c %d ",data[0],data[1]);
        card_byte_to_s(data,(unsigned char *)&g_info->c_turn);
        printf("Card:%c %d ",data[0],data[1]);
        card_byte_to_s(data,(unsigned char *)&g_info->c_river);
        printf("Card:%c %d\n",data[0],data[1]);
        break;
    default:
        printf("Stage: ERROR\n");
        break;
    }

    printf("My Call in this stage %d\n",g_info->my_total_call_in_stage);
    p = g_info->player_list;
    while(p->next != NULL){
        printf("Player id:%d Seating_order:%d Jetton:%d Money:%d Bet:%d ",\
               p->next->pid,p->next->seating_orde,p->next->jetton,p->next->money,\
               p->next->bet);
        switch (p->next->stat) {
        case STAT_ALL_IN:
            printf("All_in\n");
            break;
        case STAT_BLIND:
            printf("Blind\n");
            break;
        case STAT_CALL:
            printf("Call\n");
            break;
        case STAT_CHECK:
            printf("Check\n");
            break;
        case STAT_FOLD:
            printf("Fold\n");
            break;
        case STAT_RAISE:
            printf("Raise\n");
            break;
        default:
            printf("Error\n");
            break;
        }
        p = p->next;
    }
    printf("Total pot:%d\n",g_info->total_pot);
    printf("--------------------------------------------------\n");
}
