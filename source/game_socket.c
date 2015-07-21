#include <unistd.h>
#include <limits.h>
#include "game_socket.h"
#include "game_info.h"

int open_sdt_stream(char *log_name)
{
    time_t   now;
    struct   tm     *timenow;
    char * dir;


    dir = NULL;
    dir = malloc(sizeof(char)*PATH_MAX);

    if(dir == NULL){
        return -1;
    }

    if(realpath("./",dir) == 0){
        return -1;
    }

    if((PATH_MAX - strlen(dir)) > 2){
        strcat(dir,"/");
    }else{
        return -1;
    }

    if(log_name != NULL){
        if((PATH_MAX - strlen(dir) - strlen(log_name) - 1) > 0){
            strcat(dir,log_name);
        }
    }

    if((PATH_MAX - strlen(dir) - strlen("_reply.log") - 1) > 0){
        strcat(dir,"_reply.log");
    }else{
        return -1;
    }


    fclose(stdout);
    fclose(stderr);
    stdout = freopen(dir,"a+",stdout);
    stderr = freopen(dir,"a+",stderr);
    fclose(stdin);
    free(dir);


    time(&now);
    timenow = localtime(&now);
    printf("\n------------------------------------------------------------\nPLAYER_LOG %s\n",asctime(timenow));

    return 0;

}

int close_sdt_stream(void)
{
    time_t   now;
    struct   tm     *timenow;


    time(&now);
    timenow = localtime(&now);
    printf("\nPLAYER_LOG End %s------------------------------------------------------------\n",asctime(timenow));
    fclose(stdout);
    fclose(stdout);
    return 0;
}

int extr_check_info(int n,char *para[],startup_info * info)
{
        int port;

        if(n < 6){
                return -1;
        }

        bzero(info,sizeof(startup_info));

        info->service_addr.sin_family = AF_INET;

        if(inet_pton(AF_INET,para[1],&info->service_addr.sin_addr) != 1){
                return -1;
        }

        port = atoi(para[2]);
        if((port<1)||(port>65535)){
                return -1;
        }
        info->service_addr.sin_port = htons(port);

        info->client_addr.sin_family = AF_INET;

        if(inet_pton(AF_INET,para[3],&info->client_addr.sin_addr) != 1){
                return -1;
        }

        port = atoi(para[4]);
        if((port < 1)||(port > 65535)){
                return -1;
        }
        info->client_addr.sin_port = htons(port);

        info->pid_i = atoi(para[5]);
        if((info->pid_i == 0)||(strlen(para[5]) >= 20)){
                return -1;
        }
        strcpy(info->pid,para[5]);

        if((n >= 7)&&(strlen(para[6]) < 20)){
                strcpy(info->pname,para[6]);
        }else{
                strcpy(info->pname,"turnip_and_egg");
        }

        return 0;
}

int open_connect_socket(int *sockfd,startup_info * info)
{
    int i = 0;

    *sockfd = socket(AF_INET,SOCK_STREAM,0);

    if(*sockfd < 0){
        return -1;
    }

    i = 1;
    if(setsockopt(*sockfd,SOL_SOCKET,SO_REUSEADDR,\
                  (const void *)(&i),sizeof(int)) < 0){
        return -1;
    }

    if(bind(*sockfd,(struct sockaddr *)&(info->client_addr),\
            sizeof(struct sockaddr_in)) < 0){
        return -1;
    }

    i = 0;
    while(connect(*sockfd,(struct sockaddr *)&(info->service_addr),\
                  sizeof(struct sockaddr_in )) < 0){
        i++;
        sleep(1);                
        if(i > 35){
            return -1;
        }
    }

    return 0;

}

int close_socket(int sockfd)
{
    printf("Close socket\n");
    close(sockfd);
}

int reg_player(int sockfd,const startup_info *info)
{
    char data[60];

    bzero(data,sizeof(data));
    strcpy(data,"reg: ");
    strcat(data,info->pid);
    strcat(data," ");
    strcat(data,info->pname);
    strcat(data," need_notify \n");

    if(write(sockfd,data,strlen(data)) < strlen(data)){
        return -1;
    }else{
        return 0;
    }

}

