/*
 * Compile this project with:
 * gcc -W -Wall -std=c99 -O3 -o pentago_ia pentago.c
*/

#include "pentago.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "time.h"

int pattern[10][3] = {
  {1,1,1},
  {1,1,0},
  {1,1,-1},
  {1,0,1},
  {0,1,0},
  {-1,1,0},
  {1,0,0},
  {1,0,-1},
  {1,-1,1},
  {0,0,0}
};

int points[10] = {
  100,
  80,
  70,
  45,
  40,
  30,
  20,
  10,
  0,
  0
};

int duo_pattern[4][2] = {
  {1,1},
  {1,0},
  {1,-1},
  {0,0}
};

int duo_points[4]={
  70,
  45,
  5,
  0
};

/* Operation parameter precises if the compute function has to add or substract the score to ev */
int index_pattern(int *operation, int array[SIZEX/2]){
  int nb;
  /* First case, the array is simply in the pattern array */
  for (int i = 0; i < 10;i++){
    nb = 0;
    for (int j=0;j<SIZEX/2;j++){
      if (array[j] != pattern[i][j]) break;
      else nb++;
    }
    if (nb >= 3){
      *operation = 1;
      return i;
    }
    /* Checking the symmetry */
    nb = 0;
    reverse_array(array, SIZEX/2);
    for (int j=0;j<SIZEX/2;j++){
      if (array[j] != pattern[i][j]) break;
      else nb++;
    }
    reverse_array(array, SIZEX/2);
    if (nb >= 3){
      *operation = 1;
      return i;
    }

    /* Checking the color symmetry (same pattern but with the colors exchanged) */
    nb = 0;
    for (int j=0;j<SIZEX/2;j++){
      if (array[j] != -pattern[i][j]) break;
      else nb++;
    }
    if (nb >= 3){
      *operation = -1;
      return i;
    }

    nb = 0;
    /* Colors exchanged and array reversed */
    reverse_array(array, SIZEX/2);
    for (int j=0;j<SIZEX/2;j++){
      if (array[j] != -pattern[i][j]) break;
      else nb++;
    }
    reverse_array(array, SIZEX/2);
    if (nb >= 3){
      *operation = -1;
      return i;
    }
  }
  return -1;
}

int duo_index_pattern(int *operation, int array[2]){
  int nb;

  /* First case, the array is simply in the duo_pattern array */
  for (int i=0;i<4;i++){
    nb = 0;
    for (int j=0;j<2;j++){
      if (array[j] != duo_pattern[i][j]) break;
      else nb++;
    }
    if (nb >= 2){
      *operation = 1;
      return i;
    }
    /* Checking the symmetry */
    nb = 0;
    reverse_array(array,2);
    for (int j=0;j<2;j++){
      if (array[j] != duo_pattern[i][j]) break;
      else nb++;
    }
    reverse_array(array,2);
    if (nb >= 2){
      *operation = 1;
      return i;
    }

    /* Checking the color symmetry (same pattern but with the colors exchanged) */
    nb = 0;
    for (int j=0;j<2;j++){
      if (array[j] != -duo_pattern[i][j]) break;
      else nb++;
    }
    if (nb >= 2){
      *operation = -1;
      return i;
    }

    nb = 0;
    /* Colors exchanged and array reversed */
    reverse_array(array, 2);
    for (int j=0;j<2;j++){
      if (array[j] != -duo_pattern[i][j]) break;
      else nb++;
    }
    reverse_array(array, 2);
    if (nb >= 2){
      *operation = -1;
      return i;
    }
  }
  return -1;
}

void reverse_array(int array[], int length){
  int tmp[length];
  for (int i=0; i<length;i++){
    tmp[i] = array[i];
  }

  for (int i=0; i<length;i++){
    array[i] = tmp[length-i-1];
  }
}


