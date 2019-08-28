#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "common.h"
#include "agent.h"
#include "game.h"


struct board {
  int tiles[9];
};

struct ultimate_board {
  struct board *boards[9];
};


//Change rating system for empty boards
//Create combinations early on
/*
|...||...||...|
|...||X..||...|
|...||...||..X|
--------------
|...||...||...|
|.O.||...||...|
|...||..X||...|
--------------
|...||...||..O|
|...||...||...|
|...||...||...|
--------------






|...||...||...|
|X..||...||...|
|...||...||...|
--------------
|...||...||...|
|.O.||X..||.X.|
|..O||O..||...|
--------------
|...||...||O.X|
|...||...||...|
|..X||...||...|


why would we choose 5??
|...||...||...|
|...||...||.X.|
|...||...||...|
--------------
|...||O..||...|
|...||...||...|
|...||...||..X|
--------------
|...||...||..O|
|...||...||...|
|...||...||...|
--------------
*/
//We start at board 0
/*
|...||...||...|
|.X.||...||.X.|
|..O||...||...|
--------------
|...||..O||...|
|...||...||...|
|...||...||...|
--------------
|...||X..||O..|
|...||...||...|
|...||...||..X|
--------------
*/
//Fix the above, scenario, we should not favour going to board 8 ever!!!
//We also never have negative board_evaluations?? (THIS SHOULD NOT BE THE CASE!)

//Globals
int board_index = 0;
struct ultimate_board *game_board;
int player;
int opponent_player;

int combined_2(struct ultimate_board *game_board, int board, int player_num);
void agent_init(void);
void usage( char argv0[] );
struct ultimate_board *initialize_board(struct ultimate_board *game_board );
void agent_start(int player_num);
int agent_second_move(int board_num, int prev_move);
void mark_board(int player, int board, int position);
void simulated_mark_board(int player, int board, int position, struct ultimate_board *simulated_board);
int agent_third_move(int board_num, int first_move, int prev_move);
int agent_next_move(int prev_move);
void agent_loss(int result, int cause);
void agent_win(int result, int cause);
void agent_draw(int result, int cause);
void agent_end();
int alpha_beta_search(struct ultimate_board* initial_board, int board, int curr_depth, int max_depth, int alpha, int beta, int combined_path_cost);
struct ultimate_board *duplicate_board(struct ultimate_board *prev_board);
int node_evaluation(struct ultimate_board *game_board, int board, int player);
int combined_1(struct ultimate_board *game_board, int board, int player_num);
void agent_parse_args( int argc, char *argv[] );
int combined_2(struct ultimate_board *game_board, int board, int player_num);
int find_win_move(struct ultimate_board *game_board, int board, int player);
void printer_board(struct ultimate_board *game_board);
void agent_cleanup();

void agent_init(void) {
  game_board = initialize_board(game_board);
}

/*********************************************************//*
   Print usage information and exit
*/
void usage( char argv0[] )
{
  printf("Usage: %s\n",argv0);
  printf("       [-p port]\n"); // tcp port
  printf("       [-h host]\n"); // tcp host
  exit(1);
}

/*********************************************************//*
   Parse command-line arguments
*/
void agent_parse_args( int argc, char *argv[] )
{
  int i=1;
  while( i < argc ) {
    if( strcmp( argv[i], "-p" ) == 0 ) {
      if( i+1 >= argc ) {
        usage( argv[0] );
      }
      port = atoi(argv[i+1]);
      i += 2;
    }
    else if( strcmp( argv[i], "-h" ) == 0 ) {
      if( i+1 >= argc ) {
        usage( argv[0] );
      }
      host = argv[i+1];
      i += 2;
    }
    else {
      usage( argv[0] );
    }
  }
}