static int read_until_xx(int sockfd,char *d,int n,const char *xx,int l)
{
    int rev_cnt = 0;

    do{
        if(read(sockfd,(d+rev_cnt),1) == 0){
            return 0;
        }

        if((*(d+rev_cnt) == *(xx+l-1))&&(rev_cnt+1 >=l)){
          if(strncmp((const char*)(d+rev_cnt-l+1),(const char *)(xx),l) == 0){
              rev_cnt++;
              *(d+rev_cnt) = '\0';
              return rev_cnt;
          }
        }
        rev_cnt ++;
    }while(rev_cnt < n);
    *(d+rev_cnt) = '\0';
    return rev_cnt;

}

int read_packet(int sockfd,char *data,int n)
{
    int rev_cnt;
    int rev_ct;

    while(1){

        rev_cnt = 0;
        rev_cnt = read_until_xx(sockfd,data,n,"\n",1);

        if(rev_cnt <= 0){
            return COLL_COLSED;
        }

        if(strncmp((const char *)data,(const char *)"inquire/",8) == 0){
            rev_ct = 0;
            rev_ct = read_until_xx(sockfd,(data+rev_cnt),(n-rev_cnt),"/inquire",8);
            if(rev_cnt <= 2){
                return COLL_ERROR;
            }else{
                return COLL_INQUIRE;
            }

        }else if(strncmp((const char *)data,(const char *)"seat/",5) == 0){
            rev_ct = 0;
            rev_ct = read_until_xx(sockfd,(data+rev_cnt),(n-rev_cnt),"/seat",5);
            if(rev_cnt <= 2){
                return COLL_ERROR;
            }else{
                return COLL_SEAT;
            }
        }else if(strncmp((const char *)data,(const char *)"blind/",6) == 0){
            rev_ct = 0;
            rev_ct = read_until_xx(sockfd,(data+rev_cnt),(n-rev_cnt),"/blind",6);

            if(rev_cnt <= 2){
                return COLL_ERROR;
            }else{
                return COLL_BLIND;
            }
        }else if(strncmp((const char *)data,(const char *)"hold/",5) == 0){
            rev_ct = 0;
            rev_ct = read_until_xx(sockfd,(data+rev_cnt),(n-rev_cnt),"/hold",5);

            if(rev_cnt <= 2){
                return COLL_ERROR;
            }else{
                return COLL_HOLD;
            }
        }else if(strncmp((const char *)data,(const char *)"flop/",5) == 0){
            rev_ct = 0;
            rev_ct = read_until_xx(sockfd,(data+rev_cnt),(n-rev_cnt),"/flop",5);

            if(rev_cnt <= 2){
                return COLL_ERROR;
            }else{
                return COLL_FLOP;
            }
        }else if(strncmp((const char *)data,(const char *)"turn/",5) == 0){
            rev_ct = 0;
            rev_ct = read_until_xx(sockfd,(data+rev_cnt),(n-rev_cnt),"/turn",5);

            if(rev_cnt <= 2){
                return COLL_ERROR;
            }else{
                return COLL_TURN;
            }
        }else if(strncmp((const char *)data,(const char *)"river/",6) == 0){
            rev_ct = 0;
            rev_ct = read_until_xx(sockfd,(data+rev_cnt),(n-rev_cnt),"/river",6);

            if(rev_cnt <= 2){
                return COLL_ERROR;
            }else{
                return COLL_RIVER;
            }
        }else if(strncmp((const char *)data,(const char *)"showdown/",9) == 0){
            rev_ct = 0;
            rev_ct = read_until_xx(sockfd,(data+rev_cnt),(n-rev_cnt),"/showdown",9);

            if(rev_cnt <= 2){
                return COLL_ERROR;
            }else{
                return COLL_SHOWDOWN;
            }
        }else if(strncmp((const char *)data,(const char *)"pot-win/",8) == 0){
            rev_ct = 0;
            rev_ct = read_until_xx(sockfd,(data+rev_cnt),(n-rev_cnt),"/pot-win",8);

            if(rev_cnt <= 2){
                return COLL_ERROR;
            }else{
                return COLL_POT_WIN;
            }
        }else if(strncmp((const char *)data,(const char *)"notify/",7) == 0){
            rev_ct = 0;
            rev_ct = read_until_xx(sockfd,(data+rev_cnt),(n-rev_cnt),"/notify",7);

            if(rev_cnt <= 2){
                return COLL_ERROR;
            }else{
                return COLL_NOTIFY;
            }
        }else if(strncmp((const char *)data,(const char *)"game-over",9) == 0){

            return COLL_GAME_OVER;
        }else{

        }
    }
}