void print_board(int board[SIZEX][SIZEY]){
  for (int i=0;i<SIZEX;i++){
    for (int j=0;j<SIZEY;j++){
      switch (board[i][j]) {
        case WHITE: printf("X ");break;
        case BLACK: printf("O ");break;
        case EMPTY: printf(". ");break;
      }
    }
    printf("%d - x\n",i);
  }
  for (int i=0;i<SIZEX;i++){
    printf("%d ",i);
  }
  printf(" - y\n");
}


void transpose(int matrix[SIZEX/2][SIZEY/2]){
  int transpose_matrix[SIZEX/2][SIZEY/2];

  for (int i=0;i<SIZEX/2;i++){
    for (int j=0;j<SIZEY/2;j++){
      transpose_matrix[i][j] = matrix[j][i];
    }
  }

  for (int i=0;i<SIZEX/2;i++){
    for (int j=0;j<SIZEY/2;j++){
      matrix[i][j] = transpose_matrix[i][j];
    }
  }
}


void rotate(int sub_board[SIZEX/2][SIZEY/2], int clockwise){
  /* Depending on clockwise, we have to apply a vertical or horizontal symmetry */
  int temporary[SIZEX/2][SIZEY/2] = {0};
  switch (clockwise) {
    case 1:
      /* Horizontal symmetry */
      for (int i=0;i<SIZEX/2;i++){
        for (int j=0;j<SIZEY/2;j++){
          temporary[i][j] = sub_board[SIZEX/2 - 1 - i][j];
        }
      }
      break;

    case -1:
      /* Vertical symmetry */
      for (int i=0;i<SIZEX/2;i++){
        for (int j=0;j<SIZEY/2;j++){
          temporary[i][j] = sub_board[i][SIZEY/2 - 1 - j];
        }
      }
      break;

    default:
      printf("Error ! The direction given is not recognized...");
  }

  for (int i=0;i<SIZEX/2;i++){
    for (int j=0;j<SIZEY/2;j++){
      sub_board[i][j] = temporary[i][j];
    }
  }

  /* Then, we transpose the matrix */
  transpose(sub_board);
}


void play(int x, int y, int color, int submatrix, int clockwise, int board[SIZEX][SIZEY]){
  /* We play a marble */
  board[x][y] = color;

  /* Now, we must rotate the chosen submatrix by 90 degrees */
  int sub_board[SIZEX/2][SIZEY/2];
  for (int i=0;i<SIZEX/2;i++){
    for (int j=0;j<SIZEY/2;j++){
      sub_board[i][j] = board[i + (SIZEX/2)*(submatrix/2)][j + (SIZEY/2)*(submatrix%2)];
    }
  }

  rotate(sub_board, clockwise);

  for (int i=0;i<SIZEX/2;i++){
    for (int j=0;j<SIZEY/2;j++){
      board[i + (SIZEX/2)*(submatrix/2)][j + (SIZEY/2)*(submatrix%2)] = sub_board[i][j];
    }
  }
}

void unplay(int x, int y, int submatrix, int clockwise, int board[SIZEX][SIZEY]){
  /* We unrotate the submatrix chosen */
  int sub_board[SIZEX/2][SIZEY/2];
  for (int i=0;i<SIZEX/2;i++){
    for (int j=0;j<SIZEY/2;j++){
      sub_board[i][j] = board[i + (SIZEX/2)*(submatrix/2)][j + (SIZEY/2)*(submatrix%2)];
    }
  }

  rotate(sub_board, -clockwise);

  for (int i=0;i<SIZEX/2;i++){
    for (int j=0;j<SIZEY/2;j++){
      board[i + (SIZEX/2)*(submatrix/2)][j + (SIZEY/2)*(submatrix%2)] = sub_board[i][j];
    }
  }

  /* We unplay the marble */
  board[x][y] = EMPTY;
}

