// Copyright Varga Raimond 2018
#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "./struct.c"

void swap1(int* a, int* b);
// give and free memory for the board
glacier** alloc_board(glacier** board, int radius, int case1);
// read initial data
glacier** read_data(glacier** board, int radius, helpers* elf, int players,
                            score* all, FILE* in);
// draw glacier borders
glacier** form_glacier(glacier** board, int radius, int center);
// check if elfs fall off the glacier
int miss_glacier(glacier** board, helpers* elf, int players, score* all,
                        int case2, FILE *out, int center);
// read and execute commands
void read_commands(glacier** board, helpers* elf, int* radius, int center,
                            int players, score* all, FILE *in, FILE *out);
// simulate a storm and it's aftershock
int snowstorm_start(int storm, helpers* elf, score* all, int players,
                            FILE *out);
// give stamina to elfs surviving a meltdown
void powerup(helpers* elf, int players, int staminaplus, score* all);
// print the current scoreboard
void print_score(score* all, int players, FILE *out);
// move an elf if it's possible
int move_elf(glacier** board, helpers* elf, int players, score* all,
                    char move, int id, FILE *out, int center);
// check if 2 elfs ended up in the same square
int check_battle(helpers* elf, int players, score* all, int id, FILE *out);
// simulate a fight between 2 elfs and decide on the winner
int fight_elfs(helpers* elf, score* all, int p1, int p2, int players,
                        FILE *out);
// check if someone won the game
int winner(score* all, int players, FILE* out);

#endif