const static int color_str_len[] = {-1,6,6,8,5};

const static unsigned char collor_index[] = {0xff,0x10,0x20,0x40,0x80};

const static unsigned char point_p_index[] = \
{0xff,POINT_10_P,POINT_2_P,POINT_3_P,\
POINT_4_P,POINT_5_P,POINT_6_P,POINT_7_P,\
POINT_8_P,POINT_9_P,POINT_A_P,POINT_J_P,\
POINT_Q_P,POINT_K_P};

const static unsigned char point_r_index[] = \
{0xff,POINT_10_R,POINT_2_R,POINT_3_R,\
POINT_4_R,POINT_5_R,POINT_6_R,POINT_7_R,\
POINT_8_R,POINT_9_R,POINT_A_R,POINT_J_R,\
POINT_Q_R,POINT_K_R};

const static unsigned short point_b_index[] = \
{0xffff,POINT_10_B,POINT_2_B,POINT_3_B,\
POINT_4_B,POINT_5_B,POINT_6_B,POINT_7_B,\
POINT_8_B,POINT_9_B,POINT_A_B,POINT_J_B,\
POINT_Q_B,POINT_K_B};

static int card_str_to_byte(unsigned char *info,const unsigned char *data)
{
    int color = 0,point = 0;

    if((info == NULL)||(data == NULL)){       
        return COMM_ERROR;
    }

    color = 0;
    if(strncmp(data,"SPADES",6) == 0){
        color = 1;
    }else if (strncmp(data,"HEARTS",6) == 0){
        color = 2;
    }else if (strncmp(data,"DIAMONDS",8) == 0){
        color = 3;
    }else if (strncmp(data,"CLUBS",5) == 0){
        color = 4;
    }else{
        return COMM_ERROR;
    }

    point = 0;
    if(('0' < *(data + color_str_len[color] +1))&&\
       (*(data + color_str_len[color] +1) <= '9')){
        point = *(data + color_str_len[color] +1) - '0';
    }else if(*(data + color_str_len[color] +1) == 'A'){
        point = 10;
    }else if(*(data + color_str_len[color] +1) == 'J'){
        point = 11;
    }else if(*(data + color_str_len[color] +1) == 'Q'){
        point = 12;
    }else if(*(data + color_str_len[color] +1) == 'K'){
        point = 13;
    }else{
        return COMM_ERROR;
    }

    (*info) = 0x00;
    (*info) = point_p_index[point];

    info ++;
    (*info) = 0x00;
    (*info) = collor_index[color]|point_r_index[point];

    info ++;
    (*info) = 0x00;
    (*info) = (unsigned char)point_b_index[point]&0xff;

    info ++;
    (*info) = 0x00;
    (*info) = (unsigned char)((point_b_index[point]&0xff00)>>8);
    return COMM_OK;

}