int winner_color(int color, int board[SIZEX][SIZEY]){
  int nb;
  int start = 2;

  /* First, we check if there is a 5 marbles line on x coordinate */
  for (int x = 0; x < SIZEX; x++){
    nb = 0;
    for (int y = start; y >= 0;y--){
      if (board[x][y] == color) nb++; else break;
    }
    for (int y=start+1; y < SIZEX;y++){
      if (board[x][y] == color) nb++; else break;
    }

    if (nb >= 5) return color*WIN;
  }

  /* Then, we check if there is a 5 marbles line on y coordinate */
  for (int y = 0; y < SIZEY; y++){
    nb = 0;
    for (int x = start;x >= 0; x--){
      if (board[x][y] == color) nb++; else break;
    }

    for (int x = start+1; x<SIZEX;x++){
      if (board[x][y] == color) nb++; else break;
    }

    if (nb >= 5) return color*WIN;
  }

  /* After, we check for the diagonals */
  /* The first one (and the ones over and below) */
  for (int i=-1;i<2;i++){
    nb = 0;
    for (int d = start; d >= 0; d--){
      if ((d == 0) && (i == -1)){}
      else {
        if (board[d][d+i] == color) nb++; else break;
      }
    }

    for (int d = start + 1; d < SIZEX; d++){
      if ((d == 5) && (i == 1)){}
      else {
        if (board[d][d+i] == color) nb++; else break;
      }
    }
    if (nb >= 5) return color*WIN;
  }

  /* The second diagonal (and the ones over and below) */
  for (int i=-1;i<2;i++){
    nb = 0;
    for (int d = start; d >= 0; d--){
      if ((d == 0) && (i == 1)){}
      else {
        if (board[d][SIZEX - 1 - d + i] == color) nb++; else break;
      }
    }

    for (int d = start + 1; d < SIZEX; d++){
      if ((d == 5) && (i == -1)){}
      else {
        if (board[d][SIZEX - 1 - d + i] == color) nb++; else break;
      }
    }
    if (nb >= 5) return color*WIN;
  }


  return 0;
}

int winner(int board[SIZEX][SIZEY]){
  int gagnant;
  gagnant = winner_color(WHITE,board);
  if (gagnant != 0){
    return gagnant;
  }
  gagnant = winner_color(BLACK,board);
  return gagnant;
}

