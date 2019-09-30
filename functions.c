// Copyright Varga Raimond 2018
#include "./functions.h"

// swap two integers
void swap1(int* a, int* b) {
    int aux;
    aux = *a;
    *a = *b;
    *b = aux;
}

glacier** read_data(glacier** board, int radius, helpers* elf, int players,
                            score* all, FILE* in) {
    int i, j;
    // the size of the initial board
    int size = 2 * radius + 1;
    for (i = 0; i < size; ++i) {
        for (j = 0; j < size; ++j) {
            // initialize the board altitude and glove power
            fscanf(in, "%d%d", &board[i][j].alt, &board[i][j].glove);
        }
    }

    for (i = 0; i < players; ++i) {
        // read initial elfs and start to form the scoreboard
        fscanf(in, "%s%d%d", elf[i].name, &elf[i].x, &elf[i].y);
        fscanf(in, "%d%d", &elf[i].hp, &elf[i].stamina);
        snprintf(all[i].name, sizeof(elf[i].name), "%s", elf[i].name);
        snprintf(all[i].cond, sizeof("DRY"), "%s", "DRY");
        swap1(&elf[i].dmg, &board[elf[i].x][elf[i].y].glove);
        board[elf[i].x][elf[i].y].glove = 0;
    }
    return board;
}

glacier** alloc_board(glacier** board, int radius, int case1) {
    int i, size = 2 * radius + 1;
    // alloc ar free memory for the board depending on the case1 parameter
    if (case1 == 1) {
        board = (glacier**)calloc(size, sizeof(glacier*));
        for (i = 0; i < size; ++i) {
            board[i] = (glacier*)calloc(size, sizeof(glacier));
        }
    } else {
        for (i = 0; i < size; ++i) {
            free(board[i]);
        }
        free(board);
    }
    return board;
}

glacier** form_glacier(glacier** board, int radius, int center) {
    int i, j;
    float dist;
    // glacier size
    int size = 2 * center + 1;
    for (i = 0; i < size; ++i) {
        for (j = 0; j < size; ++j) {
            // condition to make the glacier a circle with a certain
            // radius, basic formula to determine the distance between
             // two points in space
            dist = (center - i) * (center - i);
            dist += (center - j) * (center - j);
            dist = sqrt(dist);
            if (dist <= radius) {
                continue;
            }
            // if I'm out of the glacier I change the value to an
            // ERR = -1 in the matrix
            board[i][j].lim = ERR;
        }
    }
    return board;
}

// function that reads all commands and executes them
// I used C as the center of the circle so I could keep the lines shorter
void read_commands(glacier** board, helpers* elf, int* radius, int C,
                            int players, score* all, FILE *in, FILE *out) {
    char cmd[LNAME], move;
    int stamina_plus, id, storm, end = 0;
    // check which command I have to execute
    while (fscanf(in, "%s", cmd) == 1 && end == 0) {
        if (strcmp(cmd, "MOVE") == 0) {
            // read the ID so i know which elf i'll move
            fscanf(in, "%d %c", &id, &move);
            // check if the elf is still in the game and then move him
            if (strcmp(all[id].cond, "DRY") == 0) {
                end = move_elf(board, elf, players, all, move, id, out, C);
            } else {
                // if the elf isn't in the game, just read and ignore all
                // his moves
                while (strchr("RLDU", move) != NULL) {
                    fscanf(in, "%c", &move);
                }
                continue;
            }
            if (end == 1) {
                return;
            }
            // repeat anterior actions for all the moves
            while (strchr("RLDU", move) != NULL && end == 0) {
                fscanf(in, "%c", &move);
                if (strcmp(all[id].cond, "DRY") == 0) {
                    end = move_elf(board, elf, players, all, move, id, out, C);
                    if (end == 1) {
                        return;
                    }
                } else {
                    while (strchr("RLDU", move) != NULL) {
                        fscanf(in, "%c", &move);
                    }
                    break;
                }
            }
        } else {
            if (strcmp(cmd, "SNOWSTORM") == 0) {
                // read the storm number and simulate the storm
                fscanf(in, "%d", &storm);
                end = snowstorm_start(storm, elf, all, players, out);
            } else {
                if (strcmp(cmd, "PRINT_SCOREBOARD") == 0) {
                    // print the current score
                    print_score(all, players, out);
                } else {
                    if (strcmp(cmd, "MELTDOWN") == 0) {
                        fscanf(in, "%d", &stamina_plus);
                        // decrease the radius and reform
                        // the smaller glacier
                        --*radius;
                        board = form_glacier(board, *radius, C);
                        // miss_glacier with case2 = 2 as explained before
                        end = miss_glacier(board, elf, players, all, 2,
                                    out, C);
                        // give the bonus stamina
                        powerup(elf, players, stamina_plus, all);
                    } else {
                        return;
                    }
                }
            }
        }
    }
}