struct ultimate_board *initialize_board(struct ultimate_board *game_board ) {
  game_board = malloc(sizeof(struct ultimate_board));
  int i = 0;
  while (i < 9) {
    game_board->boards[i] = malloc(sizeof(struct board));
    struct board* current_board = game_board->boards[i];
    int i2 = 0;
    while(i2 < 9) {
      current_board->tiles[i2] = -1;
      i2++;
    }
    i++;
  }
  return game_board;
}
void printer_board(struct ultimate_board *game_board) {
  int board_row = 0;
  int ultimate_board_row = 0;
  int i = 0;
  while (i < 3) {
    int i2 = 0;
    while(i2 < 3) { 
      int i3 = 0;
      while(i3 < 3) { 
        int i4 = 0;
        struct board* current_board = game_board->boards[ultimate_board_row + i3];
        printf("|");
        while(i4  < 3) {
          if (current_board->tiles[i4 + board_row] == -1) {
            printf(".");
          } else if (current_board->tiles[i4 + board_row] == 0) {
            printf("X");
          } else {
            printf("O");
          }
          i4++;
        }
        printf("|");
        i3++;
      }
      board_row = board_row + 3;
      if (board_row == 9) {
        board_row = 0;
      }
      i2++;
      printf("\n");
    }
    ultimate_board_row = ultimate_board_row + 3;
    printf("--------------\n");
    i++;
  }
}

void agent_start(int player_num) {
  printf("%d\n", player_num);
  if (player_num == 0) {
    player = 0;
    opponent_player = 1;
    printf("WE ARE X\n");
  } else {
    player = 1;
    opponent_player = 0;
    printf("WE ARE O\n");
  }
}


int agent_second_move(int board_num, int prev_move) {
  prev_move = prev_move-1;
  board_num = board_num-1;
  mark_board(opponent_player, board_num, prev_move);
  int move = alpha_beta_search(game_board, board_index, 0, 8, -10000, 10000, 0);
  mark_board(player, board_index, move);
  return move+1;
}

void mark_board(int player_char, int board, int position) {
  struct board *current_board = game_board->boards[board];
  current_board->tiles[position] = player_char;
  board_index = position;
}

void simulated_mark_board(int player_char, int board, int position, struct ultimate_board *simulated_board) {
  struct board *current_board = simulated_board->boards[board];
  current_board->tiles[position] = player_char;
}


int agent_third_move(int board_num, int first_move, int prev_move) {
  first_move = first_move-1;
  prev_move = prev_move - 1;
  board_num = board_num-1;
  mark_board(player, board_num, first_move);
  mark_board(opponent_player, board_index, prev_move);
  int move = alpha_beta_search(game_board, board_index, 0, 8, -10000, 10000, 0);
  mark_board(player, board_index, move);
  printf("Our Move: %d\n", move+1);
  return move+1;
}



int agent_next_move(int prev_move) {
  prev_move = prev_move-1;
  mark_board(opponent_player, board_index, prev_move);
  int move = alpha_beta_search(game_board, board_index, 0, 8, -10000, 10000, 0);
  mark_board(player, board_index, move);
  printf("Our Move: %d\n", move+1);
  printer_board(game_board);
  return move+1;
}
void agent_last_move(int prev_move) {
  mark_board(opponent_player, board_index, prev_move);
}

void agent_end() {

}
void agent_gameover( int result, int cause )   {
  if (result == 2) {
    printf("WE WON!!\n");
  } else if (result == 3) {
    printf("WE LOST!!\n");
  } else if (result == 4) {
    printf("We drew\n");
  }
}

void agent_cleanup() {

}

