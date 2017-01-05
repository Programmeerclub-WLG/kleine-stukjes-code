#include <stdio.h>
#include <string.h>

/******************************************************************************/
/* 				Macro's																															*/
/******************************************************************************/

/* Debug variable */
#define DEBUG

/* Io bound macro's */
#define output(format, ...) printf(format, ##__VA_ARGS__); \
 fflush(stdout)
#define logmsg(format, ...) fprintf(stderr, format, ##__VA_ARGS__); \
 fflush(stderr)

#define is_nill(buffer) strcmp(buffer, "NIL") == 0
#define is_quit(buffer) strcmp(buffer, "Quit") == 0

/* Init constants */
#define BOARD_SIZE 6

/* In-game constants */
#define EMPTY 0
#define WHITE 1
#define BLACK 2

#define NO_WALL_COST 0
#define SINGLE_WALL_COST 1
#define DOUBLE_WALL_COST 2

#define in_field(x, y) ((x) >= 0 && (x) <= 5 && (y) >= 0 && (y) <= 5)
#define free(x, y) board[x][y] == EMPTY

/* Wall thickness macro's */
#define horizontal_cost(x, y) horizontal_movement[y][x]
#define vertical_cost(x, y) vertical_movement[y][x]

/******************************************************************************/
/*				Global state 																												*/
/******************************************************************************/

/* Global variables with game state */
/* All are [x][y] indexed */
unsigned char board[BOARD_SIZE][BOARD_SIZE];
unsigned char vertical_movement[BOARD_SIZE-1][BOARD_SIZE];
unsigned char horizontal_movement[BOARD_SIZE][BOARD_SIZE-1];

unsigned char MY_COLOR;
unsigned char TILE_1[2];
unsigned char TILE_2[2];
unsigned char TILE_3[2];
unsigned char TILE_4[2];
unsigned char TILES[4][2];

/******************************************************************************/
/*				IO functions 																												*/
/******************************************************************************/

/* Write a move to stdout */
void write_move(char a, char b, char x, char y) {
	char buffer[5];
	// NOTE: Coords are printed (y, x)
	buffer[0] = b + 'a';
	buffer[1] = a + '1';
	buffer[2] = y + 'a';
	buffer[3] = x + '1';
	buffer[4] = '\0';		// Default string is a pointer
	output("%s\n", buffer);
}


#ifdef DEBUG

void print_board() {
	logmsg("%d | %d | %d | %d | %d | %d\n", board[0][0], board[1][0], \
	board[2][0], board[3][0], board[4][0], board[5][0]);
	logmsg("---------------------\n");
	logmsg("%d | %d | %d | %d | %d | %d\n", board[0][1], board[1][1], \
	board[2][1], board[3][1], board[4][1], board[5][1]);
	logmsg("---------------------\n");
	logmsg("%d | %d | %d | %d | %d | %d\n", board[0][2], board[1][2], \
	board[2][2], board[3][2], board[4][2], board[5][1]);
	logmsg("---------------------\n");
	logmsg("%d | %d | %d | %d | %d | %d\n", board[0][3], board[1][3], \
	board[2][3], board[3][3], board[4][3], board[5][3]);
	logmsg("---------------------\n");
	logmsg("%d | %d | %d | %d | %d | %d\n", board[0][4], board[1][4], \
	board[2][4], board[3][4], board[4][4], board[5][4]);
	logmsg("---------------------\n");
	logmsg("%d | %d | %d | %d | %d | %d\n", board[0][5], board[1][5], \
	board[2][5], board[3][5], board[4][5], board[5][5]);
}

void print_walls() {
	char i = 0;
	for (char c = 0; c < 5; c++) { 		// Haha C++
		for (char n = 0; n < 5; n++) {
			logmsg("%d", horizontal_movement[c][n]);
			i++;
		}
		logmsg("\n");
		for (char n = 0; n < 6; n++) {
			logmsg("%d", vertical_movement[c][n]);
			i++;
		}
		logmsg("\n");
	}
	// Last row
	for (char n = 0; n < 5; n++) {
		logmsg("%d", horizontal_movement[5][n]);
		i++;
	}
	logmsg("\n");
}