static int handle_inquire(unsigned char *data,game_info *g_info)
{
    unsigned char * tmp;
    int pid,f_seating;

    p_node *player_p = NULL;

    if(strncmp((const char *)data,(const char *)"inquire/",8) != 0){
        return COMM_ERROR;
    }
    g_info->stage_round[g_info->stage]++;
    if(g_info->stage_round[g_info->stage] == 1){
        g_info->my_total_call_in_stage = 0;
    }else{
        if(g_info->my_stat == STAT_CALL){
            g_info->my_total_call_in_stage++;
        }
    }

    if((data = strstr(data,"\n")) == NULL){
         return COMM_ERROR;
    }
    data += 1;

    while (1) {
        pid = atoi(data);
        player_p = get_player_by_pid(pid,g_info);

        tmp = data;
        if((data = strstr(data," ")) == NULL){
            data = tmp;
            goto next_line;
        }
        data += 1;
        player_p->jetton = atoi(data);

        tmp = data;
        if((data = strstr(data," ")) == NULL){
            data = tmp;
            goto next_line;
        }
        data += 1;
        player_p->money = atoi(data);

        tmp = data;
        if((data = strstr(data," ")) == NULL){
            data = tmp;
            goto next_line;
        }
        data += 1;
        player_p->bet = atoi(data);

        tmp = data;
        if((data = strstr(data," ")) == NULL){
            data = tmp;
            goto next_line;
        }
        data += 1;

        if((data[0] == 'c')&&(data[1] == 'a')){
            //call
            player_p->stat = STAT_CALL;
        }else if((data[0] == 'c')&&(data[1] == 'h')){
            //check
            player_p->stat = STAT_CHECK;
        }else if(data[0] == 'r'){
            //raise
            player_p->stat = STAT_RAISE;

        }else if(data[0] == 'a'){
            //all_in
            player_p->stat = STAT_ALL_IN;
        }else if(data[0] == 'b'){
            //blind
            player_p->stat = STAT_BLIND;
        }else if(data[0] == 'f'){
            //fold
            player_p->stat = STAT_FOLD;
        }else{
            player_p->stat = UNUSED;
        }

next_line:
        if((data = strstr(data,"\n")) == NULL){
            data = tmp;
            return COMM_ERROR;
        }
        data += 1;
        if(strncmp(data,"total",5) == 0){
            tmp = data;
            if((data = strstr(data,": ")) != NULL){
                data += 2;
                g_info->total_pot = atoi(data);
            }
            break;
        }

    }

    g_info->my_bet = get_bet_by_seating_order(g_info->my_seating,g_info);
    g_info->my_jetton = get_player_by_seating_order_no_add(\
                g_info->my_seating,g_info)->jetton;
    g_info->my_money = get_player_by_seating_order_no_add(\
                g_info->my_seating,g_info)->money;

    pid = g_info->my_seating;
    g_info->min_cost = 0;
    do{
        pid--;
        if(pid < 1){
            pid = g_info->total_player;
        }
        if(pid == g_info->my_seating){
            break;
        }
        player_p  = get_player_by_seating_order_no_add(pid,g_info);
        if((player_p ==NULL)||(player_p->stat == UNUSED)){
            break;
        }
        if(player_p->stat != STAT_FOLD){
            g_info->min_cost = player_p->bet - g_info->my_bet;
            break;
        }
    }while(1);

    g_info->min_cost = (g_info->min_cost > g_info->my_jetton)?\
                g_info->my_jetton:g_info->min_cost;

    pid = g_info->my_seating;
    g_info->total_flod = 0;
    do{
        pid++;
        if(pid > g_info->total_player){
            pid = 1;
        }

        if(pid == g_info->my_seating){
            break;
        }

        player_p  = get_player_by_seating_order_no_add(pid,g_info);

        if(player_p->stat == STAT_FOLD){
            g_info->total_flod++;
        }
    }while(1);

    g_info->total_raise_all_in_in_stage = 0;
    do{
        pid--;
        if(pid < 1){
            pid = g_info->total_player;
        }

        if(pid == g_info->my_seating){
            break;
        }

        player_p  = get_player_by_seating_order_no_add(pid,g_info);

        if((player_p->stat == STAT_RAISE)||(player_p->stat == STAT_ALL_IN)){
            g_info->total_raise_all_in_in_stage++;
        }

    }while(1);

    g_info->my_stat =  get_player_by_seating_order_no_add(g_info->my_seating,g_info)->stat;
    return COMM_OK;
}