int compute_board(int board[SIZEX][SIZEY]){
  int ev = 0;
  int operation;
  int index;

  /* for sur les sous-plateaux */
  for (int sub = 0; sub < 4;sub++){
    /* Checking the lines */
    for (int i=0;i<SIZEX/2;i++){
      int row[SIZEX/2] = {0};
      for (int j=0;j<SIZEY/2;j++){
        row[j] = board[i + (SIZEX/2)*(sub/2)][j + (SIZEY/2)*(sub%2)];
      }
      /* On trouve l'index du pattern */
      index = index_pattern(&operation,row);

      /* On ajoute ou soustrait les points à ev */
      if (index != -1){
        if (operation == 1)
          ev += points[index];
        else if (operation == -1)
          ev -= points[index];
      } else
        printf("Error, index not found...");
    }

    /* Checking the columns */
    for (int j=0;j<SIZEX/2;j++){
      int column[SIZEX/2] = {0};
      for (int i=0;i<SIZEY/2;i++){
        column[i] = board[i + (SIZEX/2)*(sub/2)][j + (SIZEY/2)*(sub%2)];
      }
      /* On trouve l'index du pattern */
      index = index_pattern(&operation,column);

      /* On ajoute ou soustrait les points à ev */
      if (index != -1){
        if (operation == 1)
          ev += points[index];
        else if (operation == -1)
          ev -= points[index];
      }
      else
        printf("Error, index not found...");
    }

    /* Checking the diagonals */
    int diagonal[SIZEX/2] = {0};
    int duo_diagonal[2] = {0};

    /* The first one */
    for (int d=0;d<SIZEX/2;d++){
      diagonal[d] = board[d + (SIZEX/2)*(sub/2)][d + (SIZEY/2)*(sub%2)];
    }
    /* On trouve l'index du pattern */
    index = index_pattern(&operation,diagonal);

    /* On ajoute ou soustrait les points à ev */
    if (index != -1){
      if (operation == 1)
        ev += points[index];
      else if (operation == -1)
        ev -= points[index];
    } else
      printf("Error, index not found...");

    /* The above one */
    for (int d=1;d<SIZEX/2;d++){
      duo_diagonal[d-1] = board[d-1 + (SIZEX/2)*(sub/2)][d + (SIZEY/2)*(sub%2)];
    }
    /* On trouve l'index du pattern */
    index = duo_index_pattern(&operation,duo_diagonal);

    /* On ajoute ou soustrait les points à ev */
    if (index != -1){
      if (operation == 1)
        ev += duo_points[index];
      else if (operation == -1)
        ev -= duo_points[index];
    } else
      printf("Error, index not found...");

    /* The below one */
    for (int d=1;d<SIZEX/2;d++){
      duo_diagonal[d-1] = board[d + (SIZEX/2)*(sub/2)][d-1 + (SIZEY/2)*(sub%2)];
    }
    /* On trouve l'index du pattern */
    index = duo_index_pattern(&operation,duo_diagonal);

    /* On ajoute ou soustrait les points à ev */
    if (index != -1){
      if (operation == 1)
        ev += duo_points[index];
      else if (operation == -1)
        ev -= duo_points[index];
    } else
      printf("Error, index not found...");

    /* The second one */
    for (int d=0;d<SIZEX/2;d++){
      diagonal[d] = board[d + (SIZEX/2)*(sub/2)][SIZEX/2 - d - 1 + (SIZEY/2)*(sub%2)];
    }
    /* On trouve l'index du pattern */
    index = index_pattern(&operation,diagonal);

    /* On ajoute ou soustrait les points à ev */
    if (index != -1){
      if (operation == 1)
        ev += points[index];
      else if (operation == -1)
        ev -= points[index];
    } else
      printf("Error, index not found...");

    /* The above one */
    for (int d=1;d<SIZEX/2;d++){
      duo_diagonal[d-1] = board[d-1 + (SIZEX/2)*(sub/2)][d + (SIZEY/2)*(sub%2)];
    }
    /* On trouve l'index du pattern */
    index = duo_index_pattern(&operation,duo_diagonal);

    /* On ajoute ou soustrait les points à ev */
    if (index != -1){
      if (operation == 1)
        ev += duo_points[index];
      else if (operation == -1)
        ev -= duo_points[index];
    } else
      printf("Error, index not found...");

    /* The below one */
    for (int d=1;d<SIZEX/2;d++){
      duo_diagonal[d-1] = board[d + (SIZEX/2)*(sub/2)][d-1 + (SIZEY/2)*(sub%2)];
    }
    /* On trouve l'index du pattern */
    index = duo_index_pattern(&operation,duo_diagonal);

    /* On ajoute ou soustrait les points à ev */
    if (index != -1){
      if (operation == 1)
        ev += duo_points[index];
      else if (operation == -1)
        ev -= duo_points[index];
    } else
      printf("Error, index not found...");
  }
  return ev;
}


int eval(int board[SIZEX][SIZEY]){
  /* We compute the equivalent integer of the position */
  int ev = compute_board(board);
  return ev;
}


