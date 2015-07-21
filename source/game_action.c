#include <string.h>
#include "game_action.h"

//定义计数变量
static int i;
static float max_win;

//瀹氫箟涓暟缁?
static int powerpoints[13][13]={
                                {11, 7, 7, 8, 6, 6, 6, 7, 8, 8, 9,11,13},
                                { 7,11, 8, 9, 8, 7, 6, 7, 8, 9,10,11,14},
                                { 7, 8,11,10,10, 9, 8, 7, 8, 9,10,11,14},
                                { 8, 9,10,12,11,10, 9, 8, 8, 9,10,12,15},
                                { 6, 8,10,11,13,11,11,10,10, 9,11,12,14},
                                { 6, 7, 9,10,11,13,12,12,11,11,11,13,14},
                                { 6, 6, 8, 9,11,12,15,13,13,13,13,13,14},
                                { 7, 7, 7, 8,10,12,13,16,15,15,15,15,16},
                                { 8, 8, 8, 8,10,11,13,15,19,18,18,18,19},
                                { 8, 9, 9, 9, 9,11,13,15,18,22,19,20,20},
                                { 9,10,10,10,11,11,13,15,18,19,27,21,22},
                                {11,11,11,12,12,13,13,15,18,20,21,30,24},
                                {13,14,14,15,14,14,14,16,19,20,22,24,40}
                               };
     
static int powerpoint[13][13]={
                               {11, 1, 2, 2, 1, 0, 0, 1, 2, 2, 3, 4, 7},
                               { 1,11, 3, 4, 3, 1, 1, 1, 2, 2, 3, 5, 7},
                               { 2, 3,11, 5, 4, 3, 2, 1, 2, 3, 4, 5, 8},
                               { 2, 4, 5,12, 6, 5, 4, 3, 2, 3, 4, 5, 8},
                               { 1, 3, 4, 6,13, 6, 5, 4, 4, 3, 4, 6, 7},
                               { 0, 1, 3, 5, 6,13, 7, 6, 6, 5, 5, 6, 8},
                               { 0, 1, 2, 4, 5, 7,15, 8, 8, 7, 7, 7, 9},
                               { 1, 1, 1, 3, 4, 6, 8,16,10, 9, 9, 9,10},
                               { 2, 2, 2, 2, 4, 6, 8,10,19,13,12,13,13},
                               { 2, 2, 3, 3, 3, 5, 7, 9,13,22,14,14,14},
                               { 3, 3, 4, 4, 4, 5, 7, 9,12,14,27,16,16},
                               { 4, 5, 5, 5, 6, 6, 7, 9,13,14,16,30,19},
                               { 7, 7, 8, 8, 7, 8, 9,10,13,14,16,19,40}
                              };


//static int hold_classify(game_info inform)
int hold_classify(game_info inform)
{

    if((inform.c_hold[0]&(COLOR_MASK<<8)) == \
            (inform.c_hold[1]&(COLOR_MASK<<8))){
        return powerpoints[((inform.c_hold[0]>>8)&(0x0f))]\
                [((inform.c_hold[1]>>8)&(0x0f))];
    }else{
        return powerpoint[((inform.c_hold[0]>>8)&(0x0f))]\
                [((inform.c_hold[1]>>8)&(0x0f))];
    }
}

