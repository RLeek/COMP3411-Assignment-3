/*********************************************************
 *  agent.c
 *  Nine-Board Tic-Tac-Toe Agent
 *  COMP3411/9414/9814 Artificial Intelligence
 *  Alan Blair, CSE, UNSW
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "common.h"
#include "agent.h"
#include "game.h"

#define MAX_MOVE 81
#define MAX_DEPTH 11
#define INFINITY 1000000

int board[10][10];
int move[MAX_MOVE+1];
int player;
int m;

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

/*********************************************************//*
   Called at the beginning of a series of games
*/
void agent_init()
{
  struct timeval tp;

  // generate a new random seed each time
  gettimeofday( &tp, NULL );
  srandom(( unsigned int )( tp.tv_usec ));
}

/*********************************************************//*
   Called at the beginning of each game
*/
void agent_start( int this_player )
{
  reset_board( board );
  m = 0;
  move[m] = 0;
  player = this_player;
}

/*********************************************************//*
   Choose second move and return it
*/
int agent_second_move( int board_num, int prev_move )
{
  int b_move;
  int i;
  int j;
  int board_copy[10][10];
  
  move[0] = board_num;
  move[1] = prev_move;
  board[board_num][prev_move] = !player;
  for (i = 1; i <= 9; i++) {
    for (j = 1; j <= 9; j++) {
        board_copy[i][j] = board[i][j];
    }
  }
  m = 2;
  b_move = alpha_beta(-INFINITY, INFINITY, MAX_DEPTH, prev_move, board_copy);
  move[m] = b_move;
  board[prev_move][b_move] = player;
  return( b_move );
}

/*********************************************************//*
   Choose third move and return it
*/
int agent_third_move(
                     int board_num,
                     int first_move,
                     int prev_move
                    )
{
  int b_move;
  int i;
  int j;
  
  move[0] = board_num;
  move[1] = first_move;
  move[2] = prev_move;
  board[board_num][first_move] =  player;
  board[first_move][prev_move] = !player;
  int board_copy[10][10];
  for (i = 1; i <= 9; i++) {
    for (j = 1; j <= 9; j++) {
        board_copy[i][j] = board[i][j];
    }
  }
  m=3;
  b_move = alpha_beta(-INFINITY, INFINITY, MAX_DEPTH, prev_move, board_copy);
  move[m] = b_move; // basically checking through random generation if empty or not.
  board[move[m-1]][b_move] = player;
  return( b_move );
}

/*********************************************************//*
   Choose next move and return it
*/
int agent_next_move( int prev_move )
{
  int i;
  int j;
  int k;
  m++;
  move[m] = prev_move;
  board[move[m-1]][move[m]] = !player;
  //printf("%d\n", board[move[m-1]][move[m]]);
  //printf("-----------------------\n");
  for (k = 0; k <= m; k++) {
    printf("%d\n", move[k]);
  }
  m++;
  int b_move = 1;
  //printf("-----------------------\n");
  int board_copy[10][10];
  for (i = 1; i <= 9; i++) {
    for (j = 1; j <= 9; j++) {
        board_copy[i][j] = board[i][j];
    }
  }
  
  //printf("%d\n", move[m-1]);
  //printf("%d %d %d\n%d %d %d\n%d %d %d\n", board[move[m-1]][1], board[move[m-1]][2], board[move[m-1]][3], board[move[m-1]][4], board[move[m-1]][5], board[move[m-1]][6], board[move[m-1]][7], board[move[m-1]][8], board[move[m-1]][9]);
  //printf("-----------------------\n");
  //printf("%d %d %d\n%d %d %d\n%d %d %d\n", board_copy[move[m-1]][1], board_copy[move[m-1]][2], board_copy[move[m-1]][3], board_copy[move[m-1]][4], board_copy[move[m-1]][5], board_copy[move[m-1]][6], board_copy[move[m-1]][7], board_copy[move[m-1]][8], board_copy[move[m-1]][9]);
  //printf("-----------------------\n");
  //printf("%d %d %d\n%d %d %d\n%d %d %d\n", board_copy[1][1], board_copy[1][2], board_copy[1][3], board_copy[1][4], board_copy[1][5], board_copy[1][6], board_copy[1][7], board_copy[1][8], board_copy[1][9]);
  b_move = alpha_beta(-INFINITY, INFINITY, MAX_DEPTH, prev_move, board_copy);
  /*do {
    this_move = 1 + random()% 9;
  } while( board[prev_move][this_move] != EMPTY );
  move[m] = this_move; */
  board[move[m-1]][b_move] = player;
  move[m] = b_move;
  //printf("Move square: %d\n", b_move);
  return( b_move );
}