int alpha_beta_search(struct ultimate_board* initial_board, int board, int curr_depth, int max_depth, int alpha, int beta, int combined_path_cost) {
  struct board *current_board =  initial_board->boards[board];
  if (curr_depth == max_depth) { 
    int i = 0;
    int value = 0;
    while(i < 9) {
      if (i == board) {
        value = value + 5 * node_evaluation(initial_board, i ,player);
      } else {
        value = value + node_evaluation(initial_board, i, player);
      }
      i++;
    }
    return value + combined_path_cost/curr_depth;
  } else if (curr_depth == 0) { 
    int move = find_win_move(initial_board,board, player);
    if (move >= 0) {
      return move;
    } 
    int best_move = 0;
    int i = 0;
    while (i < 9) {
      if (current_board->tiles[i] == -1) {
        struct ultimate_board *new_board = duplicate_board(initial_board);
        simulated_mark_board(player, board, i, new_board);
        int node_cost = node_evaluation(new_board, board,player) - 2*node_evaluation(new_board, i,opponent_player);
        combined_path_cost = node_cost + combined_path_cost;
        int new_alpha = alpha_beta_search(new_board, i, curr_depth+1, max_depth, alpha, beta, combined_path_cost);
        fprintf(stdout,"Alpha: %d move: %d\n", new_alpha,i );
        if (alpha < new_alpha) {
          alpha = new_alpha;
          best_move = i;
        }
      }
      i++;
    }
    return best_move; 
  } else if (curr_depth%2 == 0) { 
    int move = find_win_move(initial_board,board, player);
    if (move >= 0) {
      return 1000 + combined_path_cost/(curr_depth);
    } 
    int i = 0;
    while (i < 9) {
      if (current_board->tiles[i] == -1) {
        struct ultimate_board *new_board = duplicate_board(initial_board);
        simulated_mark_board(player, board, i, new_board);
        int node_cost = node_evaluation(new_board, board, player) - 2*node_evaluation(new_board, i, opponent_player);
        combined_path_cost = node_cost + combined_path_cost;
        int new_alpha = alpha_beta_search(new_board, i, curr_depth+1, max_depth, alpha, beta, combined_path_cost);
        if (alpha < new_alpha) {
          alpha = new_alpha;
        }
        if (alpha >= beta) {
          return alpha;
        }
      }
      i++;
    }
    return alpha;
  } else if (curr_depth%2 == 1) { //For opponent
    int move = find_win_move(initial_board,board, opponent_player);
    if (move >= 0) {
      return -1000 + combined_path_cost/(curr_depth);
    }
    int i = 0;
    while (i < 9) {
      if (current_board->tiles[i] == -1) {
        struct ultimate_board *new_board = duplicate_board(initial_board);
        simulated_mark_board(opponent_player, board, i, new_board);
        int node_cost = node_evaluation(new_board, board, opponent_player) - 2*node_evaluation(new_board, i, player);
        combined_path_cost = node_cost + combined_path_cost;
        int new_beta = alpha_beta_search(new_board, i, curr_depth+1, max_depth, alpha, beta, combined_path_cost);
        if (beta > new_beta) {
          beta = new_beta;
        }
        if (alpha >= beta) {
          return beta;
        }
      }
      i++;
    }
    return beta;
  }
  return 0 + combined_path_cost/(curr_depth);
}

struct ultimate_board *duplicate_board(struct ultimate_board *prev_board) {
  struct ultimate_board *game_board = malloc(sizeof(struct ultimate_board));
  int i = 0;
  while (i < 9) {
    game_board->boards[i] = malloc(sizeof(struct board));
    struct board* current_board = game_board->boards[i];
    int i2 = 0;
    while(i2 < 9) {
      current_board->tiles[i2] = prev_board->boards[i]->tiles[i2];
      i2++;
    }
    i++;
  }
  return game_board;
}

int node_evaluation(struct ultimate_board *game_board, int board, int player) {
  int opponent = 0;
  if (player == 0) {
    opponent = 1;
  } else {
    opponent = 0;
  }
  int value = 3 * combined_2(game_board, board, player) +  combined_1(game_board, board, player) - (3 * combined_2(game_board, board, opponent) + combined_1(game_board, board, opponent));
  return value;
}