//底牌圈响应
static void preflop_action(int *action,int *bet,int p,game_info inform,action_info a_info)
{
    if((inform.min_cost == 0)&&(a_info.is_all_check_before == 1)){
        *action = ACTION_RAISE;
        *bet = 1;
        return;
    }

    if(p < a_info.hold_small){
        if(inform.min_cost == 0){
            *action = ACTION_CHECK;
            *bet = 0;
            return;
        }else if((inform.total_raise_all_in_in_stage > 0)||(inform.my_bet==0)){
            *action = ACTION_FOLD;
            *bet = 0;
            return;
        }else{
            *action = ACTION_CALL;
            *bet = 0;
            return ;
        }
    }else if(p < a_info.hold_middle){
//        if((inform.total_pot<=60)&&((inform.my_seating>(inform.total_player/2)||(inform.my_seating==1)))){
//            *action=ACTION_RAISE;
//            *bet=1;
//            return;
//        }else if(inform.min_cost==0){
//            *action = ACTION_CHECK;
//            *bet = 0;
//            return ;
//        }else if((inform.total_raise_all_in_in_stage > 1)||((inform.my_bet==0)&&(inform.total_player>7))){
//            *action = ACTION_FOLD;
//            *bet = 0;
//            return ;
//        }else if(inform.min_cost<80){
//            *action = ACTION_CALL;
//            *bet = 0;
//            return ;
//        }else{
//            *action= ACTION_FOLD;
//            *bet = 0;
//            return;
//        }

        if(inform.my_bet+inform.min_cost>500){
            *action = ACTION_FOLD;
            *bet = 0;
            return ;
        }else if(inform.min_cost==0){
            *action = ACTION_CHECK;
            *bet = 0;
            return ;
//        }else if((inform.my_bet==0)&&((inform.my_seating==4)||(inform.my_seating==5))){
//            *action = ACTION_FOLD;
//            *bet = 0;
//            return ;
        }else if((inform.total_raise_all_in_in_stage<=1)&&((inform.my_seating>(inform.total_player-3))||(inform.my_seating==1))){
            if(inform.my_bet+inform.min_cost<200){
                *action=ACTION_RAISE;
                *bet = 1;
                return;
            }else{
                *action = ACTION_CALL;
                *bet = 0;
                return ;
            }
        }else if(inform.min_cost<80){
            *action = ACTION_CALL;
            *bet = 0;
            return ;
        }else{
            *action= ACTION_FOLD;
            *bet = 0;
            return;
        }
    }else{
        if(inform.my_bet+inform.min_cost>800){
            *action=ACTION_FOLD;
            *bet=0;
            return;
        }else if(inform.my_bet<500){
            *action=ACTION_RAISE;
            *bet=80;
            return;
        }else{
            *action = ACTION_CALL;
            *bet = 0;
            return;
        }
    }
}
//花色比较
static int color_compare(int *col,int n)
{
    int i,t0,t1,t2,t3;

    t0=0;
    t1=0;
    t2=0;
    t3=0;
    for(i=0;i<n;i++){
        if((col[i]&0x00001000) == 0x00001000){
            t0++;
            if(t0 == 4){
                 return 1;
            }
        }else if((col[i]&0x00002000) == 0x00002000){
            t1++;
            if(t1 == 4){
                 return 1;
            }
        }else if((col[i]&0x00004000) == 0x00004000){
            t2++;
            if(t2 == 4){
                 return 1;
            }
        }else if((col[i]&0x00008000) == 0x00008000){
            t3++;
            if(t3 == 4){
                 return 1;
            }
        }

    }
    return 0;

}

static int straight_judge(int *a,int l)
{
    int n;

    if(l==6){
        n=combine(a,6,5);
        if((FIVE_STRAIGHT >= n)&&(n >= ACE_STRAIGHT)){
            return 1;
        }else{
            return 0;
        }
    }else if(l==7){
        n=combine(a,7,5);
        if((FIVE_STRAIGHT >= n)&&(n >= ACE_STRAIGHT))
            return 1;
        else
            return 0;
    }
}

//计算用牌数组
const static int next[]={69634,135427,266757,529159,1053707,2102541,4199953,8394515,\
                   16783383,33560861,67115551,134224677,268442665};

//缺少多少牌成顺子?
int straight_lack(int *a,int n)
{
    int count=0;
    int *p = NULL;

    p = malloc(sizeof(int)*(n+1));
    memcpy(p,a,sizeof(int)*n);

    for(i=0;i<13;i++){
        p[n]=next[i];
        if(straight_judge(p,n+1) == 1){
            count++;
        }
    }

    free(p);
    return count;
}

//翻牌圈牌分类
static int flop_classify(game_info inform,action_info a_info)
{
    int n;

    n = five_hand_compare(inform.c_hold);

    if(n==0){
        return FEIPAI;
    }else if(n < 322){

        return BBCHENG;
    }else if(n < a_info.made_hand){

        return CHENGPAI;
    }else if(n < a_info.ready_hand){

        return TINGPAI;
    }else{

        if(color_compare(inform.c_hold,5) == 1){ //如果有四个花色相同，则听牌
            return TINGPAI;
        }else{
            if(straight_lack(inform.c_hold,5) == 0){ //如果不能补成顺子
                return FEIPAI;
            }else{
                return TINGPAI;
            }
        }
    }
}

