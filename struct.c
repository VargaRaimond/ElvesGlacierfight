// Copyright Varga Raimond 2018
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#define ERR -1
#define LNAME 20

// struct for the glacier with altitude, glove power
// and the glacier with it's current limits
typedef struct {
    int glove;
    int alt;
    int lim;
}glacier;

// struct for elfs proprieties throughout the game
typedef struct {
    char name[LNAME];
    int x, y;
    int hp, stamina;
    int dmg;
}helpers;


// struct for scoreboard
typedef struct {
    char name[LNAME];
    char cond[LNAME];
    int kills;
}score;
