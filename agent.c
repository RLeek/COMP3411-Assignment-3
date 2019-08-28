//COMP3411, Semester 1, Assignment 3
//Ricky Liu z5207969

//Questions 1:
//Briefly describe how your program works, including any algorithms and data 
//structures employed, and explain any design decisions you made along the way.

//The agent below uses a modified version of alpha-beta search. The main
//modification is that this alpha-beta search also accounts for average path 
//cost, instead of just the end-result. This is because if two final nodes have
//the same heurisitic value, our search would naturally choose the move that 
//was generated first, however this is not necessarily optimal given that the
//second final node has a higher average path cost. The higher average path 
//cost means that the second final node will likely have better 'alternative'
//moves given that the opponent AI plays differently to how we presume our 
//oppoent plays in our alpha-beta search. In 100 games, by accounting for 
//average path cost, we win approximately 5-10 more games. Our evaluation 
//heuristic ratios (i.e. How we weigh our heuristic values), have largely been 
//determined through choosing a value, running 100 games to compute the win
//percentage against ./lookt and then choosing a different value until we have
//maximized our wins. 

//Our alpha-beta search starts with determining if there is a winning move.
//If there is none, then we generate our moves in the order of the board. 
//We then evaluate each corresponding board we generate using our move. The 
//evaluation heuristic we use is the one used for a normal tic-tac toe board 
//minus the evaluation of the next board (based on our move) for the opponent. 
//The ratio is 1 to 0.6 respectively. We then add this value to a 'path cost' 
//variable. We then repeat this for the opponent, checking for a win and 
//then evaluating the board for the opponent minus evaluating the next board 
//for us. We then minus this from our path cost (since we want to maximise 
//our path cost, not the opponent). This process is repeated until the 
//terminal node. Here we use a different evaluation heurisitic. This 
//heuristic essentially searches through each board and determines whether 
//we have a winning move or our opponent has a winning move and then 
//adds/subtracts a given value. Our path cost is then divided by the depth 
//to compute an average which is added to this terminal node heuristic, which 
//we then return. The final move we choose is the move that returns the best\
//alpha value first.

//The struct we use to present the ultimate board is an array of smaller 
//boards in the form of a 9-length array. This was chosen because it made it 
//simple to isolate and evaluate individual boards.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "common.h"
#include "agent.h"
#include "game.h"

//Structures
struct board {
  int tiles[9];
};

struct ultimate_board {
  struct board *boards[9];
};

//Functions
void agent_init(void);
void agent_start(int player_num);
int agent_second_move(int board_num, int prev_move);
int agent_third_move(int board_num, int first_move, int prev_move);
int agent_next_move(int prev_move);
void agent_last_move(int prev_move);
void agent_gameover(int result, int cause);
void agent_end();
void agent_cleanup();

struct ultimate_board *initialize_board(struct ultimate_board *game_board );
struct ultimate_board *duplicate_board(struct ultimate_board *prev_board);
void mark_board(int player_char, int board, int position);
void simulated_mark_board(int player_char, int board, int position, struct ultimate_board *simulated_board);

int alpha_beta_search(struct ultimate_board* initial_board, int board, int curr_depth, int max_depth, int alpha, int beta, int combined_path_cost);
int final_node_evaluation(struct ultimate_board* initial_board, int board);
int node_evaluation(struct ultimate_board *game_board, int board, int player);
int combined_1(struct ultimate_board *game_board, int board, int player_num);
int combined_2(struct ultimate_board *game_board, int board, int player_num);
int find_win_move(struct ultimate_board *game_board, int board, int player);
void usage(char argv0[]);
void agent_parse_args(int argc, char *argv[]);

//Global Variables
int board_index = 0;
struct ultimate_board *game_board;
int player;
int opponent_player;





//Functions that are called by ./servt


//agent_init: Mallocs the game board and makes its values zero
void agent_init(void) {
  game_board = initialize_board(game_board);
}


//Agent_Start: Determines which player 
//we are and sets global values
void agent_start(int player_num) {
  if (player_num == 0) {
    player = 0;
    opponent_player = 1;
  } else {
    player = 1;
    opponent_player = 0;
  }
}


//Agent_Second_Move: Stores previous move and then
//decides next move
int agent_second_move(int board_num, int prev_move) {
  prev_move = prev_move - 1; //Convert to array index coordinates
  board_num = board_num - 1;
  mark_board(opponent_player, board_num, prev_move);
  int move = alpha_beta_search(game_board, board_index, 0, 10, -10000, 10000, 0);
  mark_board(player, board_index, move);
  return move + 1;
}