#endif

/******************************************************************************/
/*				Functions for board interaction 																		*/
/******************************************************************************/

/* Initialize the board */
void init_board() {
	for (unsigned char n = 0; n < BOARD_SIZE; n++) {
		for (unsigned char m = 0; m < BOARD_SIZE; m++) {
			board[n][m] = EMPTY;
		}
	}
	board[0][0] = BLACK;
	board[0][1] = BLACK;
	board[1][0] = BLACK;
	board[1][1] = BLACK;
	board[5][5] = WHITE;
	board[4][5] = WHITE;
	board[5][4] = WHITE;
	board[4][4] = WHITE;
}

/* Read the 60 chars from stdin that represent the walls and initialize
the walls */
void init_walls() {
	char buffer[60];
	scanf("%s", buffer);
	char i = 0;
	for (char c = 0; c < 5; c++) { 		// Haha C++
		// | | | | |
		for (char n = 0; n < 5; n++) {
			horizontal_movement[c][n] = buffer[i] - '0';
			i++;
		}
		for (char n = 0; n < 6; n++) {
			vertical_movement[c][n] = buffer[i] - '0';
			i++;
		}
	}
	// Last row
	for (char n = 0; n < 5; n++) {
		horizontal_movement[5][n] = buffer[i] - '0';
		i++;
	}
}

/* Compute the cost of moving from one square
to an adjacent square. */
unsigned char cost(char a, char b, char x, char y) {
	if (a < x) {
		return horizontal_cost(a, b);
	} else if (b < y) {
		return vertical_cost(a, b);
	} else if (a > x) {
		return horizontal_cost(x, y);
	} else {
		return vertical_cost(x, y);
	}
}

unsigned char can_move_double(char a, char b, char x, char y) {
  if (a < x) {
		return horizontal_cost(a, b) + horizontal_cost(a+1, b) == 0;
	} else if (b < y) {
		return vertical_cost(a, b) + vertical_cost(a, b+1) == 0;
	} else if (a > x) {
		return horizontal_cost(x, y) + horizontal_cost(a-1, b) == 0;
	} else {
		return vertical_cost(x, y) + vertical_cost(a, b-1) == 0;
	}
}

/* Move a tile from (a, b) to (x, y). Color is either black or white */
void move(char a, char b, char x, char y, char color) {
	board[a][b] = EMPTY;
	board[x][y] = color;
	if (color == MY_COLOR) {
		write_move(a, b, x, y);
    for (int i = 0; i < 4; i++) {
      if (TILES[i][0] == a && TILES[i][1] == b) {
        TILES[i][0] = x;
        TILES[i][1] = y;
        break;
      }
    }
	}
}

/******************************************************************************/
/* 				White protocol 																											*/
/******************************************************************************/