static int handle_seat(char *data,game_info *g_info,startup_info * s_info)
{
    int i = 0,pid;
    static int s_round = 0;
    unsigned char  * tmp = NULL;
    p_node *player_p = NULL;

    if(strncmp((const char *)data,(const char *)"seat/",5) != 0){
        return COMM_ERROR;
    }

    s_round ++;
    g_info->round = s_round;
    g_info->stage_round[STAGE_HLOD] = 0;
    g_info->stage_round[STAGE_FLOP] = 0;
    g_info->stage_round[STAGE_TURN] = 0;
    g_info->stage_round[STAGE_RIVER] = 0;
    g_info->my_stat = UNUSED;
    g_info->my_bet = 0;
    g_info->my_ranking = 0;

    mask_all_seating_order_with(0,g_info);

    for(i = 1;i < 9;i++){
        if((data = strstr(data,"\n")) == NULL){
            return COMM_ERROR;
        }
        data += 1;

        if(strncmp(data,"/seat",5) == 0){
            break;
        }

        tmp = strstr(data,":");
        if(tmp == NULL){
            tmp = data;
        }else{
            tmp += 2;
        }

        pid = atoi(tmp);
        player_p = get_player_by_pid(pid,g_info);
        player_p->pid = pid;
        player_p->seating_orde = i;

        data = tmp;
        if((data = strstr(data," ")) == NULL){
            data = tmp;
            continue;
        }
        data += 1;
        player_p->jetton = atoi(data);

        tmp = data;
        if((data = strstr(data," ")) == NULL){
            data = tmp;
            continue;
        }
        data += 1;     
        player_p->money = atoi(data);
        player_p->stat = UNUSED;
        player_p->bet = 0;

    }

    del_all_seating_order_with(0,g_info);

    g_info->total_player = i-1;

    i = 0;
    i =  get_seating_order_by_pid(s_info->pid_i,g_info);
    if(i<=0){
        return COMM_ERROR;
    }else if(i >3){
        g_info->my_role = ROLE_PLAYER;
        g_info->my_stat = UNUSED;
    }else if(i == 1){
        g_info->my_role = ROLE_BUTTON;
        g_info->my_stat = STAT_BUTTON;
    }else if((g_info->total_player <3)){
        g_info->my_role = ROLE_BLIND;
        g_info->my_stat = STAT_BLIND;
    }else if(i == 2){
        g_info->my_role = ROLE_SBLIND;
        g_info->my_stat = STAT_BLIND;
    }else{
        g_info->my_role = ROLE_BBLIND;
        g_info->my_stat = STAT_BLIND;
    }
    g_info->my_seating = i;

    g_info->stage = STAGE_SEAT;

    g_info->my_jetton = get_player_by_seating_order_no_add(\
                g_info->my_seating,g_info)->jetton;
    g_info->my_money = get_player_by_seating_order_no_add(\
                g_info->my_seating,g_info)->money;

    g_info->my_ranking = 1;
    for(i = 1;i <= g_info->total_player;i++){
        if((i != g_info->my_seating)&&\
                ((get_player_by_seating_order_no_add(i,g_info)->jetton+\
                 get_player_by_seating_order_no_add(i,g_info)->money) >\
                 (g_info->my_jetton+g_info->my_money))){
            g_info->my_ranking++;
        }
    }

    return COMM_OK;
}
static int handle_bilnd(char *data,game_info *g_info)
{
    return COMM_OK;
}

static int handle_hold(unsigned char *data,game_info *g_info)
{
    int i;
    unsigned char * tmp = NULL;

    if(strncmp((const char *)data,(const char *)"hold/",5) != 0){
        return COMM_ERROR;
    }

    bzero(g_info->c_hold,8);

    tmp = data;
    for(i=0;i<2;i++){
        tmp = strstr(tmp,"\n");
        if(tmp == NULL){
            return COMM_ERROR;
        }
        tmp += 1;

        if(card_str_to_byte((unsigned char *)&g_info->c_hold[i],tmp) != COMM_OK){
            return COMM_ERROR;
        }
    }

    g_info->stage = STAGE_HLOD;
    for(i = 1;i <= g_info->total_player;i++){
        get_player_by_seating_order_no_add(i,g_info)->stat = UNUSED;
    }

    return COMM_OK;
}

static int handle_flop(char *data,game_info *g_info)
{
    int i;
    unsigned char * tmp = NULL;

    if(strncmp((const char *)data,(const char *)"flop/",5) != 0){
        return COMM_ERROR;
    }

    bzero(g_info->c_flop,12);

    tmp = data;
    for(i=0;i<3;i++){
        tmp = strstr(tmp,"\n");
        if(tmp == NULL){
            return COMM_ERROR;
        }
        tmp += 1;

        if(card_str_to_byte((char *)&g_info->c_flop[i],tmp) != COMM_OK){
            return COMM_ERROR;
        }
    }

    g_info->stage = STAGE_FLOP;

    for(i = 1;i <= g_info->total_player;i++){
        get_player_by_seating_order_no_add(i,g_info)->stat = UNUSED;
    }

    return COMM_OK;

}