//Agent_Third_Move: Stores previous moves and then decides next
//move
int agent_third_move(int board_num, int first_move, int prev_move) {
  first_move = first_move - 1;
  prev_move = prev_move - 1;
  board_num = board_num - 1;
  mark_board(player, board_num, first_move);
  mark_board(opponent_player, board_index, prev_move);
  int move = alpha_beta_search(game_board, board_index, 0, 10, -10000, 10000, 0);
  mark_board(player, board_index, move);
  return move + 1;
}



//Agent_Next_Move: Stores previous
//and then decides next move
int agent_next_move(int prev_move) {
  prev_move = prev_move - 1;
  mark_board(opponent_player, board_index, prev_move);
  int move = alpha_beta_search(game_board, board_index, 0, 10, -10000, 10000, 0);
  mark_board(player, board_index, move);
  return move + 1;
}


//Agent_Last_Move: Determines final
//move
void agent_last_move(int prev_move) {
  mark_board(opponent_player, board_index, prev_move);
}


//Agent_Gameover: Tells us the end result
void agent_gameover(int result, int cause) {
  if (result == 2) {
    printf("WE WON!!\n");
  } else if (result == 3) {
    printf("WE LOST!!\n");
  } else if (result == 4) {
    printf("We drew\n");
  }
}


//Agent_End: For ending
//our agent
void agent_end() {
  //No action necessary
}


//Agent_Cleanup: For 
//cleaning up our agent
void agent_cleanup() {
  //No clean-up necessary
}





//Functions that manipulate and create tic-tac-toe boards


//initialize_board: Creates the game board using the structures above
struct ultimate_board *initialize_board(struct ultimate_board *game_board ) {
  game_board = malloc(sizeof(struct ultimate_board));
  int i = 0;
  while (i < 9) {
    game_board->boards[i] = malloc(sizeof(struct board));
    struct board *current_board = game_board->boards[i];
    int i2 = 0;
    while(i2 < 9) {
      current_board->tiles[i2] = -1;
      i2++;
    }
    i++;
  }
  return game_board;
}


//duplicate_board: Creates a duplicate board (used in alpha-beta search)
struct ultimate_board *duplicate_board(struct ultimate_board *prev_board) {
  struct ultimate_board *given_board = malloc(sizeof(struct ultimate_board));
  int i = 0;
  while (i < 9) {
    given_board->boards[i] = malloc(sizeof(struct board));
    struct board *current_board = given_board->boards[i];
    int i2 = 0;
    while(i2 < 9) {
      current_board->tiles[i2] = prev_board->boards[i]->tiles[i2];
      i2++;
    }
    i++;
  }
  return given_board;
}


//free_board: Frees a given board
void free_board(struct ultimate_board *given_board) {
  int i = 0;
  while(i < 9) {
    free(given_board->boards[i]);
    i++;
  }
  free(given_board);
}



//Mark_Board: Records a given move on our board
void mark_board(int player_char, int board, int position) {
  struct board *current_board = game_board->boards[board];
  current_board->tiles[position] = player_char;
  board_index = position;
}


//Simulated_Mark_Board: Recoards a given move on a local board (used in alpha-beta search)
void simulated_mark_board(int player_char, int board, int position, struct ultimate_board *simulated_board) {
  struct board *current_board = simulated_board->boards[board];
  current_board->tiles[position] = player_char;
}





//Functions for alpha-beta search and evaluation


