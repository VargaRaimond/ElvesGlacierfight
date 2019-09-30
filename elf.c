// Copyright Varga Raimond 2018
#include "./functions.h"

int main(int argc, char* argv[]) {
    (void)argc;
    int players, radius, center, end = 0;
    char input[LNAME], output[LNAME];
    glacier **board = NULL;
    helpers* elf;
    score *all;
    // take command line arg to build file names
    snprintf(input, sizeof(input), "%s", argv[0]);
    snprintf(output, sizeof(output), "%s", argv[0]);
    snprintf(&input[strlen(input)], sizeof(".in"), "%s", ".in");
    snprintf(&output[strlen(output)], sizeof(".out"), "%s", ".out");
    FILE *in = fopen(input, "rt");
    FILE *out = fopen(output, "wt");
    fscanf(in, "%d%d", &radius, &players);
    center = radius;
    all = (score*)calloc(players, sizeof(score));
    elf = (helpers*)calloc(players, sizeof(helpers));
    board = alloc_board(board, center, 1);
    // read initial data and form initial glacier
    board = read_data(board, radius, elf, players, all, in);
    form_glacier(board, radius, center);
    end = miss_glacier(board, elf, players, all, 1, out, center);
    // start executing commands if the game isn't already over
    if (end == 0) {
        read_commands(board, elf, &radius, center, players, all, in, out);
    }
    // free all memory
    board = alloc_board(board, center, 2);
    free(elf);
    free(all);
    fclose(in);
    fclose(out);
    return 0;
}