int combined_1(struct ultimate_board *game_board, int board, int player_num) {
  struct board *current_board = game_board->boards[board];
  int number_of_1 = 0;
  if (current_board->tiles[0] == -1 && current_board->tiles[1] == -1 && current_board->tiles[2] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[2] == -1 && current_board->tiles[1] == -1 && current_board->tiles[0] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[0] == -1 && current_board->tiles[2] == -1 && current_board->tiles[1] == player_num) {
    number_of_1++;
  }

  if (current_board->tiles[3] == -1 && current_board->tiles[4] == -1 && current_board->tiles[5] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[5] == -1 && current_board->tiles[4] == -1 && current_board->tiles[3] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[3] == -1 && current_board->tiles[5] == -1 && current_board->tiles[4] == player_num) {
    number_of_1++;
  }

  if (current_board->tiles[6] == -1 && current_board->tiles[7] == -1 && current_board->tiles[8] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[6] == -1 && current_board->tiles[8] == -1 && current_board->tiles[7] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[8] == -1 && current_board->tiles[7] == -1 && current_board->tiles[6] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[0] == -1 && current_board->tiles[3] == -1 && current_board->tiles[6] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[6] == -1 && current_board->tiles[0] == -1 && current_board->tiles[3] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[6] == -1 && current_board->tiles[3] == -1 && current_board->tiles[0] == player_num) {
    number_of_1++;
  }

  if (current_board->tiles[1] == -1 && current_board->tiles[4] == -1 && current_board->tiles[7] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[1] == -1 && current_board->tiles[7] == -1 && current_board->tiles[4] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[4] == -1 && current_board->tiles[7] == -1 && current_board->tiles[1] == player_num) {
    number_of_1++;
  }

  if (current_board->tiles[2] == -1 && current_board->tiles[5] == -1 && current_board->tiles[8] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[2] == -1 && current_board->tiles[8] == -1 && current_board->tiles[5] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[8] == -1 && current_board->tiles[5] == -1 && current_board->tiles[2] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[0] == -1 && current_board->tiles[4] == -1 && current_board->tiles[8] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[0] == -1 && current_board->tiles[8] == -1 && current_board->tiles[4] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[8] == -1 && current_board->tiles[4] == -1 && current_board->tiles[0] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[2] == -1 && current_board->tiles[4] == -1 && current_board->tiles[6] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[2] == -1 && current_board->tiles[6] == -1 && current_board->tiles[4] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[6] == -1 && current_board->tiles[4] == -1 && current_board->tiles[2] == player_num) {
    number_of_1++;
  }

  return number_of_1;

}

