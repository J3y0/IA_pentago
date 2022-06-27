#ifndef PENTAGO_H_
#define PENTAGO_H_

#define SIZEX 6
#define SIZEY 6

#define WHITE 1
#define BLACK -1
#define EMPTY 0

#define EXIT_ERROR 126
#define MAXV INT16_MAX
#define WIN (MAXV-1)
#define BWIN (WIN-SIZEX*SIZEY)

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })


int index_pattern(int *operation, int array[SIZEX/2]);

int duo_index_pattern(int *operation, int array[2]);

void reverse_array(int array[], int length);

/*
 * A simple function that displays the board
*/
void print_board(int board[SIZEX][SIZEY]);

/*
 * Plays the move (x,y) on the board
 * @submatrix indicates which submatrix has been spun: whether 0,1,2,3
 * @clockwise is equal to 1 if the submatrix spun clockwise, -1 else
*/
void play(int x, int y, int color, int submatrix, int clockwise, int board[SIZEX][SIZEY]);

/*
 * Unplays the move with coordinates (x,y)
 * @submatrix indicates which submatrix has been spun: whether 0,1,2,3
 * @clockwise is the clockwise value used by play(). The inversion takes place in the body of this function
*/
void unplay(int x, int y, int submatrix, int clockwise, int board[SIZEX][SIZEY]);

/*
 * Compute transpose of the matrix given in parameter
 */
void transpose(int matrix[SIZEX/2][SIZEY/2]);

/*
 * Rotate the matrix by +/-90°, depending on clockwise value
 * @clockwise is the clockwise value: whether 1 or -1
 */
void rotate(int sub_board[SIZEX/2][SIZEY/2], int clockwise);

/*
 * Evaluate if there is a winner in a given position and for a given color.
*/
int winner_color(int color, int board[SIZEX][SIZEY]);

int winner(int board[SIZEX][SIZEY]);

/* Eval function will analyze the position and returns if it is favourable towards color
 * First, the function computes whether there is a winner or not.
 * Then, it computes the equivalent integer of the position (it recognizes pattern of 3 marbles)
*/
int eval(int board[SIZEX][SIZEY]);

int compute_board(int board[SIZEX][SIZEY]);

int alpha_beta(int alpha, int beta, int color, int depth, int board[SIZEX][SIZEY], int base, int maxdepth);

#endif // PENTAGO_H_