//缺少听牌数
static int tingpai_lack(game_info inform)
{
    int n;
    int pair_flag = 0,flush_flag = 0;

    if(inform.stage == STAGE_FLOP){
        n=five_hand_compare(inform.c_hold);

        if((ACE_HIGH >= n)&&(n>=TEN_PAIR)){
            pair_flag = 1;
        }

        if(color_compare(inform.c_hold,5) ==1){
            flush_flag = 1;
        }

        return (2*pair_flag + 9*flush_flag + 4*straight_lack(inform.c_hold,5));
    }else if(inform.stage == STAGE_TURN){

        n=combine(inform.c_hold,6,5);

        if((ACE_HIGH >= n)&&(n>=TEN_PAIR)){
            pair_flag = 1;
        }

        if(color_compare(inform.c_hold,5) ==1){
            flush_flag = 1;
        }

        return (2*pair_flag + 9*flush_flag + 4*straight_lack(inform.c_hold,6));
    }
       return 0;
}

//计算投资回报率
static int invest_return(game_info inform)
{
    int n;
    float hand_odds,roi,pot_odds;

    n = tingpai_lack(inform);

    if(inform.stage==STAGE_FLOP){
        hand_odds=n/47;
    }else{
        hand_odds=n/46;
    }

    pot_odds=(inform.min_cost)/(inform.total_pot);
    roi=hand_odds/pot_odds;

    if(roi > 1.5){
        return 1;
    }else{
        return 0;
    }
}

//翻牌圈响应
static void flop_action(int *action,int *bet,int state,game_info inform,action_info a_info)
{
    if((inform.min_cost == 0)&&(a_info.is_all_check_before == 1)){
        *action = ACTION_RAISE;
        *bet = 1;
        return;
    }

    if (state == FEIPAI){
        if(inform.min_cost == 0){
            *action = ACTION_CHECK;
            *bet = 0;
            return;
        }else{
            *action = ACTION_FOLD;
            *bet = 0;
            return;
        }
    }else if (state == CHENGPAI){                           //5/22 后续添加赢牌概率判断
            if(inform.my_bet+inform.min_cost>=1200){
                *action=ACTION_FOLD;
                *bet=0;
                return;
            }else if(win_rate(inform) > max_win){
                if((inform.total_raise_all_in_in_stage > 1)||\
                        (inform.my_bet > 500)||(inform.min_cost>=200)){
                    *action = ACTION_CALL;
                    *bet = 0;
                    return;
                }else{
                    *action = ACTION_RAISE;
                    *bet = 200;
                    return;
                }
            }else if((inform.my_bet <= 400)&&(inform.min_cost<=200)){
                    *action = ACTION_CALL;
                    *bet = 0;
                    return;
            }else{
                if(inform.min_cost<=40){
                    *action=ACTION_CHECK;
                    *bet=0;
                    return;
                }else{
                    *action = ACTION_FOLD;
                    *bet = 0;
                    return;
                }
            }

    }else if (state == TINGPAI){
        if(invest_return(inform) == 1){
            if(inform.min_cost ==0 ){
                *action = ACTION_CHECK;
                *bet = 0;
                return;
            }else{
                if((inform.my_bet<250)&&(inform.min_cost<200)){                     //5.22 新增听牌跟牌筹码判断
                    *action = ACTION_CALL;
                    *bet = 0;
                    return;
                }else{
                    *action = ACTION_FOLD;
                    *bet = 0;
                    return;
                }
            }
        }else{
            if(inform.min_cost==0){
                *action = ACTION_CHECK;
                *bet = 0;
                return;
            }else{
                *action = ACTION_FOLD;
                *bet = 0;
                return;
            }
        }
    }else if(state == BBCHENG){
        if((win_rate(inform)>max_win)&& (inform.my_bet+inform.min_cost)<2500){
            *action=ACTION_RAISE;
            *bet=1000;
            return;
        }else{
            *action = ACTION_CALL;
            *bet = 0;
            return ;
        }
    }else{
        *action = ACTION_CHECK;
        *bet = 0;
        return ;
    }
}