// execute the commanded storm
int snowstorm_start(int storm, helpers* elf, score* all, int players,
                            FILE *out) {
    int x, y, dmg, storm_radius, i;
    float dist;
    // extract every information from the initial number
    x = storm;
    x = (x << 24) >> 24;
    y = storm;
    y = (y << 16) >> 24;
    dmg = storm;
    dmg = dmg >> 24;
    dmg &= 255;
    storm_radius = storm;
    storm_radius = (storm_radius << 8) >> 24;

    // check if any elf is inside the storm and damage them
    for (i = 0; i < players; ++i) {
        if (x == elf[i].x && y == elf[i].y) {
            elf[i].hp -= dmg;
        } else {
            // calculate distance from elf to storm center
            dist = (x - elf[i].x) * (x - elf[i].x);
            dist += (y - elf[i].y) * (y - elf[i].y);
            dist = sqrt(dist);
            if (dist <= storm_radius) {
                elf[i].hp -= dmg;
            }
        }
    }

    // check if any elf's hp reached 0 or bellow
    for (i = 0; i < players; ++i) {
        if (strcmp(all[i].cond, "DRY") == 0) {
            if (elf[i].hp <= 0) {
                snprintf(all[i].cond, sizeof("WET"), "%s", "WET");
                fprintf(out, "%s was hit by snowstorm.\n", elf[i].name);
                // check if someone won
                if (winner(all, players, out)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

void powerup(helpers* elf, int players, int stamina_plus, score* all) {
  int i;
  // give the bonus stamina to all elfs that are still alive after the meltdown
  for (i = 0; i < players; ++i) {
    if (strcmp(all[i].cond, "DRY") == 0) {
        elf[i].stamina += stamina_plus;
    }
  }
}

void print_score(score* all, int players, FILE *out) {
    int i, j, *id;
    id = (int*)calloc(players, sizeof(int));
    // initial order of elves
    for (i = 0; i < players; ++i) {
        id[i] = i;
    }
    // bubble sort for the id array using condition, kills and lastly the
    // names of every elf as conditions
    for (i = 0; i < players - 1; ++i) {
        for (j = i + 1; j < players; ++j) {
            // sort by condition
            if (strcmp(all[id[i]].cond, all[id[j]].cond) > 0) {
                swap1(&id[i], &id[j]);
            } else {
                if (strcmp(all[id[i]].cond, all[id[j]].cond) == 0) {
                    // sort by kill count
                    if (all[id[i]].kills < all[id[j]].kills) {
                        swap1(&id[i], &id[j]);
                    } else {
                        if (all[id[i]].kills == all[id[j]].kills) {
                            // sort by names
                            if (strcmp(all[id[i]].name, all[id[j]].name) > 0) {
                                swap1(&id[i], &id[j]);
                            }
                        }
                    }
                }
            }
        }
    }
    // print elf names, condition and kill count with tabs between
    fprintf(out, "SCOREBOARD:\n");
    for (i = 0; i < players; i++) {
        fprintf(out, "%s\t%s\t%d\n", all[id[i]].name,
                        all[id[i]].cond, all[id[i]].kills);
    }
    free(id);
}

int miss_glacier(glacier** board, helpers* elf, int players, score* all,
                        int case2, FILE *out, int center) {
    int i;
    int size = 2 * center + 1;
    for (i = 0; i < players; ++i) {
    // check if the elf is still on the glacier
        if (board[elf[i].x][elf[i].y].lim == ERR || elf[i].x < 0 ||
                    elf[i].y < 0 || elf[i].x >= size || elf[i].y >= size) {
            if (strcmp(all[i].cond, "DRY") == 0) {
                // print that the elf has fallen and change his status to wet
                // the case2 variable depends on the place from where the
                // function was called: 1 when I check the initial positions
                // 2 for the meltdown command and the default case  when an
                // elf moves out of the glacier
                switch (case2) {
                    case 1:
                        if (winner(all, players, out)) {
                            return 1;
                        }
                        fprintf(out, "%s has missed the glacier.\n",
                                        elf[i].name);
                        break;
                    case 2:
                        if (winner(all, players, out)) {
                            return 1;
                        }
                        fprintf(out, "%s got wet because of global warming.\n",
                                        elf[i].name);
                        break;
                    default:
                        // this case checks if an elf moves out of the glacier
                        if (winner(all, players, out)) {
                            return 1;
                        }
                        fprintf(out, "%s fell off the glacier.\n", elf[i].name);
                }
                snprintf(all[i].cond, sizeof("WET"), "%s", "WET");
                if (winner(all, players, out) == 1) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

// moves an elf while checking if the move is possible
int move_elf(glacier** board, helpers* elf, int players, score* all,
                    char move, int id, FILE *out, int center) {
    // end will be 1 only if someone won the game
    // dif will be the difference in altitude between the 2 squares
    int end = 0, dif = 0, size = 2 * center;
    // check what direction was requested then check if the move is possible
    // if it's possible, make the move, consume stamina, check the new gloves
    // and then check if there will be a battle
    if (move == 'R') {
        // check if the elf steps out of the matrix defined for the game
        // and repeat this check for every direction he moves in
        if (elf[id].y + 1 <= size) {
            dif = board[elf[id].x][elf[id].y + 1].alt;
            dif -= board[elf[id].x][elf[id].y].alt;
            dif = abs(dif);
        }
        if (dif <= elf[id].stamina) {
            ++elf[id].y;
            elf[id].stamina -= dif;
        }
    }
    if (move == 'L') {
        if (elf[id].y - 1 >= 0) {
            dif = board[elf[id].x][elf[id].y - 1].alt;
            dif -= board[elf[id].x][elf[id].y].alt;
            dif = abs(dif);
        }
        if (dif <= elf[id].stamina) {
            --elf[id].y;
            elf[id].stamina -= dif;
        }
    }
    if (move == 'U') {
        if (elf[id].x - 1 >= 0) {
            dif = board[elf[id].x - 1][elf[id].y].alt;
            dif -= board[elf[id].x][elf[id].y].alt;
            dif = abs(dif);
        }
        if (dif <= elf[id].stamina) {
            --elf[id].x;
            elf[id].stamina -= dif;
        }
    }
    if (move == 'D') {
        if (elf[id].x + 1 <= size) {
            dif = board[elf[id].x + 1][elf[id].y].alt;
            dif -= board[elf[id].x][elf[id].y].alt;
            dif = abs(dif);
        }
        if (dif <= elf[id].stamina) {
            ++elf[id].x;
            elf[id].stamina -= dif;
        }
    }
    // check if the elf has fallen off after the last move
    end = miss_glacier(board, elf, players, all, 3, out, center);
    if (strcmp(all[id].cond, "WET") == 0) {
        return end;
    }
    // check gloves
    if (board[elf[id].x][elf[id].y].glove > elf[id].dmg) {
        swap1(&elf[id].dmg, &board[elf[id].x][elf[id].y].glove);
    }
    end = check_battle(elf, players, all, id, out);
    return end;
}

// checks if 2 elves ended up in the same cell
int check_battle(helpers* elf, int players, score* all, int id, FILE *out) {
    // end will be 1 if someone won the game
    int end = 0, i;
    for (i = 0; i < players; ++i) {
        // if they are in the same square and they're both still in game
        // let them fight
        if (elf[id].x == elf[i].x && elf[id].y == elf[i].y && i != id) {
            if (strcmp(all[i].cond, "DRY") == 0) {
                if (strcmp(all[id].cond, "DRY") == 0) {
                    end = fight_elfs(elf, all, id, i, players, out);
                    // if a fight happened it doesn't matter if there still are
                    // more elves in the same cell according to the ref file
                    return end;
                }
            }
        }
    }
    return end;
}

// function that gets 2 elves and simulates their fight
int fight_elfs(helpers* elf, score* all, int p1, int p2, int players,
                        FILE *out) {
    // the elfs fight until one reaches 0 hp or bellow
    while (elf[p1].hp > 0 && elf[p2].hp > 0) {
        // check who attacks first according to stamina
        // and then check everytime if the other elf can still attack
        if (elf[p1].stamina >= elf[p2].stamina) {
            elf[p2].hp -= elf[p1].dmg;
            if (elf[p2].hp > 0) {
                elf[p1].hp -= elf[p2].dmg;
            }
        } else {
            elf[p1].hp -= elf[p2].dmg;
            if (elf[p1].hp > 0) {
                elf[p2].hp -= elf[p1].dmg;
            }
        }
    }
    // print who sent whom home and update scoreboard
    // and transfer stamina from the loser to the winner
    if (elf[p1].hp <= 0) {
        ++all[p2].kills;
        elf[p2].stamina += elf[p1].stamina;
        elf[p1].stamina = 0;
        snprintf(all[p1].cond, sizeof("WET"), "%s", "WET");
        fprintf(out, "%s sent %s back home.\n", all[p2].name, all[p1].name);
        if (winner(all, players, out) == 1) {
            return 1;
        }
    } else {
        ++all[p1].kills;
        elf[p1].stamina += elf[p2].stamina;
        elf[p2].stamina = 0;
        snprintf(all[p2].cond, sizeof("WET"), "%s", "WET");
        fprintf(out, "%s sent %s back home.\n", all[p1].name, all[p2].name);
        if (winner(all, players, out) == 1) {
            return 1;
        }
    }
    return 0;
}

// check if someone won the game and print that
int winner(score* all, int players, FILE* out) {
    int i, alive_elfs = 0;
    char win[LNAME];
    for (i = 0; i < players; ++i) {
        if (strcmp(all[i].cond, "DRY") == 0) {
            alive_elfs++;
            snprintf(win, sizeof(all[i].name), "%s", all[i].name);
        }
    }
    // if only one elf is alive the game is over
    if (alive_elfs == 1) {
        fprintf(out, "%s has won.\n", win);
        return 1;
    }
    return 0;
}