static int handle_turn(char *data,game_info *g_info)
{
    unsigned char * tmp = NULL;
    int i;

    if(strncmp((const char *)data,(const char *)"turn/",5) != 0){
        return COMM_ERROR;
    }

    bzero(&g_info->c_turn,4);

    tmp = strstr(data,"\n");
    if(tmp == NULL){
        return COMM_ERROR;
    }
    tmp += 1;

    if(card_str_to_byte((char *)&g_info->c_turn,tmp) != COMM_OK){
        return COMM_ERROR;
    }

    g_info->stage = STAGE_TURN;
    for(i = 1;i <= g_info->total_player;i++){
        get_player_by_seating_order_no_add(i,g_info)->stat = UNUSED;
    }
    return COMM_OK;
}

static int handle_river(char *data,game_info *g_info)
{
    unsigned char * tmp = NULL;
    int i;

    if(strncmp((const char *)data,(const char *)"river/",6) != 0){
        return COMM_ERROR;
    }

    bzero(&g_info->c_river,4);

    tmp = strstr(data,"\n");
    if(tmp == NULL){
        return COMM_ERROR;
    }
    tmp += 1;

    if(card_str_to_byte((char *)&g_info->c_river,tmp) != COMM_OK){
        return COMM_ERROR;
    }

    g_info->stage = STAGE_RIVER ;
    for(i = 1;i <= g_info->total_player;i++){
        get_player_by_seating_order_no_add(i,g_info)->stat = UNUSED;
    }

    return COMM_OK;
}

static int handle_showdown(char *data,game_info *g_info)
{
    return COMM_OK;
}

static int handle_pot_win(char *data,game_info *g_info)
{
    return COMM_OK;
}

