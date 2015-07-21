#include <stdio.h>
#include <math.h>
#include <string.h>
#include<time.h>
#include "game_compare.h"
#include "game_info.h"
static int min=7462;
static int b[5];

static unsigned int find_fast(unsigned int u)
{
    unsigned a, b, r;
    u += 0xe91aaa35;
    u ^= u >> 16;
    u += u << 8;
    u ^= u >> 4;
    b  = (u >> 8) & 0x1ff;
    a  = (u + (u << 2)) >> 19;
    r  = a ^ hash_adjust[b];
    return r;
}

int five_hand_compare(int card[5])
{
    short s;
    int q = (card[0] | card[1] | card[2] | card[3] | card[4]) >> 16;

    if (card[0] & card[1] & card[2] & card[3] & card[4] & 0xf000){
        return flushes[q]; // 牌型为同花
    }
    if ((s = unique5[q])){
        return s;          // 牌型为顺子或高牌
    }
    return hash_values[find_fast((card[0] & 0xff) * (card[1] & 0xff) * (card[2] & 0xff) * (card[3] & 0xff) * (card[4] & 0xff))];
}


int get_hand_rank( short int val )
{
    if (val > 6185) return(HIGH_CARD);        // 1277种 高牌
    if (val > 3325) return(ONE_PAIR);         // 2860种 单对
    if (val > 2467) return(TWO_PAIR);         //  858种 两队
    if (val > 1609) return(THREE_OF_A_KIND);  //  858种 三条
    if (val > 1599) return(STRAIGHT);         //   10种 顺子
    if (val > 322)  return(FLUSH);            // 1277种 同花
    if (val > 166)  return(FULL_HOUSE);       //  156种 葫芦
    if (val > 10)   return(FOUR_OF_A_KIND);   //  156种 四条
    return(STRAIGHT_FLUSH);                   //   10种 同花顺
}

static int combinex(int a[],int n,int m)
{
    int i,j;

    int c[5];
    int value;

    for(i=n; i>=m; i--){
        b[m-1] = i - 1;
        if (m > 1){
            combinex(a,i-1,m-1);
        }else{
            for(j=4; j>=0; j--)
                c[j]=a[b[j]];
            value=five_hand_compare(c);
            if(value<min){
                min=value;
            }
        }
    }
    return min;
}

int combine(int a[],int n,int m)
{
    min=7462;
    min=combinex(a,n,m);
    return min;
}

static void poker_array(int array[52])
{
    int i,j;
    int deuce=0x00011002;
    int part1=0;
    int part2=(deuce >> 8) & 0xf;
    int part3=deuce & 0xf000;
    int part4=(deuce >>16) & 0xffff;
    int part_one;
    int part_two;
    int part_three;
    int part_four;

    for(i=0;i<13;i++){
        part_one=(part1 | lookup_prime[i]);
        part_two=(part2 | i)<<8;
        part_three=part3;
        part_four=(int)((part4)*pow(2,i))<<16;
        for(j=0;j<4;j++){
            array[i*4+j]=part_one+part_two+(part_three << j)+part_four;
        }
    }
}

static void delete_same(int a[52],game_info inform)
{
    int i=0;
    if(inform.stage==STAGE_FLOP){
        for(i=0;i<52;i++){
            if(a[i]==inform.c_hold[0]||a[i]==inform.c_hold[1]||a[i]==inform.c_flop[0]||a[i]==inform.c_flop[1]||a[i]==inform.c_flop[2]){
                a[i]=0;
            }
        }
    }else if(inform.stage==STAGE_TURN){
        for(i=0;i<52;i++){
            if(a[i]==inform.c_hold[0]||a[i]==inform.c_hold[1]||a[i]==inform.c_flop[0]||a[i]==inform.c_flop[1]||a[i]==inform.c_flop[2]||a[i]==inform.c_turn){
                a[i]=0;
            }
        }
    }else if(inform.stage==STAGE_RIVER){
        for(i=0;i<52;i++){
            if(a[i]==inform.c_hold[0]||a[i]==inform.c_hold[1]||a[i]==inform.c_flop[0]||a[i]==inform.c_flop[1]||a[i]==inform.c_flop[2]||a[i]==inform.c_turn||a[i]==inform.c_river){
                a[i]=0;
            }
        }
    }
}