//alpha_beta_search: Performs a modified alpha_beta_search that considers path cost
int alpha_beta_search(struct ultimate_board* initial_board, int board, int curr_depth, int max_depth, int alpha, int beta, int combined_path_cost) {
  struct board *current_board = initial_board->boards[board];
  if (curr_depth == max_depth) { 


    int value = final_node_evaluation(initial_board, board);
    return value + combined_path_cost/(curr_depth/2);


  } else if (curr_depth == 0) { //First_player


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
        int node_cost = node_evaluation(new_board, board,player) - 0.6*node_evaluation(new_board, i,opponent_player);
        combined_path_cost = node_cost + combined_path_cost;
        int new_alpha = alpha_beta_search(new_board, i, curr_depth+1, max_depth, alpha, beta, combined_path_cost);
        free_board(new_board);
        if (alpha < new_alpha) {
          alpha = new_alpha;
          best_move = i;
        }
      }
      i++;
    }
    return best_move; 


  } else if (curr_depth%2 == 0) { //For player


    int move = find_win_move(initial_board,board, player);
    if (move >= 0) {
      if (curr_depth > 1) {
        return 1000 + combined_path_cost/(curr_depth/2);
      } else {
        return 1000 + combined_path_cost/(curr_depth);
      }
    } 


    int i = 0;
    while (i < 9) {
      if (current_board->tiles[i] == -1) {
        struct ultimate_board *new_board = duplicate_board(initial_board);
        simulated_mark_board(player, board, i, new_board);
        int node_cost = node_evaluation(new_board, board, player) - 0.6*node_evaluation(new_board, i, opponent_player);
        combined_path_cost = node_cost + combined_path_cost;
        int new_alpha = alpha_beta_search(new_board, i, curr_depth+1, max_depth, alpha, beta, combined_path_cost);
        free_board(new_board);
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
      if (curr_depth > 1) {
        return -1000 + combined_path_cost/(curr_depth/2);
      } else {
        return -1000 + combined_path_cost/(curr_depth);
      }
    }


    int i = 0;
    while (i < 9) {
      if (current_board->tiles[i] == -1) {
        struct ultimate_board *new_board = duplicate_board(initial_board);
        simulated_mark_board(opponent_player, board, i, new_board);
        int new_beta = alpha_beta_search(new_board, i, curr_depth+1, max_depth, alpha, beta, combined_path_cost);
        free_board(new_board);
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


//Final_node_evlation: Evaluates final board usin an alternative heuristic
//for tic-tac-toe
int final_node_evaluation(struct ultimate_board* initial_board, int board) {
  int i = 0;
  int value = 0;
  while(i < 9) {
    int move1 = find_win_move(initial_board,i, opponent_player);
    int move2 = find_win_move(initial_board,i, player);
    if (board == i) {
      if (move1 < 0 && move2 >= 0) {
        value = value + 90;
      } else if (move1 >= 0 && move2 >= 0){
        value = value + 0;
      } else if (move2 < 0 && move1 >= 0) {
        value = value - 90;
      }
    }
    if (move1 < 0 && move2 >= 0) {
      value = value + 100;
    } else if (move1 >= 0 && move2 >= 0){
      value = value + 0;
    } else if (move2 < 0 && move1 >= 0) {
      value = value - 100;
    }
    i++;
  }
  return value;
}


//Node_evaluation: Evaluates a single board using the board heuristic for
//tic-tac-toe (given in tutorial);
int node_evaluation(struct ultimate_board *given_board, int board, int curr_player) {
  int curr_opponent = 0;
  if (curr_player == 0) {
    curr_opponent = 1;
  } else if (curr_player == 1) {
    curr_opponent = 0;
  }
  int value = 3 * combined_2(given_board, board, curr_player) +  combined_1(given_board, board, curr_player) - (3 * combined_2(given_board, board, curr_opponent) + combined_1(given_board, board, curr_opponent));
  return value;
}


//combined_1: Finds all rows that only contain one x or o 
//(depending on player)
int combined_1(struct ultimate_board *given_board, int board, int player_num) {
  struct board *current_board = given_board->boards[board];
  int number_of_1 = 0;

  //Rows

  //Row 1
  if (current_board->tiles[0] == -1 && current_board->tiles[1] == -1 && current_board->tiles[2] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[2] == -1 && current_board->tiles[1] == -1 && current_board->tiles[0] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[0] == -1 && current_board->tiles[2] == -1 && current_board->tiles[1] == player_num) {
    number_of_1++;
  }
  //Row 2
  if (current_board->tiles[3] == -1 && current_board->tiles[4] == -1 && current_board->tiles[5] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[5] == -1 && current_board->tiles[4] == -1 && current_board->tiles[3] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[3] == -1 && current_board->tiles[5] == -1 && current_board->tiles[4] == player_num) {
    number_of_1++;
  }
  //Row 3
  if (current_board->tiles[6] == -1 && current_board->tiles[7] == -1 && current_board->tiles[8] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[6] == -1 && current_board->tiles[8] == -1 && current_board->tiles[7] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[8] == -1 && current_board->tiles[7] == -1 && current_board->tiles[6] == player_num) {
    number_of_1++;
  }

  //Columns

  //Column 1
  if (current_board->tiles[0] == -1 && current_board->tiles[3] == -1 && current_board->tiles[6] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[6] == -1 && current_board->tiles[0] == -1 && current_board->tiles[3] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[6] == -1 && current_board->tiles[3] == -1 && current_board->tiles[0] == player_num) {
    number_of_1++;
  }
  //Column 2
  if (current_board->tiles[1] == -1 && current_board->tiles[4] == -1 && current_board->tiles[7] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[1] == -1 && current_board->tiles[7] == -1 && current_board->tiles[4] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[4] == -1 && current_board->tiles[7] == -1 && current_board->tiles[1] == player_num) {
    number_of_1++;
  }
  //Column 3
  if (current_board->tiles[2] == -1 && current_board->tiles[5] == -1 && current_board->tiles[8] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[2] == -1 && current_board->tiles[8] == -1 && current_board->tiles[5] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[8] == -1 && current_board->tiles[5] == -1 && current_board->tiles[2] == player_num) {
    number_of_1++;
  }

  //Diagonals

  //Diagonal 1
  if (current_board->tiles[0] == -1 && current_board->tiles[4] == -1 && current_board->tiles[8] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[0] == -1 && current_board->tiles[8] == -1 && current_board->tiles[4] == player_num) {
    number_of_1++;
  }
  if (current_board->tiles[8] == -1 && current_board->tiles[4] == -1 && current_board->tiles[0] == player_num) {
    number_of_1++;
  }
  //Diagonal 2
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


//combined_2: Finds all rows that only contain two xs or os
//(depending on player)
int combined_2(struct ultimate_board *given_board, int board, int player_num) {
  struct board *current_board = given_board->boards[board];
  int number_of_2 = 0;

  //Rows

  //Row 1
  if (current_board->tiles[0] == player_num && current_board->tiles[1] == player_num && current_board->tiles[2] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[2] == player_num && current_board->tiles[1] == player_num && current_board->tiles[0] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[0] == player_num && current_board->tiles[2] == player_num && current_board->tiles[1] == -1) {
    number_of_2++;
  }
  //Row 2
  if (current_board->tiles[3] == player_num && current_board->tiles[4] == player_num && current_board->tiles[5] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[5] == player_num && current_board->tiles[4] == player_num && current_board->tiles[3] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[3] == player_num && current_board->tiles[5] == player_num && current_board->tiles[4] == -1) {
    number_of_2++;
  }
  //Row 3
  if (current_board->tiles[6] == player_num && current_board->tiles[7] == player_num && current_board->tiles[8] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[6] == player_num && current_board->tiles[8] == player_num && current_board->tiles[7] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[8] == player_num && current_board->tiles[7] == player_num && current_board->tiles[6] == -1) {
    number_of_2++;
  }

  //Column

  //Column 1
  if (current_board->tiles[0] == player_num && current_board->tiles[3] == player_num && current_board->tiles[6] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[6] == player_num && current_board->tiles[0] == player_num && current_board->tiles[3] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[6] == player_num && current_board->tiles[3] == player_num && current_board->tiles[0] == -1) {
    number_of_2++;
  }
  //Column 2
  if (current_board->tiles[1] == player_num && current_board->tiles[4] == player_num && current_board->tiles[7] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[1] == player_num && current_board->tiles[7] == player_num && current_board->tiles[4] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[4] == player_num && current_board->tiles[7] == player_num && current_board->tiles[1] == -1) {
    number_of_2++;
  }
  //Column 3
  if (current_board->tiles[2] == player_num && current_board->tiles[5] == player_num && current_board->tiles[8] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[2] == player_num && current_board->tiles[8] == player_num && current_board->tiles[5] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[8] == player_num && current_board->tiles[5] == player_num && current_board->tiles[2] == -1) {
    number_of_2++;
  }

  //Diagonals

  //Diagonal 1
  if (current_board->tiles[0] == player_num && current_board->tiles[4] == player_num && current_board->tiles[8] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[0] == player_num && current_board->tiles[8] == player_num && current_board->tiles[4] == -1) {
    number_of_2++;
  }
  if (current_board->tiles[8] == player_num && current_board->tiles[4] == player_num && current_board->tiles[0] == -1) {
    number_of_2++;
  }
  //Diagonal 2
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


//find_win_move: Finds any possible winning move for a player
//(depending on player)
int find_win_move(struct ultimate_board *given_board, int board, int player_num) {
  struct board *current_board = given_board->boards[board];
  int winning_move = -1;

  //Rows

  //Row 1
  if (current_board->tiles[0] == player_num && current_board->tiles[1] == player_num && current_board->tiles[2] == -1) {
     winning_move = 2;
  }
  if (current_board->tiles[2] == player_num && current_board->tiles[1] == player_num && current_board->tiles[0] == -1) {
    winning_move = 0;
  }
  if (current_board->tiles[0] == player_num && current_board->tiles[2] == player_num && current_board->tiles[1] == -1) {
    winning_move = 1;
  }
  //Row 2
  if (current_board->tiles[3] == player_num && current_board->tiles[4] == player_num && current_board->tiles[5] == -1) {
    winning_move = 5;
  }
  if (current_board->tiles[5] == player_num && current_board->tiles[4] == player_num && current_board->tiles[3] == -1) {
    winning_move = 3;
  }
  if (current_board->tiles[3] == player_num && current_board->tiles[5] == player_num && current_board->tiles[4] == -1) {
    winning_move = 4;
  }
  //Row 3
  if (current_board->tiles[6] == player_num && current_board->tiles[7] == player_num && current_board->tiles[8] == -1) {
    winning_move = 8;
  }
  if (current_board->tiles[6] == player_num && current_board->tiles[8] == player_num && current_board->tiles[7] == -1) {
    winning_move = 7;
  }
  if (current_board->tiles[8] == player_num && current_board->tiles[7] == player_num && current_board->tiles[6] == -1) {
    winning_move = 6;
  }

  //Column

  //Column 1
  if (current_board->tiles[0] == player_num && current_board->tiles[3] == player_num && current_board->tiles[6] == -1) {
    winning_move = 6;
  }
  if (current_board->tiles[6] == player_num && current_board->tiles[0] == player_num && current_board->tiles[3] == -1) {
    winning_move = 3;
  }
  if (current_board->tiles[6] == player_num && current_board->tiles[3] == player_num && current_board->tiles[0] == -1) {
    winning_move = 0;
  }
  //Column 2
  if (current_board->tiles[1] == player_num && current_board->tiles[4] == player_num && current_board->tiles[7] == -1) {
    winning_move = 7;
  }
  if (current_board->tiles[1] == player_num && current_board->tiles[7] == player_num && current_board->tiles[4] == -1) {
    winning_move = 4;
  }
  if (current_board->tiles[4] == player_num && current_board->tiles[7] == player_num && current_board->tiles[1] == -1) {
    winning_move = 1;
  }
  //Column 3
  if (current_board->tiles[2] == player_num && current_board->tiles[5] == player_num && current_board->tiles[8] == -1) {
    winning_move = 8;
  }
  if (current_board->tiles[2] == player_num && current_board->tiles[8] == player_num && current_board->tiles[5] == -1) {
    winning_move = 5;
  }
  if (current_board->tiles[8] == player_num && current_board->tiles[5] == player_num && current_board->tiles[2] == -1) {
    winning_move = 2;
  }

  //Diagonal

  //Diagonal 1
  if (current_board->tiles[0] == player_num && current_board->tiles[4] == player_num && current_board->tiles[8] == -1) {
    winning_move = 8;
  }
  if (current_board->tiles[0] == player_num && current_board->tiles[8] == player_num && current_board->tiles[4] == -1) {
    winning_move = 4;
  }
  if (current_board->tiles[8] == player_num && current_board->tiles[4] == player_num && current_board->tiles[0] == -1) {
    winning_move = 0;
  }
  //Diagonal 2
  if (current_board->tiles[2] == player_num && current_board->tiles[4] == player_num && current_board->tiles[6] == -1) {
    winning_move = 6;
  }
  if (current_board->tiles[2] == player_num && current_board->tiles[6] == player_num && current_board->tiles[4] == -1) {
    winning_move = 4;
  }
  if (current_board->tiles[6] == player_num && current_board->tiles[4] == player_num && current_board->tiles[2] == -1) {
    winning_move = 2;
  }
  return winning_move;
}





//For parsing commands


void usage(char argv0[]) {
  printf("Usage: %s\n",argv0);
  printf("       [-p port]\n"); // tcp port
  printf("       [-h host]\n"); // tcp host
  exit(1);
}

void agent_parse_args(int argc, char *argv[]) {
  int i = 1;
  while(i < argc) {
    if(strcmp( argv[i], "-p") == 0) {
      if(i+1 >= argc) {
        usage(argv[0]);
      }
      port = atoi(argv[i + 1]);
      i += 2;
    }
    else if(strcmp( argv[i], "-h") == 0 ) {
      if(i + 1 >= argc) {
        usage(argv[0]);
      }
      host = argv[i + 1];
      i += 2;
    }
    else {
      usage(argv[0]);
    }
  }
}