static int handle_notify(char *data,game_info *g_info)
{
    unsigned char * tmp;
    int pid;

    p_node *player_p = NULL;

    if(strncmp((const char *)data,(const char *)"notify/",7) != 0){
        return COMM_ERROR;
    }

    g_info->stage_round[g_info->stage]++;
    if(g_info->stage_round[g_info->stage] == 1){
        g_info->my_total_call_in_stage = 0;
    }else{
        if(g_info->my_stat == STAT_CALL){
            g_info->my_total_call_in_stage++;
        }
    }

    if((data = strstr(data,"\n")) == NULL){
         return COMM_ERROR;
    }
    data += 1;

    while (1) {
        pid = atoi(data);
        player_p = get_player_by_pid(pid,g_info);

        tmp = data;
        if((data = strstr(data," ")) == NULL){
            data = tmp;
            goto next_line;
        }
        data += 1;
        player_p->jetton = atoi(data);

        tmp = data;
        if((data = strstr(data," ")) == NULL){
            data = tmp;
            goto next_line;
        }
        data += 1;
        player_p->money = atoi(data);

        tmp = data;
        if((data = strstr(data," ")) == NULL){
            data = tmp;
            goto next_line;
        }
        data += 1;
        player_p->bet = atoi(data);

        tmp = data;
        if((data = strstr(data," ")) == NULL){
            data = tmp;
            goto next_line;
        }
        data += 1;

        if((data[0] == 'c')&&(data[1] == 'a')){
            //call
            player_p->stat = STAT_CALL;
        }else if((data[0] == 'c')&&(data[1] == 'h')){
            //check
            player_p->stat = STAT_CHECK;
        }else if(data[0] == 'r'){
            //raise
            player_p->stat = STAT_RAISE;

        }else if(data[0] == 'a'){
            //all_in
            player_p->stat = STAT_ALL_IN;
        }else if(data[0] == 'b'){
            //blind
            player_p->stat = STAT_BLIND;
        }else if(data[0] == 'f'){
            //fold
            player_p->stat = STAT_FOLD;
        }else{
            player_p->stat = UNUSED;
        }

next_line:
        if((data = strstr(data,"\n")) == NULL){
            data = tmp;
            return COMM_ERROR;
        }
        data += 1;
        if(strncmp(data,"total",5) == 0){
            tmp = data;
            if((data = strstr(data,": ")) != NULL){
                data += 2;
                g_info->total_pot = atoi(data);
            }
            break;
        }

    }

    g_info->my_bet = get_bet_by_seating_order(g_info->my_seating,g_info);

    g_info->min_cost = -1;

    pid = g_info->my_seating;
    g_info->total_flod = 0;
    do{
        pid++;
        if(pid > g_info->total_player){
            pid = 1;
        }

        player_p  = get_player_by_seating_order_no_add(pid,g_info);

        if(player_p->stat == STAT_FOLD){
            g_info->total_flod++;
        }

        if(pid == g_info->my_seating){
            break;
        }

    }while(1);

    pid = g_info->my_seating;
    g_info->total_raise_all_in_in_stage = 0;
    do{
        pid--;
        if(pid < 1){
            pid = g_info->total_player;
        }

        player_p  = get_player_by_seating_order_no_add(pid,g_info);

        if((player_p->stat == STAT_RAISE)||(player_p->stat == STAT_ALL_IN)){
            g_info->total_raise_all_in_in_stage++;
        }

        if(pid == g_info->my_seating){
            break;
        }

    }while(1);

    g_info->my_stat =  get_player_by_seating_order_no_add(\
                g_info->my_seating,g_info)->stat;
    return COMM_OK;
}
int collect_info(int sockfd, game_info *g_info,startup_info *s_info)
{
    char data[600];

    while(1){

        switch(read_packet(sockfd,data,600)){
        case COLL_INQUIRE:            
            //printf("Inquire:\n");
            //printf("%s\n",data);
            handle_inquire(data,g_info);
            //test_print(g_info);
            return RE_COLL_ACTION;
        case COLL_SEAT:
            //printf("Seat:\n");
            //printf("%s\n",data);
            handle_seat(data,g_info,s_info);
            //test_print(g_info);
            break;
        case COLL_BLIND:
            //printf("Blind:\n");
            //printf("%s\n",data);
            handle_bilnd(data,g_info);
            //test_print(g_info);
            break;
        case COLL_HOLD:
            //printf("Hold:\n");
            //printf("%s\n",data);
            handle_hold(data,g_info);
            //test_print(g_info);
            break;
        case COLL_FLOP:
            //printf("Flop:\n");
            //printf("%s\n",data);
            handle_flop(data,g_info);
            //test_print(g_info);
            break;
        case COLL_TURN:
            //printf("Turn:\n");
            //printf("%s\n",data);
            handle_turn(data,g_info);
            //test_print(g_info);
            break;
        case COLL_RIVER:
            //printf("River:\n");
            //printf("%s\n",data);         
            handle_river(data,g_info);
            //test_print(g_info);
            break;
        case COLL_SHOWDOWN:
            //printf("SHOW Down:\n%s\n",data);            
            handle_showdown(data,g_info);
            break;
        case COLL_POT_WIN:
            //printf("Pot Win:\n");
            //printf("%s\n",data);
            handle_pot_win(data,g_info);
            break;
        case COLL_NOTIFY:
            //printf("Notify:\n");
            //printf("%s\n",data);
            handle_notify(data,g_info);
            //test_print(g_info);
            break;
        case COLL_COLSED:
        case COLL_GAME_OVER:
            return COLL_COLSED;
        case COLL_ERROR:
            return RE_COLL_ERROR;
        default:
            break;

        }
    }

}

int att_action(int sockfd,int action,int num)
{
    char data[60];

    if((ACTION_CHECK <= action)&&(action <= ACTION_FOLD)){
        switch(action){
        case ACTION_CALL:
            strcpy(data,"call \n");
            break;
        case ACTION_RAISE:
            if(0 > num){
                num = 0;
            }

            sprintf(data,"raise %d \n",num);
            break;
        case ACTION_CHECK:
            strcpy(data,"check \n");
            break;
        case ACTION_ALL_IN:
            strcpy(data,"all_in \n");
            break;
        case ACTION_FOLD:
            strcpy(data,"fold \n");
            break;
        default:
            strcpy(data,"check \n");
            break;
        }
        if(write(sockfd,data,strlen(data)) < strlen(data)){
            return RE_ACTION_ERROR;
        }else{
            return RE_ACTION_OK;
        }
    }else{
        return RE_ACTION_ERROR;
    }

}
