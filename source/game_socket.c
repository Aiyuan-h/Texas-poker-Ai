#include <unistd.h>
#include "game_socket.h"
#include "game_info.h"

int open_sdt_stream(char *log_name)
{
    time_t   now;
    struct   tm     *timenow;
    char * dir;

    //获取工作路径
    dir = NULL;
    dir = malloc(sizeof(char)*300);
    if(dir == NULL){
        return -1;
    }
    if(realpath("./",dir) == 0){
        return -1;
    }

    //重定向标准输出流，标准错误流，关闭标准输入流
    strcat(dir,"/");
    if(log_name != NULL){
        strcat(dir,log_name);
    }
    strcat(dir,"_reply.log");
    fclose(stdout);
    fclose(stderr);
    freopen(dir,"a+",stdout);
    freopen(dir,"a+",stderr);
    fclose(stdin);
    free(dir);

    //输出日志头
    time(&now);
    timenow = localtime(&now);
    printf("\n------------------------------------------------------------\nPLAYER_LOG %s\n",asctime(timenow));

    return 0;

}

int close_sdt_stream(void)
{
    time_t   now;
    struct   tm     *timenow;

    //输出日志尾
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

        //检查参数个数
        if(n < 6){
                return -1;
        }

        //清空变量
        bzero(info,sizeof(startup_info));

        //服务器协议族初始化
        info->service_addr.sin_family = AF_INET;

        //服务器IP地址转换
        if(inet_pton(AF_INET,para[1],&info->service_addr.sin_addr) != 1){
                return -1;
        }

        //服务器端口号转换
        port = atoi(para[2]);
        if((port<1)||(port>65535)){
                return -1;
        }
        info->service_addr.sin_port = htons(port);

        //客户端协议族初始化
        info->client_addr.sin_family = AF_INET;

        //客户端IP地址转换
        if(inet_pton(AF_INET,para[3],&info->client_addr.sin_addr) != 1){
                return -1;
        }

        //客户端端口号转换
        port = atoi(para[4]);
        if((port < 1)||(port > 65535)){
                return -1;
        }
        info->client_addr.sin_port = htons(port);

        //检查提取PID
        info->pid_i = atoi(para[5]);
        if((info->pid_i == 0)||(strlen(para[5]) >= 20)){
                return -1;
        }
        strcpy(info->pid,para[5]);

        //检查提取pname
        if((n >= 7)&&(strlen(para[6]) < 20)){
                strcpy(info->pname,para[6]);
        }else{
                strcpy(info->pname,"Rong");
        }

        return 0;
}

int open_connect_socket(int *sockfd,startup_info * info)
{
    int i = 0;

    *sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(*sockfd == -1){
        return -1;
    }

    if(bind(*sockfd,(struct sockaddr *)&(info->client_addr),sizeof(struct sockaddr_in )) == -1){
        return -1;
    }

    while(connect(*sockfd,(struct sockaddr *)&(info->service_addr),sizeof(struct sockaddr_in )) == -1){
        i++;
        sleep(1);
        if(i > 10){
            return -1;
        }
    }

    return 0;

}

int close_socket(int sockfd)
{
    close(sockfd);
}