//转牌圈牌分类
static int turn_classify(game_info info,action_info a_info)
{
    int n;

    n=combine(info.c_hold,6,5);

    if(n==0){
        return FEIPAI;
    }else if(n < 322){

        return BBCHENG;
    }else if(n < a_info.made_hand){

        return CHENGPAI;
    }else if(n < a_info.ready_hand){

        return TINGPAI;
    }else{
        if(color_compare(info.c_hold,6)==1){   //如果有四个花色相同，则听牌
            return TINGPAI;
        }else{
            if(straight_lack(info.c_hold,6)==0){ //如果不能补成顺子
                return FEIPAI;
            }else{
                return TINGPAI;
            }
        }
    }
}

//转牌圈响应
static void turn_action(int *action,int * bet,int state,game_info inform,action_info a_info)
{
    if((inform.min_cost == 0)&&(a_info.is_all_check_before == 1)){
        *action = ACTION_RAISE;
        *bet = 1;
        return;
    }

    if (state == FEIPAI){
        if(inform.min_cost==0){
            *action = ACTION_CHECK;
            *bet = 0;
            return;
        }else{
            *action = ACTION_FOLD;
            *bet = 0;
            return;
        }
    }else if (state == CHENGPAI){                                      //需要新增赢牌概率
        if(inform.my_bet+inform.min_cost>=1200){
            *action=ACTION_FOLD;
            *bet=0;
            return;
        }else if(win_rate(inform)>max_win){
            if((inform.total_raise_all_in_in_stage>1)||\
                    (inform.my_bet >= 500)||(inform.min_cost>=200)){
                *action = ACTION_CALL;
                *bet = 0;
                return;
            }else{
                *action = ACTION_RAISE;
                *bet = 200;
                return;
            }
        }else if((inform.my_bet < 500)&&(inform.min_cost<=200)){
                *action = ACTION_CALL;
                *bet = 0;
                return;
        }else{
            if(inform.min_cost==0){
                *action=ACTION_CHECK;
                *bet=0;
                return;
            }else{
                *action = ACTION_FOLD;
                *bet = 0;
                return;
            }
        }
    }else if (state == TINGPAI){
        if(invest_return(inform) == 1){
            if(inform.min_cost==0){
                *action = ACTION_CHECK;
                *bet = 0;
                return;
            }else{
                if((inform.min_cost < 150)&&(inform.my_bet<200)){
                    *action = ACTION_CALL;
                    *bet = 0;
                    return;
                }else{
                    *action = ACTION_FOLD;
                    *bet = 0;
                    return;
                }
            }
        }else{
            if(inform.min_cost==0){
                *action = ACTION_CHECK;
                *bet = 0;
                return;
            }else{
                *action = ACTION_FOLD;
                *bet = 0;
                return;
            }
        }
    }else if(state == BBCHENG){
        if((win_rate(inform)>max_win) && (inform.my_bet+inform.min_cost)<2500){
            *action=ACTION_RAISE;
            *bet=1000;
            return;
        }else{
            *action = ACTION_CALL;
            *bet = 0;
            return;
        }
    }
}

//河牌圈牌分类
static int river_classify(game_info inform,action_info a_info)
{
    int n;

    n=combine(inform.c_hold,7,5);

    if(n==0){
        return FEIPAI;
    }else if(n <= 322){
        return BBCHENG;
    }else if(n < a_info.made_hand){
        return CHENGPAI;
    }else{
        return FEIPAI;
    }
}