/*********************************************************//*
   Receive last move and mark it on the board
*/
void agent_last_move( int prev_move )
{
  m++;
  move[m] = prev_move;
  board[move[m-1]][move[m]] = !player;
}

/*********************************************************//*
    Win Checking 
*/
// currently will only check if a player has won.
int win_check(int board_num, int curr_player, int ab_board[10][10]) { // board_num is actually just the player move.
    if (ab_board[board_num][1] == curr_player && ab_board[board_num][2] == curr_player && ab_board[board_num][3] == curr_player) {
        return 1;
    } else if (ab_board[board_num][4] == curr_player && ab_board[board_num][5] == curr_player && ab_board[board_num][6] == curr_player) {
        return 1;
    } else if (ab_board[board_num][7] == curr_player && ab_board[board_num][8] == curr_player && ab_board[board_num][9] == curr_player) {
        return 1;
    } else if (ab_board[board_num][1] == curr_player && ab_board[board_num][4] == curr_player && ab_board[board_num][7] == curr_player) {
        return 1;
    } else if (ab_board[board_num][2] == curr_player && ab_board[board_num][5] == curr_player && ab_board[board_num][8] == curr_player) {
        return 1;
    } else if (ab_board[board_num][3] == curr_player && ab_board[board_num][6] == curr_player && ab_board[board_num][9] == curr_player) {
        return 1;
    } else if (ab_board[board_num][1] == curr_player && ab_board[board_num][5] == curr_player && ab_board[board_num][9] == curr_player) {
        return 1;
    } else if (ab_board[board_num][3] == curr_player && ab_board[board_num][5] == curr_player && ab_board[board_num][7] == curr_player) {
        return 1;
    } else {
        return 0; // need to work here for actual heuristic
    }
}