int reg_player(int sockfd,const startup_info *info)
{
    char data[60];

    //组建数据报文
    bzero(data,sizeof(data));
    strcpy(data,"reg: ");
    strcat(data,info->pid);
    strcat(data," ");
    strcat(data,info->pname);
    strcat(data," ");
    strcat(data,"\n");

    //发送数据报文
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
        //服务器断开链接，返回
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
        //接收数据
        rev_cnt = 0;
        rev_cnt = read_until_xx(sockfd,data,n,"\n",1);

        //判断链接是否断开
        if(rev_cnt <= 0){
            return COLL_COLSED;
        }

        //判断报文头格式
        if(strncmp((const char *)data,(const char *)"inquire/",8) == 0){
            rev_ct = 0;
            rev_ct = read_until_xx(sockfd,(data+rev_cnt),(n-rev_cnt),"/inquire",8);
            //判断链接是否断开
            if(rev_cnt <= 2){
                return COLL_ERROR;
            }else{
                return COLL_INQUIRE;
            }

        }else if(strncmp((const char *)data,(const char *)"seat/",5) == 0){
            rev_ct = 0;
            rev_ct = read_until_xx(sockfd,(data+rev_cnt),(n-rev_cnt),"/seat",5);
            //判断链接是否断开
            if(rev_cnt <= 2){
                return COLL_ERROR;
            }else{
                return COLL_SEAT;
            }
        }else if(strncmp((const char *)data,(const char *)"blind/",6) == 0){
            rev_ct = 0;
            rev_ct = read_until_xx(sockfd,(data+rev_cnt),(n-rev_cnt),"/blind",6);
            //判断链接是否断开
            if(rev_cnt <= 2){
                return COLL_ERROR;
            }else{
                return COLL_BLIND;
            }
        }else if(strncmp((const char *)data,(const char *)"hold/",5) == 0){
            rev_ct = 0;
            rev_ct = read_until_xx(sockfd,(data+rev_cnt),(n-rev_cnt),"/hold",5);
            //判断链接是否断开
            if(rev_cnt <= 2){
                return COLL_ERROR;
            }else{
                return COLL_HOLD;
            }
        }else if(strncmp((const char *)data,(const char *)"flop/",5) == 0){
            rev_ct = 0;
            rev_ct = read_until_xx(sockfd,(data+rev_cnt),(n-rev_cnt),"/flop",5);
            //判断链接是否断开
            if(rev_cnt <= 2){
                return COLL_ERROR;
            }else{
                return COLL_FLOP;
            }
        }else if(strncmp((const char *)data,(const char *)"turn/",5) == 0){
            rev_ct = 0;
            rev_ct = read_until_xx(sockfd,(data+rev_cnt),(n-rev_cnt),"/turn",5);
            //判断链接是否断开
            if(rev_cnt <= 2){
                return COLL_ERROR;
            }else{
                return COLL_TURN;
            }
        }else if(strncmp((const char *)data,(const char *)"river/",6) == 0){
            rev_ct = 0;
            rev_ct = read_until_xx(sockfd,(data+rev_cnt),(n-rev_cnt),"/river",6);
            //判断链接是否断开
            if(rev_cnt <= 2){
                return COLL_ERROR;
            }else{
                return COLL_RIVER;
            }
        }else if(strncmp((const char *)data,(const char *)"showdown/",9) == 0){
            rev_ct = 0;
            rev_ct = read_until_xx(sockfd,(data+rev_cnt),(n-rev_cnt),"/showdown",9);
            //判断链接是否断开
            if(rev_cnt <= 2){
                return COLL_ERROR;
            }else{
                return COLL_SHOWDOWN;
            }
        }else if(strncmp((const char *)data,(const char *)"pot-win/",8) == 0){
            rev_ct = 0;
            rev_ct = read_until_xx(sockfd,(data+rev_cnt),(n-rev_cnt),"/pot-win",8);
            //判断链接是否断开
            if(rev_cnt <= 2){
                return COLL_ERROR;
            }else{
                return COLL_POT_WIN;
            }
        }else if(strncmp((const char *)data,(const char *)"game-over",9) == 0){
            //游戏结束
            return COLL_GAME_OVER;
        }else{
            //无法识别数据报文
        }
    }
}

const static int color_str_len[] = {-1,6,6,8,5};
const static unsigned char collor_index[] = {0xff,0x10,0x20,0x40,0x80};
const static unsigned char point_p_index[] = {0xff,POINT_10_P,POINT_2_P,POINT_3_P,\
                                             POINT_4_P,POINT_5_P,POINT_6_P,POINT_7_P,\
                                             POINT_8_P,POINT_9_P,POINT_A_P,POINT_J_P,\
                                             POINT_Q_P,POINT_K_P};
const static unsigned char point_r_index[] = {0xff,POINT_10_R,POINT_2_R,POINT_3_R,\
                                             POINT_4_R,POINT_5_R,POINT_6_R,POINT_7_R,\
                                             POINT_8_R,POINT_9_R,POINT_A_R,POINT_J_R,\
                                             POINT_Q_R,POINT_K_R};