float win_rate(game_info inform )
{
    int i,j,k,x,y;
    float t,s=1;
    int count=0;
    int a[52],g[5];
    int c_seq[9] = {0};
    int c,d,e,f=0;
    int value,hand_value,min_value=7462;
    double t3;
    clock_t t1,t2;
    t1=clock();
    
    //所有牌型存入数组
    poker_array(a);

    memcpy(c_seq,inform.c_hold,sizeof(int)*7);
    //计算手牌成牌概率
    if(inform.stage==STAGE_FLOP){
        for(x=0;x<52;x++){
            if((inform.c_hold[0]==a[x])||(inform.c_hold[1]==a[x])||(inform.c_flop[0]==a[x])||\
                    (inform.c_flop[1]==a[x])||(inform.c_flop[2]==a[x])){
                continue;
            }
            for(y=x+1;y<52;y++,i=0,j=0){
                if((inform.c_hold[0]==a[y])||(inform.c_hold[1]==a[y])||(inform.c_flop[0]==a[y])||\
                        (inform.c_flop[1]==a[y])||(inform.c_flop[2]==a[y])){
                    continue;
                }
                c_seq[5]=a[x];
                c_seq[6]=a[y];

                for(c=0;c<7;c++){
                    for(d=c+1;d<7;d++){
                        for(e=0;e<7;e++){
                            if((e!=c)&&(e!=d)){
                                g[f++]=c_seq[e];
                            }
                        }
                        f=0;
                        value=five_hand_compare(g);
                        if(value<min_value){
                            min_value=value;
                        }
                    }
                }
                hand_value=min_value;
                min_value=7462;

                for(i=0;i<52;i++){
                    if((inform.c_hold[0]==a[i])||(inform.c_hold[1]==a[i])||(inform.c_flop[0]==a[i])||\
                            (inform.c_flop[1]==a[i])||(inform.c_flop[2]==a[i])||(a[x]==a[i])||(a[y]==a[i])){
                        continue;
                    }
                    for(j=i+1;j<52;j++){
                        if((inform.c_hold[0]==a[j])||(inform.c_hold[1]==a[j])||(inform.c_flop[0]==a[j])||\
                                (inform.c_flop[1]==a[j])||(inform.c_flop[2]==a[j])||(a[x]==a[j])||(a[y]==a[j])){
                            continue;
                        }
                        c_seq[7]=a[i];
                        c_seq[8]=a[j];

                        for(c=0;c<7;c++){
                            for(d=c+1;d<7;d++){
                                for(e=0;e<7;e++){
                                    if((e!=c)&&(e!=d)){
                                        g[f++]=c_seq[e+2];
                                    }
                                }
                                f=0;
                                value=five_hand_compare(g);
                                if(value<min_value){
                                    min_value=value;
                                }
                            }
                        }
                        if(min_value>=hand_value){
                            count++;
                        }
                        min_value=7462;
                    }
                }
            }
        }
        for(i=inform.total_player-inform.total_flod-1;i>0;i--){
            t=(float)(count-i+1)/(1070190-i+1);
            s=s*t;
        }
        t2=clock();
        t3 = (double)(t2 - t1)/CLOCKS_PER_SEC;
        printf("shijian.........................................:%f\n",t3);
        return s;
    }else if(inform.stage==STAGE_TURN){
        for(k=0;k<52;k++,i=0,j=0){
            if(inform.c_hold[0]==a[k]||inform.c_hold[1]==a[k]||inform.c_flop[0]==a[k]||\
                    inform.c_flop[1]==a[k]||inform.c_flop[2]==a[k]||inform.c_turn==a[k]){
                continue;
            }
            c_seq[6]=a[k];
            hand_value=combine(c_seq,7,5);

            for(i=0;i<52;i++){
                if(inform.c_hold[0]==a[i]||inform.c_hold[1]==a[i]||inform.c_flop[0]==a[i]||\
                        inform.c_flop[1]==a[i]||inform.c_flop[2]==a[i]||inform.c_turn==a[i]||a[k]==a[i]){
                    continue;
                }
                for(j=i+1;j<52;j++){
                    if(inform.c_hold[0]==a[j]||inform.c_hold[1]==a[j]||inform.c_flop[0]==a[j]||\
                            inform.c_flop[1]==a[j]||inform.c_flop[2]==a[j]||inform.c_turn==a[j]||a[k]==a[j]){
                        continue;
                    }
                    c_seq[7]=a[i];
                    c_seq[8]=a[j];
                    if(combine(&c_seq[2],7,5)>=hand_value){
                        count++;
                    }
                }
            }
        }
        for(i=inform.total_player-inform.total_flod-1;i>0;i--){
            t=(float)(count-i+1)/(45540-i+1);
            s=s*t;
        }
        return s;
    }else if(inform.stage==STAGE_RIVER){
        hand_value=combine(c_seq,7,5);
        for(i=0;i<52;i++){
            if((inform.c_hold[0]==a[i])||(inform.c_hold[1]==a[i])||(inform.c_flop[0]==a[i])||\
                    (inform.c_flop[1]==a[i])||(inform.c_flop[2]==a[i])||(inform.c_turn==a[i])||(inform.c_river==a[i])){
                continue;
            }
            for(j=i+1;j<52;j++){
                if((inform.c_hold[0]==a[j])||(inform.c_hold[1]==a[j])||(inform.c_flop[0]==a[j])||\
                        (inform.c_flop[1]==a[j])||(inform.c_flop[2]==a[j])||(inform.c_turn==a[j])||(inform.c_river==a[j])){
                    continue;
                }
                c_seq[7]=a[i];
                c_seq[8]=a[j];
                if(combine(&c_seq[2],7,5)>=hand_value){
                    count++;
                }
            }
        }
        for(i=inform.total_player-inform.total_flod-1;i>0;i--){
            t=(float)(count-i+1)/(990-i+1);
            s=s*t;
        }
        return s;
    }
    return 0;
}