/*********************************************************//*
    Positional Advantage Counter
*/
int pos_count(int board_num, int curr_player, int ab_board[10][10]) {
    // 1 2 3
    if (ab_board[board_num][1] == curr_player && ab_board[board_num][2] == curr_player && ab_board[board_num][3] == EMPTY) {
        return win_loc(board_num, ab_board);
    } else if (ab_board[board_num][1] == curr_player && ab_board[board_num][3] == curr_player && ab_board[board_num][2] == EMPTY) {
        return win_loc(board_num, ab_board);
    } else if (ab_board[board_num][2] == curr_player && ab_board[board_num][3] == curr_player && ab_board[board_num][1] == EMPTY) {
        return win_loc(board_num, ab_board);
    // 4 5 6
    } else if (ab_board[board_num][4] == curr_player && ab_board[board_num][5] == curr_player && ab_board[board_num][6] == EMPTY) {
        return win_loc(board_num, ab_board);
    } else if (ab_board[board_num][4] == curr_player && ab_board[board_num][6] == curr_player && ab_board[board_num][5] == EMPTY) {
        return win_loc(board_num, ab_board);
    } else if (ab_board[board_num][5] == curr_player && ab_board[board_num][6] == curr_player && ab_board[board_num][4] == EMPTY) {
        return win_loc(board_num, ab_board);
    // 7 8 9
    } else if (ab_board[board_num][7] == curr_player && ab_board[board_num][8] == curr_player && ab_board[board_num][9] == EMPTY) {
        return win_loc(board_num, ab_board);
    } else if (ab_board[board_num][7] == curr_player && ab_board[board_num][9] == curr_player && ab_board[board_num][8] == EMPTY) {
        return win_loc(board_num, ab_board);
    } else if (ab_board[board_num][8] == curr_player && ab_board[board_num][9] == curr_player && ab_board[board_num][7] == EMPTY) {
        return win_loc(board_num, ab_board);
    // 1 4 7
    } else if (ab_board[board_num][1] == curr_player && ab_board[board_num][4] == curr_player && ab_board[board_num][7] == EMPTY) {
        return win_loc(board_num, ab_board);
    } else if (ab_board[board_num][1] == curr_player && ab_board[board_num][7] == curr_player && ab_board[board_num][4] == EMPTY) {
        return win_loc(board_num, ab_board);
    } else if (ab_board[board_num][4] == curr_player && ab_board[board_num][7] == curr_player && ab_board[board_num][1] == EMPTY) {
        return win_loc(board_num, ab_board);
    // 2 5 8
    } else if (ab_board[board_num][2] == curr_player && ab_board[board_num][5] == curr_player && ab_board[board_num][8] == EMPTY) {
        return win_loc(board_num, ab_board);
    } else if (ab_board[board_num][2] == curr_player && ab_board[board_num][8] == curr_player && ab_board[board_num][5] == EMPTY) {
        return win_loc(board_num, ab_board);
    } else if (ab_board[board_num][5] == curr_player && ab_board[board_num][8] == curr_player && ab_board[board_num][2] == EMPTY) {
        return win_loc(board_num, ab_board);
    // 3 6 9
    } else if (ab_board[board_num][3] == curr_player && ab_board[board_num][6] == curr_player && ab_board[board_num][9] == EMPTY) {
        return win_loc(board_num, ab_board);
    } else if (ab_board[board_num][3] == curr_player && ab_board[board_num][9] == curr_player && ab_board[board_num][6] == EMPTY) {
        return win_loc(board_num, ab_board);
    } else if (ab_board[board_num][6] == curr_player && ab_board[board_num][9] == curr_player && ab_board[board_num][3] == EMPTY) {
        return win_loc(board_num, ab_board);
    // 1 5 9
    } else if (ab_board[board_num][1] == curr_player && ab_board[board_num][5] == curr_player && ab_board[board_num][9] == EMPTY) {
        return win_loc(board_num, ab_board);
    } else if (ab_board[board_num][1] == curr_player && ab_board[board_num][9] == curr_player && ab_board[board_num][5] == EMPTY) {
        return win_loc(board_num, ab_board);
    } else if (ab_board[board_num][5] == curr_player && ab_board[board_num][9] == curr_player && ab_board[board_num][1] == EMPTY) {
        return win_loc(board_num, ab_board);
    // 3 5 7
    } else if (ab_board[board_num][3] == curr_player && ab_board[board_num][5] == curr_player && ab_board[board_num][7] == EMPTY) {
        return win_loc(board_num, ab_board);
    } else if (ab_board[board_num][3] == curr_player && ab_board[board_num][7] == curr_player && ab_board[board_num][5] == EMPTY) {
        return win_loc(board_num, ab_board);
    } else if (ab_board[board_num][5] == curr_player && ab_board[board_num][7] == curr_player && ab_board[board_num][3] == EMPTY) {
        return win_loc(board_num, ab_board);
    }
    return 0;
}

/*********************************************************//*
    Value Counter
*/
int win_loc(int board_num, int ab_board[10][10]) {
    int i;
    int total = 0;
    for (i = 1; i <= 9; i++) {
        if (ab_board[i][board_num] == EMPTY && i != board_num) {
            total++;   
        }
    }
    return total;
}

/*********************************************************//*
    Heuristics
*/
// 
    int heuristics(int ab_board[10][10]) {
        int i;
        int score = 0;
        for (i = 1; i <= 9; i++) {
            score += pos_count(i, player, ab_board);
            score -= pos_count(i, !player, ab_board);
        }
        return score;
    }