const static unsigned short point_b_index[] = {0xffff,POINT_10_B,POINT_2_B,POINT_3_B,\
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

static int handle_inquire(char *data,game_info *g_info)
{
    return COMM_OK;
}
static int handle_seat(char *data,game_info *g_info,startup_info * s_info)
{
    int i = 0,pid;
    static int s_round = 0;
    unsigned char  * tmp = NULL;
    p_node *player_p = NULL;
    //检查报文头
    if(strncmp((const char *)data,(const char *)"seat/",5) != 0){
        return COMM_ERROR;
    }

    //更新比赛轮数
    s_round ++;
    g_info->round = s_round;

    mask_all_seating_order_with(0,g_info);

    for(i = 1;i < 9;i++){
        if((data = strstr(data,"\n")) == NULL){
            return COMM_ERROR;
        }
        data += 1;

        if(strcmp(data,"/seat") == 0){
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
    }

    del_all_seating_order_with(0,g_info);

    g_info->total_player = i-1;

    i = 0;
    i =  get_seating_order_by_pid(s_info->pid_i,g_info);
    if(i<=0){
        return COMM_ERROR;
    }else if(i >3){
        g_info->my_role = ROLE_PLAYER;
    }else if(i == 1){
        g_info->my_role = ROLE_BUTTON;
    }else if((g_info->total_player <3)){
        g_info->my_role = ROLE_BLIND;
    }else if(i == 2){
        g_info->my_role = ROLE_SBLIND;
    }else{
        g_info->my_role = ROLE_BBLIND;
    }
    g_info->my_seating = i;

    //更新比赛阶段
    g_info->stage = STAGE_SEAT;

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

    //检查数据报头
    if(strncmp((const char *)data,(const char *)"hold/",5) != 0){
        return COMM_ERROR;
    }

    //清除记录
    bzero(g_info->c_hold,8);

    //逐条读取消息
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

    //更新比赛阶段
    g_info->stage = STAGE_HLOD;

    return COMM_OK;
}

static int handle_flop(char *data,game_info *g_info)
{
    int i;
    unsigned char * tmp = NULL;

    //检查数据报头
    if(strncmp((const char *)data,(const char *)"flop/",5) != 0){
        return COMM_ERROR;
    }

    //清除记录
    bzero(g_info->c_flop,12);

    //逐条读取消息
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

    //更新比赛阶段
    g_info->stage = STAGE_FLOP;

    return COMM_OK;

}

static int handle_turn(char *data,game_info *g_info)
{
    unsigned char * tmp = NULL;

    //检查数据报头
    if(strncmp((const char *)data,(const char *)"turn/",5) != 0){
        return COMM_ERROR;
    }

    //清除记录
    bzero(&g_info->c_turn,4);

    //逐条读取消息
    tmp = strstr(data,"\n");
    if(tmp == NULL){
        return COMM_ERROR;
    }
    tmp += 1;

    if(card_str_to_byte((char *)&g_info->c_turn,tmp) != COMM_OK){
        return COMM_ERROR;
    }

    //更新比赛阶段
    g_info->stage = STAGE_TURN;

    return COMM_OK;
}

static int handle_river(char *data,game_info *g_info)
{
    unsigned char * tmp = NULL;

    //检查数据报头
    if(strncmp((const char *)data,(const char *)"river/",6) != 0){
        return COMM_ERROR;
    }

    //清除记录
    bzero(&g_info->c_river,4);

    //逐条读取消息
    tmp = strstr(data,"\n");
    if(tmp == NULL){
        return COMM_ERROR;
    }
    tmp += 1;

    if(card_str_to_byte((char *)&g_info->c_river,tmp) != COMM_OK){
        return COMM_ERROR;
    }

    //更新比赛阶段
    g_info->stage = STAGE_RIVER ;

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

int collect_info(int sockfd, game_info *g_info,startup_info *s_info)
{
    char data[600];

    while(1){
        switch(read_packet(sockfd,data,600)){
        case COLL_INQUIRE:
            //printf("IN:\n%s\n",data);
            //处理inquire
            handle_inquire(data,g_info);
            return RE_COLL_ACTION;
        case COLL_SEAT:
            printf("\nSeat:\n%s\n",data);
            //处理seat
            handle_seat(data,g_info,s_info);
            test_print(g_info);
            break;
        case COLL_BLIND:
            //printf("Blind:\n%s\n",data);
            //处理blind
            handle_bilnd(data,g_info);
            break;
        case COLL_HOLD:
            printf("\nHold:\n%s\n",data);
            //处理hold
            handle_hold(data,g_info);
            test_print(g_info);
            break;
        case COLL_FLOP:
            printf("\nFlop:\n%s\n",data);
            //处理flop
            handle_flop(data,g_info);
            test_print(g_info);
            break;
        case COLL_TURN:
            printf("\nTURN:\n%s\n",data);
            //处理turn
            handle_turn(data,g_info);
            test_print(g_info);
            break;
        case COLL_RIVER:
            printf("\nRIVER:\n%s\n",data);
            //处理river
            handle_river(data,g_info);
            test_print(g_info);
            break;
        case COLL_SHOWDOWN:
            //printf("SHOW Down:\n%s\n",data);
            //处理showdown
            handle_showdown(data,g_info);
            break;
        case COLL_POT_WIN:
            //printf("Pot Win:\n%s\n",data);
            //处理pot_win
            handle_pot_win(data,g_info);
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
            }else if(num > 5000){
                num = 5000;
            }
            sprintf(data,"raise %d\n",num);
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