void white_move() {
  // First: Try to move as far as possible for free
  for (int i = 3; i > -1; i--) {
    unsigned char x = TILES[i][0];
    unsigned char y = TILES[i][1];
    if (in_field(x-2, y) && (!free(x-1, y)) && free(x-2, y) && can_move_double(x, y, x-2, y)) {
      move(x, y, x-2, y, WHITE);
      return;
    } else if (in_field(x, y-2) && (!free(x, y-1)) && free(x, y-2) && can_move_double(x, y, x, y-2)) {
      move(x, y, x, y-2, WHITE);
      return;
    }
  }
  for (int i = 3; i > -1; i++) {
    unsigned char x = TILES[i][0];
    unsigned char y = TILES[i][1];
    if (in_field(x-1, y) && free(x-1, y) && cost(x, y, x-1, y) == 0) {
      move(x, y, x-1, y, WHITE);
      return;
    } else if (in_field(x, y-1) && free(x, y-1) && cost(x, y, x, y-1) == 0) {
      move(x, y, x, y-1, WHITE);
      return;
    }
  }
  // As free as possible
  for (int i = 3; i > -1; i--) {
    unsigned char x = TILES[i][0];
    unsigned char y = TILES[i][1];
    if (in_field(x-1, y) && free(x-1, y) && cost(x, y, x-1, y) == 1) {
      move(x, y, x-1, y, WHITE);
      return;
    } else if (in_field(x, y-1) && free(x, y-1) && cost(x, y, x, y-1) == 1) {
      move(x, y, x, y-1, WHITE);
      return;
    }
  }
  // Well just do a random move since there's no other choice
  for (int i = 3; i > -1; i--) {
    unsigned char x = TILES[i][0];
    unsigned char y = TILES[i][1];
    if (in_field(x-1, y) && free(x-1, y)) {
      move(x, y, x-1, y, WHITE);
      return;
    } else if (in_field(x, y-1) && free(x, y-1)) {
      move(x, y, x, y-1, WHITE);
      return;
    }
  }
  // Allright, it turns out we cannot move any further to the left
  // or up. Calculate the best move in another direction.
  // Basicly, this is special code to make sure all tiles
  // can be placed properly in the corner
  for (int i = 3; i > -1; i--) {
    unsigned char x = TILES[i][0];
    unsigned char y = TILES[i][1];
    if (x == 0 && y == 0) {   // Perfect; Don't move
      continue;
    } else if (x == 1 && y == 0) {
      if (free(0, 0)) {
        move(1, 0, 0, 0, WHITE);
      }
    } else if (x == 0 && y == 1) {
      if (free(0, 0)) {
        move(0, 1, 0, 0, WHITE);
      }
    } else if (x == 0 && free(x+1, y)) {
      move(x, y, x-1, y, WHITE);
    } else if (y == 0 && free(x, y+1)) {
      move(x, y, x, y-1, WHITE);
    }
  }
}

/* Play as white */
void play_white() {
	MY_COLOR = WHITE;
  TILES[0][0] = 4; TILES[0][1] = 5;
  TILES[1][0] = 4; TILES[1][1] = 4;
  TILES[2][0] = 5; TILES[2][1] = 4;
  TILES[3][0] = 5; TILES[3][1] = 5;
	// perform first move
	white_move();

	#ifdef DEBUG
	print_board();
	#endif

	for (;;) {
		// Black move
		char buffer[4];
		static char coords[4];
		scanf("%s", buffer);
		if (!(is_nill(buffer))) {
      if (is_quit(buffer)) {
        return;
      }
			// NOTE: Input is (y, x)
			coords[0] = buffer[0] - 'a';
			coords[1] = buffer[1] - '1';
			coords[2] = buffer[2] - 'a';
			coords[3] = buffer[3] - '1';
			//logmsg("%d %d %d %d\n", coords[0], coords[1], coords[2], coords[3]);
			move(coords[1], coords[0], coords[3], coords[2], BLACK);
		}
		// TODO: Implement white move calculation
    white_move();

		#ifdef DEBUG
		print_board();
		#endif
	}
}

/******************************************************************************/
/*				Black Protocol 																											*/
/******************************************************************************/