/*********************************************************//*
    Alpha Beta Pruning (Recursive) - Actually altered alpha beta pruning
*/
int alpha_beta(int alpha, int beta, int depth, int subboard, int board_copy[10][10]) {
	int j;
	int k;
	int win;
	// printf("Depth: %d\n", depth);
	if (depth == 0) {
		int value;
		int i;
		for (i = 1; i <= 9 ; i++) {
			if (board_copy[subboard][i] == EMPTY) {
				int new_board_copy[10][10];
				for (j = 1; j <= 9; j++) {
					for (k = 1; k <= 9; k++) {
						new_board_copy[j][k] = board_copy[j][k];
					}
				}
				new_board_copy[subboard][i] = !player;
				win = win_check(subboard, !player, new_board_copy);
				if (win == 1) {
				    return -INFINITY; // losing move.
				}
	        }
	    }
		value = heuristics(board_copy);
		return value;
	} else if (depth == MAX_DEPTH) {
		int best_move = 0;
		int value;
		int i;
		for (i = 1; i <= 9 ; i++) {
			if (board_copy[subboard][i] == EMPTY) {
				int new_board_copy[10][10];
				for (j = 1; j <= 9; j++) {
					for (k = 1; k <= 9; k++) {
						new_board_copy[j][k] = board_copy[j][k];
					}
				}
				new_board_copy[subboard][i] = player;
				win = win_check(subboard, player, new_board_copy);
				if (win == 1) {
				    return i; // winning move.
				}
				value = alpha_beta(alpha, beta, depth-1, i, new_board_copy);
				if (alpha < value) {
					alpha = value;
					best_move = i;
				}
				if (alpha >= beta) {
				    // printf("Alpha is greater than beta!\n");
					return best_move;
				}
			}
		}
		while (board_copy[subboard][best_move] != EMPTY) {
		    best_move++; // this is to make sure that it does not get stuck if there is only losing moves.
		}
		return best_move;
	} else if (depth % 2 == 1) {
		int value;
		int i;
		for (i = 1; i <= 9 ; i++) {
			if (board_copy[subboard][i] == EMPTY) {
				int new_board_copy[10][10];
				for (j = 1; j <= 9; j++) {
					for (k = 1; k <= 9; k++) {
						new_board_copy[j][k] = board_copy[j][k];
					}
				}
				new_board_copy[subboard][i] = player;
				win = win_check(subboard, player, new_board_copy);
				if (win == 1) {
				    return INFINITY; // winning move.
				}
				value = alpha_beta(alpha, beta, depth-1, i, new_board_copy);
				if (alpha < value) {
					alpha = value;
				}
				if (alpha >= beta) {
				    // printf("Alpha is greater than beta!\n");
					return alpha;
				}
			}
		}
		return alpha;
	} else {
		int value;
		int i;
		for (i = 1; i <= 9 ; i++) {
			if (board_copy[subboard][i] == EMPTY) {
				int new_board_copy[10][10];
				for (j = 1; j <= 9; j++) {
					for (k = 1; k <= 9; k++) {
						new_board_copy[j][k] = board_copy[j][k];
					}
				}
				new_board_copy[subboard][i] = !player;
				win = win_check(subboard, !player, new_board_copy);
				if (win == 1) {
				    return -INFINITY; // losing move.
				}
				value = alpha_beta(alpha, beta, depth-1, i, new_board_copy);
				if (beta > value) {
					beta = value;
				}
				if (alpha >= beta) {
				    // printf("Alpha is greater than beta!\n");
					return beta;
				}
			}
		}
		return beta;
	}
}

/*********************************************************//*
   Called after each game
*/
void agent_gameover(
                    int result,// WIN, LOSS or DRAW
                    int cause  // TRIPLE, ILLEGAL_MOVE, TIMEOUT or FULL_BOARD
                   )
{
    if (result == WIN) {
        printf("WE WON!\n");
    } else if (result == LOSS) {
        printf("WE LOST\n");
    } else {
        printf("DRAW\n");
    }
  // nothing to do here
}

/*********************************************************//*
   Called after the series of games
*/
void agent_cleanup()
{
  // nothing to do here
}