//河牌圈响应
static int river_action(int * action,int *bet,int state,game_info inform)
{
    if(state == BBCHENG){
        if((win_rate(inform)>max_win) && (inform.my_bet+inform.min_cost)<2500){
            *action=ACTION_RAISE;
            *bet=1000;
            return;
        }else{
            *action = ACTION_CALL;
            *bet = 0;
            return;
        }
    }else if(state == FEIPAI){
        if(inform.min_cost == 0){
            *action = ACTION_CHECK;
            *bet = 0;
            return;
        }else{
            *action = ACTION_FOLD;
            *bet = 0;
            return;
        }
    }else if (state==CHENGPAI){
        if(inform.my_bet+inform.min_cost>=1200){
            *action=ACTION_FOLD;
            *bet=0;
            return;
        }else if(win_rate(inform)==1){
            *action=ACTION_ALL_IN;
            *bet=0;
            return;
        }else if(win_rate(inform)>max_win){
            if((inform.total_raise_all_in_in_stage>1)||\
                    (inform.min_cost > 100||(inform.my_bet>500))){
                *action = ACTION_CALL;
                *bet = 0;
                return;
            }else{
                *action = ACTION_RAISE;
                *bet = 200;
                return;
            }
        }else if((inform.min_cost <200)&&\
                 (inform.my_bet < 500)){
            *action = ACTION_CALL;
            *bet = 0;
            return;
        }else{
            if(inform.min_cost==0){
                *action=ACTION_CHECK;
                *bet=0;
                return;
            }else{
                *action = ACTION_FOLD;
                *bet = 0;
                return;
            }
        }
    }
}


//总调用函数
void action_call_x(int *action,int *bet,game_info inform,action_info a_info)
{
    if(inform.stage == STAGE_HLOD){
        preflop_action(action,bet,hold_classify(inform),inform,a_info);
        return;
    }else if(inform.stage == STAGE_FLOP){

        flop_action(action,bet,flop_classify(inform,a_info),inform,a_info);
        return;
    }else if(inform.stage==STAGE_TURN){

        turn_action(action,bet,turn_classify(inform,a_info),inform,a_info);
        return ;
    }else if(inform.stage==STAGE_RIVER){

        river_action(action,bet,river_classify(inform,a_info),inform);
        return ;
    }
}

void action_call(int *action, int *bet, game_info inform)
{
    static action_info a_info;
    a_info.hold_small = 9;
    a_info.hold_middle = 19;
    a_info.made_hand = 2896;
    a_info.ready_hand = 6186;

    if((inform.stage == STAGE_HLOD)&&(inform.stage_round[STAGE_HLOD] == 1)){
        a_info.is_all_check_before = 0;
    }

//    if(inform.total_player< 6){
//        a_info.hold_small = 6;
//        a_info.hold_middle = 15;
//        a_info.made_hand = 3546;
//        a_info.ready_hand = 6186;

//        action_call_x(action,bet,inform,a_info);
//    }else
    if((inform.round > 400)&&(inform.my_ranking>3)){

        a_info.hold_small = 6;
        a_info.hold_middle = 11;
        a_info.made_hand = 4206;
        a_info.ready_hand = 6186;

        if(inform.total_player-inform.total_flod==8){
            max_win==0.30;
        }else if(inform.total_player-inform.total_flod==7){
            max_win=0.35;
        }else if(inform.total_player-inform.total_flod==6){
            max_win=0.40;
        }else if(inform.total_player-inform.total_flod==5){
            max_win=0.5;
        }else if(inform.total_player-inform.total_flod==4){
            max_win=0.6;
        }else if(inform.total_player-inform.total_flod==3){
            max_win=0.7;
        }else{
            max_win=0.8;
        }

        action_call_x(action,bet,inform,a_info);
    }else{
        a_info.hold_small = 11;
        a_info.hold_middle = 19;
        a_info.made_hand = 4206;
        a_info.ready_hand = 6186;

        if(inform.total_player-inform.total_flod==8){
            max_win==0.35;
        }else if(inform.total_player-inform.total_flod==7){
            max_win=0.4;
        }else if(inform.total_player-inform.total_flod==6){
            max_win=0.45;
        }else if(inform.total_player-inform.total_flod==5){
            max_win=0.55;
        }else if(inform.total_player-inform.total_flod==4){
            max_win=0.65;
        }else if(inform.total_player-inform.total_flod==3){
            max_win=0.75;
        }else{
            max_win=0.85;
        }

        action_call_x(action,bet,inform,a_info);
    }

    if(*action == ACTION_CHECK){
        a_info.is_all_check_before = 1;
    }else{
        a_info.is_all_check_before = 0;
    }
}