void black_move() {
  // First: Try to move as far as possible for free
  for (int i = 3; i > -1; i--) {
    unsigned char x = TILES[i][0];
    unsigned char y = TILES[i][1];
    if (in_field(x+2, y) && (!free(x+1, y)) && free(x+2, y) && can_move_double(x, y, x+2, y)) {
      move(x, y, x+2, y, WHITE);
      return;
    } else if (in_field(x, y+2) && (!free(x, y+1)) && free(x, y+2) && can_move_double(x, y, x, y+2)) {
      move(x, y, x, y+2, WHITE);
      return;
    }
  }
  for (int i = 3; i > -1; i++) {
    unsigned char x = TILES[i][0];
    unsigned char y = TILES[i][1];
    if (in_field(x+1, y) && free(x+1, y) && cost(x, y, x+1, y) == 0) {
      move(x, y, x+1, y, WHITE);
      return;
    } else if (in_field(x, y+1) && free(x, y+1) && cost(x, y, x, y+1) == 0) {
      move(x, y, x, y+1, WHITE);
      return;
    }
  }
  // As free as possible
  for (int i = 3; i > -1; i--) {
    unsigned char x = TILES[i][0];
    unsigned char y = TILES[i][1];
    if (in_field(x+1, y) && free(x+1, y) && cost(x, y, x+1, y) == 1) {
      move(x, y, x+1, y, WHITE);
      return;
    } else if (in_field(x, y+1) && free(x, y+1) && cost(x, y, x, y+1) == 1) {
      move(x, y, x, y+1, WHITE);
      return;
    }
  }
  // Well just do a random move since there's no other choice
  for (int i = 3; i > -1; i--) {
    unsigned char x = TILES[i][0];
    unsigned char y = TILES[i][1];
    if (in_field(x+1, y) && free(x+1, y)) {
      move(x, y, x+1, y, WHITE);
      return;
    } else if (in_field(x, y+1) && free(x, y+1)) {
      move(x, y, x, y+1, WHITE);
      return;
    }
  }
  // Basicly, this is special code to make sure all tiles
  // can be placed properly in the corner
  for (int i = 3; i > -1; i--) {
    unsigned char x = TILES[i][0];
    unsigned char y = TILES[i][1];
    if (x == 5 && y == 5) {   // Perfect; Don't move
      continue;
    } else if (x == 4 && y == 5) {
      if (free(5, 5)) {
        move(4, 5, 5, 5, BLACK);
      }
    } else if (x == 5 && y == 4) {
      if (free(5, 5)) {
        move(5, 4, 5, 5, BLACK);
      }
    } else if (x == 5 && free(x-1, y)) {
      move(x, y, x-1, y, BLACK);
    } else if (y == 5 && free(x, y-1)) {
      move(x, y, x, y-1, BLACK);
    }
  }
}

void play_black(char *first_move) {
	MY_COLOR = BLACK;
  TILES[0][0] = 0; TILES[0][1] = 1;
  TILES[1][0] = 1; TILES[1][1] = 1;
  TILES[2][0] = 1; TILES[2][1] = 0;
  TILES[3][0] = 0; TILES[3][1] = 0;
	// Write white's first move
	unsigned char coords[4];
	if (!(is_nill(first_move))) {
    if (is_quit(first_move)) {
      return;
    }
		coords[0] = first_move[0] - 'a';
		coords[1] = first_move[1] - '1';
		coords[2] = first_move[2] - 'a';
		coords[3] = first_move[3] - '1';
		move(coords[1], coords[0], coords[3], coords[2], WHITE);
	}
	// perform first move
	black_move();

	#ifdef DEBUG
	print_board();
	#endif

	for (;;) {
		// White move
		char buffer[4];
		static char coords[4];
		scanf("%s", buffer);
		if (!(is_nill(buffer))) {
      if (is_quit(buffer)) {
        return;
      }
			// NOTE: Input is (y, x)
			coords[0] = buffer[0] - 'a';
			coords[1] = buffer[1] - '1';
			coords[2] = buffer[2] - 'a';
			coords[3] = buffer[3] - '1';
			move(coords[1], coords[0], coords[3], coords[2], WHITE);
		}
    black_move();

		#ifdef DEBUG
		print_board();
		#endif
	}
}

/******************************************************************************/
/*				Main 																																*/
/******************************************************************************/

int main(void) {
	init_board();
	init_walls();

	#ifdef DEBUG
	print_walls();
	print_board();
	#endif

	char buffer[6];
	scanf("%s", buffer);
	if (strcmp(buffer, "Start") == 0) {
		play_white();
	} else {
		play_black(buffer);
	}
  return 0;
}
