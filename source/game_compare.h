#ifndef GAME_COMPARE_H
#define GAME_COMPARE_H

#include "game_info.h"

#define	STRAIGHT_FLUSH	1
#define	FOUR_OF_A_KIND	2
#define	FULL_HOUSE	3
#define	FLUSH		4
#define	STRAIGHT	5
#define	THREE_OF_A_KIND	6
#define	TWO_PAIR	7
#define	ONE_PAIR	8
#define	HIGH_CARD	9

#define TEN_PAIR 4206
#define ACE_PAIR 3326
#define ACE_HIGH 6186
#define FOUR_DEUCES 166
#define ACE_STRAIGHT 1600
#define FIVE_STRAIGHT 1609

typedef struct poker_inform{
    int c_position;
    int rank;
}poker_info;

extern const unsigned short int flushes[];
extern const unsigned short int unique5[];
extern const unsigned short int hash_adjust[];
extern const unsigned short int hash_values[];
extern const int lookup_prime[];


int five_hand_compare(int card[5]);
int combine(int a[],int n,int m);
float win_rate(game_info inform );

#endif