int best_move[4]; /* best_move format= {x, y, sub_board, direction_of_rotation} */
int alpha_beta(int alpha, int beta, int color, int depth, int board[SIZEX][SIZEY], int base, int maxdepth){
  int v = winner(board);
  if (abs(v) == WIN){
    if (v>0) return v-depth; /* We return v-depth, so the computer goes right to the shortest way to win */
    else return v+depth; /* Same at the difference, this time it wants to drag on its lost */
  }

  if (depth == maxdepth) {
    int v = eval(board);
    return v;
  }
  if (depth == SIZEX*SIZEY - 1) return 0;

  int a = alpha;
  int b = beta;
  int g;
  if (color==WHITE) g=-32767; else g=32767;

  for (int x=0;x<SIZEX && a < b;x++) {
    for (int y=0;y<SIZEX && a < b;y++) {
      for (int sub =0;sub < 4 && a < b;sub++){
        for (int direction = -1; direction <= 1 && a<b; direction += 2){
          if (board[x][y] == 0){
            play(x,y,color,sub, direction, board);
            v = alpha_beta(a,b,-color,depth+1, board, base, maxdepth);
            unplay(x,y,sub,direction,board);

            if (color==WHITE) {
              if ((depth == base) && (v > g)){
                /* Need to change what to return (x, y, sub, direction) */
                best_move[0] = x;
                best_move[1] = y;
                best_move[2] = sub;
                best_move[3] = direction;
              }
              g=max(g,v);
              a=max(a,g);
            }
            else {
              if ((depth == base) && (v < g)){
                best_move[0] = x;
                best_move[1] = y;
                best_move[2] = sub;
                best_move[3] = direction;
              }
              g=min(g,v);
              b=min(b,g);
            }
          }
        }
      }
    }
  }
  return g;
}



int main() {
  int board[SIZEX][SIZEY] = {0};

  /* play(1,1,BLACK,1,1,board); */
  /* play(0,1,WHITE,3,1,board); */
  /* play(4,0,WHITE,3,1,board); */
  /* play(3,5,WHITE,3,1,board); */
  /* play(4,4,WHITE,3,1,board); */
  /* print_board(board); */
  /* printf("%d", compute_board(board)); */

  int user_move[4] = {0};
  int ret,maxdepth,base;
  clock_t time,otime;
  double ftime;

  base=0;
  while (1) {
    maxdepth=base;
    otime=clock();
    do {
      maxdepth++;

      best_move[0] = -1;
      best_move[1] = -1;
      best_move[2] = -1;
      best_move[3] = -1;

      ret=alpha_beta(-MAXV,MAXV,WHITE,base,board,base,maxdepth);
      time=clock()-otime;
      ftime=(double)time/(double)CLOCKS_PER_SEC;
      printf("best_move=%d %d sub: %d rotation: %d ret=%d base=%d maxdepth=%d time=%f\n",
	    best_move[0],best_move[1],best_move[2], best_move[3], ret,base,maxdepth,ftime);

      if (best_move[0] == -1) {printf("error -1\n");exit(EXIT_ERROR);}
    } while ((abs(ret)<BWIN)&&(ftime<=1.0)&&(maxdepth<(SIZEX*SIZEY-1)));

    play(best_move[0], best_move[1], WHITE, best_move[2], best_move[3], board);
    ret=winner(board);
    printf("My move:%d %d  sub: %d rotation: %d\n",best_move[0], best_move[1], best_move[2], best_move[3]);
    print_board(board);
    base++;
    if (abs(ret)>=BWIN) break;

    while (1) {
      printf("Your move (format: 'x y'):");
      scanf("%d %d", &user_move[0], &user_move[1]);
      if ((user_move[0]>=0)&&(user_move[0]<SIZEX) && (user_move[1]>=0)&&(user_move[1]<SIZEY) && board[user_move[0]][user_move[1]]==0) {
        printf("Your rotation (format: 'sub-board direction'):");
        scanf("%d %d", &user_move[2], &user_move[3]);

        if ((user_move[2] >= 0) && (user_move[2] < 4) && ((user_move[3] == -1) || (user_move[3] == 1))){
          play(user_move[0], user_move[1], BLACK, user_move[2], user_move[3], board);
          ret=winner(board);
          base++;
          break;
        } else
          printf("Invalid move, please try again !\n");
      } else
        printf("Invalid move, please try again !\n");

    }
    print_board(board);
    if (abs(ret)>=BWIN) break;
  }
  return 0;
}