int combined_2(struct ultimate_board *game_board, int board, int player_num) {
  struct board *current_board = game_board->boards[board];
  int number_of_2 = 0;
  if (current_board->tiles[0] == player_num && current_board->tiles[1] == player_num && current_board->tiles[2] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[2] == player_num && current_board->tiles[1] == player_num && current_board->tiles[0] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[0] == player_num && current_board->tiles[2] == player_num && current_board->tiles[1] == -1) {
    number_of_2++;
  }

  if (current_board->tiles[3] == player_num && current_board->tiles[4] == player_num && current_board->tiles[5] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[5] == player_num && current_board->tiles[4] == player_num && current_board->tiles[3] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[3] == player_num && current_board->tiles[5] == player_num && current_board->tiles[4] == -1) {
    number_of_2++;
  }

  if (current_board->tiles[6] == player_num && current_board->tiles[7] == player_num && current_board->tiles[8] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[6] == player_num && current_board->tiles[8] == player_num && current_board->tiles[7] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[8] == player_num && current_board->tiles[7] == player_num && current_board->tiles[6] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[0] == player_num && current_board->tiles[3] == player_num && current_board->tiles[6] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[6] == player_num && current_board->tiles[0] == player_num && current_board->tiles[3] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[6] == player_num && current_board->tiles[3] == player_num && current_board->tiles[0] == -1) {
    number_of_2++;
  }

  if (current_board->tiles[1] == player_num && current_board->tiles[4] == player_num && current_board->tiles[7] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[1] == player_num && current_board->tiles[7] == player_num && current_board->tiles[4] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[4] == player_num && current_board->tiles[7] == player_num && current_board->tiles[1] == -1) {
    number_of_2++;
  }

  if (current_board->tiles[2] == player_num && current_board->tiles[5] == player_num && current_board->tiles[8] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[2] == player_num && current_board->tiles[8] == player_num && current_board->tiles[5] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[8] == player_num && current_board->tiles[5] == player_num && current_board->tiles[2] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[0] == player_num && current_board->tiles[4] == player_num && current_board->tiles[8] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[0] == player_num && current_board->tiles[8] == player_num && current_board->tiles[4] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[8] == player_num && current_board->tiles[4] == player_num && current_board->tiles[0] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[2] == player_num && current_board->tiles[4] == player_num && current_board->tiles[6] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[2] == player_num && current_board->tiles[6] == player_num && current_board->tiles[4] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[6] == player_num && current_board->tiles[4] == player && current_board->tiles[2] == -1) {
    number_of_2++;
  }

  return number_of_2;

}
int find_win_move(struct ultimate_board *game_board, int board, int player) {
  struct board *current_board = game_board->boards[board];
  int winning_move = -1;
    if (current_board->tiles[0] == player && current_board->tiles[1] == player && current_board->tiles[2] == -1) {
      winning_move = 2;
    }
    if (current_board->tiles[2] == player && current_board->tiles[1] == player && current_board->tiles[0] == -1) {
      winning_move = 0;
    }
    if (current_board->tiles[0] == player && current_board->tiles[2] == player && current_board->tiles[1] == -1) {
      winning_move = 1;
    }

    if (current_board->tiles[3] == player && current_board->tiles[4] == player && current_board->tiles[5] == -1) {
      winning_move = 5;
    }
    if (current_board->tiles[5] == player && current_board->tiles[4] == player && current_board->tiles[3] == -1) {
      winning_move = 3;
    }
    if (current_board->tiles[3] == player && current_board->tiles[5] == player && current_board->tiles[4] == -1) {
      winning_move = 4;
    }

    if (current_board->tiles[6] == player && current_board->tiles[7] == player && current_board->tiles[8] == -1) {
      winning_move = 8;
    }
    if (current_board->tiles[6] == player && current_board->tiles[8] == player && current_board->tiles[7] == -1) {
      winning_move = 7;
    }
    if (current_board->tiles[8] == player && current_board->tiles[7] == player && current_board->tiles[6] == -1) {
      winning_move = 6;
    }
    if (current_board->tiles[0] == player && current_board->tiles[3] == player && current_board->tiles[6] == -1) {
      winning_move = 6;
    }
    if (current_board->tiles[6] == player && current_board->tiles[0] == player && current_board->tiles[3] == -1) {
      winning_move = 3;
    }
    if (current_board->tiles[6] == player && current_board->tiles[3] == player && current_board->tiles[0] == -1) {
      winning_move = 0;
    }

    if (current_board->tiles[1] == player && current_board->tiles[4] == player && current_board->tiles[7] == -1) {
      winning_move = 7;
    }
    if (current_board->tiles[1] == player && current_board->tiles[7] == player && current_board->tiles[4] == -1) {
      winning_move = 4;
    }
    if (current_board->tiles[4] == player && current_board->tiles[7] == player && current_board->tiles[1] == -1) {
      winning_move = 1;
    }

    if (current_board->tiles[2] == player && current_board->tiles[5] == player && current_board->tiles[8] == -1) {
      winning_move = 8;
    }
    if (current_board->tiles[2] == player && current_board->tiles[8] == player && current_board->tiles[5] == -1) {
      winning_move = 5;
    }
    if (current_board->tiles[8] == player && current_board->tiles[5] == player && current_board->tiles[2] == -1) {
      winning_move = 2;
    }
    if (current_board->tiles[0] == player && current_board->tiles[4] == player && current_board->tiles[8] == -1) {
      winning_move = 8;
    }
    if (current_board->tiles[0] == player && current_board->tiles[8] == player && current_board->tiles[4] == -1) {
      winning_move = 4;
    }
    if (current_board->tiles[8] == player && current_board->tiles[4] == player && current_board->tiles[0] == -1) {
      winning_move = 0;
    }
    if (current_board->tiles[2] == player && current_board->tiles[4] == player && current_board->tiles[6] == -1) {
      winning_move = 6;
    }
    if (current_board->tiles[2] == player && current_board->tiles[6] == player && current_board->tiles[4] == -1) {
      winning_move = 4;
    }
    if (current_board->tiles[6] == player && current_board->tiles[4] == player && current_board->tiles[2] == -1) {
      winning_move = 2;
    }

  return winning_move;
}

