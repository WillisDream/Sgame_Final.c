#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
// #define DELAY 180000
// Rajae Willis,Thi Minh Huyen Le , Alondra Torres, Joseph
struct Snake {
  int x, z;
  struct Snake *next;
};

enum Direction { LEFT, RIGHT, UP, DOWN };

// Functions Prototype
void drawScreen();
void initCurses();
void initGame();
void updateDirection(int);
void shiftSnake();
void add_colorpairs();
void drawSnake();
int randomTrophy(int min, int max);
void printTrophy();
void growup();
void speedup();
void checkWin();
// Global variables
bool gameOver;
enum Direction currDir;
struct Snake *head;
char c;    // The trophy number
int randx; // The position x of the trophy
int randz; // The position z of the trophy
int score = 0;
int snakeLength = 5;
int DELAY = 180000;
int main(void) {

  initGame();
  initCurses();

  while (TRUE) {
    if (gameOver) { // Initialize if game is over
      sleep(1);
      initGame();
      score = 0;
      snakeLength = 5;
      DELAY = 180000;
      initGame();
      mvprintw(0, 0, "SCORE: %d", score);
    }
    int keypress = getch();
    updateDirection(keypress);
    shiftSnake();
    drawSnake();
    if (head->x == randx &&
        head->z == randz) // Condition to take snake meet trophy
      growup();
    refresh();
    usleep(DELAY); // Delay between snakes movements.
  }
  endwin(); // terminate ncurses
  return 0;
}
void drawScreen() {
  // Creates Border for snake game
  attron(COLOR_PAIR(8));
  // creates horizontal lines in program
  for (int i = 0; i < COLS; i++) {
    move(1, i);
    addch(ACS_HLINE);
    move(LINES - 1, i);
    addch(ACS_HLINE);
  }
  // creates vertical lines in program
  for (int k = 1; k < LINES; k++) {
    move(k, 0);
    addch(ACS_VLINE);
    move(k, COLS - 1);
    addch(ACS_VLINE);
  }
  // Add special symbols to the corner of border
  move(1, 0);
  addch(ACS_ULCORNER);
  move(LINES - 1, 0);
  addch(ACS_LLCORNER);
  move(1, COLS - 1);
  addch(ACS_URCORNER);
  move(LINES - 1, COLS - 1);
  addch(ACS_LRCORNER);
  attroff(COLOR_PAIR(8));
  printTrophy();
  // Print notice on the screen for player
  mvprintw(0, 0,
           "SCORE: %d       Columns: %d          Lines: %d     Key Q: exit     "
           "'If length of snake is greater than %d, you win'",
           score, COLS - 1, LINES - 2, COLS / 6);
}

void drawSnake() {
  // Creates the snakes body
  // Traverse every node of the snake to get position x and z
  struct Snake *currBody;
  currBody = head;

  while (currBody != NULL) {
    attron(COLOR_PAIR(10));
    mvaddch(currBody->z, currBody->x, 'O');
    attroff(COLOR_PAIR(10));
    currBody = currBody->next;
  }
  // Cursor is placed into correct rectangular position -- render
  move(LINES - 1, 0);
}

void initCurses() {
  initscr();            // ncurses begins
  keypad(stdscr, TRUE); // initiate arrow keys
  timeout(0); // Getchar does not pause and wait for input because non-blocking
              // read is employed.
  add_colorpairs(); // color combinations
}
void add_colorpairs() {
  if (has_colors()) {
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_RED);
    init_pair(2, COLOR_BLACK, COLOR_GREEN);
    init_pair(3, COLOR_BLACK, COLOR_YELLOW);
    init_pair(4, COLOR_BLACK, COLOR_BLUE);
    init_pair(5, COLOR_BLACK, COLOR_CYAN);
    init_pair(6, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);
    init_pair(8, COLOR_YELLOW, COLOR_RED);
    init_pair(9, COLOR_GREEN, COLOR_BLACK);
    init_pair(10, COLOR_YELLOW, COLOR_GREEN);
  }
}

void initGame() {
  clear(); // creates an default screen
  drawScreen();
  currDir = RIGHT;
  gameOver = false;

  // Snake coordinates at the start of the game, in the centre of the screen
  int x = COLS / 2;
  int z = LINES / 2;

  struct Snake *currBody;
  currBody = (struct Snake *)malloc(sizeof(struct Snake));
  currBody->next = NULL;
  currBody->x = x - (snakeLength - 1);
  currBody->z = z;

  for (int i = snakeLength - 2; i >= 0; i--) {
    head = (struct Snake *)malloc(sizeof(struct Snake));
    head->next = currBody;
    head->x = x - i;
    head->z = z;
    currBody = head;
  }
}

void updateDirection(int keypress) {
  switch (keypress) {
  case KEY_UP:
    if (currDir == DOWN) {
      // Program end
      break;
    }
    currDir = UP;
    break;
  case KEY_DOWN:
    if (currDir == UP) {
      // Program end
      break;
    }
    currDir = DOWN;
    break;
  case KEY_LEFT:
    if (currDir == RIGHT) {
      // Program end
      break;
    }
    currDir = LEFT;
    break;
  case KEY_RIGHT:
    if (currDir == LEFT) {
      // Program end
      break;
    }
    currDir = RIGHT;
    break;
  case 'Q': // Q or q will terminate program
  case 'q':
    clear();
    endwin();
    exit(0);
    break;
  }
}

void shiftSnake() {
  // Create a new node and update the position for this node, then  link to the
  // body of the snake.
  struct Snake *newHead;
  newHead = (struct Snake *)malloc(sizeof(struct Snake));
  newHead->next = head;
  newHead->x = head->x;
  newHead->z = head->z;
  head = newHead;

  switch (currDir) {
  case UP:
    head->z--;
    break;
  case DOWN:
    head->z++;
    break;
  case LEFT:
    head->x--;
    break;
  case RIGHT:
    head->x++;
    break;
  }

  // If Snakes head collides with the border, the game is finished.
  if (head->x >= COLS - 2 || head->x <= 1 || head->z >= LINES - 2 ||
      head->z <= 1) {
    gameOver = true;
  }

  struct Snake *currBody;
  currBody = head;
  while (currBody->next->next != NULL) {
    // If Snakes head collides with the body, the game is finished.
    if (head->x == currBody->next->x && head->z == currBody->next->z) {
      gameOver = true;
    }
    currBody = currBody->next;
  }
  mvaddch(currBody->next->z, currBody->next->x, ' ');
  free(currBody->next);
  currBody->next = NULL;
}
void growup() {
  // Create the randnumber ( trophy number) of node, set position for each node
  // and link to the body of snake
  int randnumber = c - 48;
  struct Snake *currBody;
  snakeLength += randnumber; // Increasing snakeLength
  currBody = head;
  while (currBody->next != NULL) {
    currBody = currBody->next;
  }
  while (randnumber > 0) {
    int dir = 1;
    if (currDir == LEFT)
      dir = -1;
    if (currDir == RIGHT)
      dir = 1;
    struct Snake *newTail;
    newTail = (struct Snake *)malloc(sizeof(struct Snake));
    newTail->next = NULL;
    newTail->x = (currBody->x) - dir;
    newTail->z = currBody->z;
    currBody->next = newTail;
    currBody = newTail;
    randnumber--;
  }
  // Prints new trophy if snake touches current trophy, adds trophy to score
  score = score + (c - 48);
  mvprintw(0, 0, "SCORE: %d", score);
  printTrophy();
  speedup();
  checkWin();
}
int randomNumber(int min, int max) {
  time_t t;
  /* Intializes random number generator */
  srand((unsigned)time(&t));
  return min + (rand() % (max - 1)); // generate random number from min to max
}
void printTrophy() {
  // Initialize the value for global variable : c, randz, randx
  c = randomNumber(1, 10) + '0';      // A trophy number as a char
  randx = randomNumber(1, COLS - 2);  // Position x for the trophy
  randz = randomNumber(2, LINES - 2); // Position z for the trophy
  mvaddch(randz, randx, c);           // Add the trophy to the screen
}
void speedup() {
  // Speed goes up for each time that snake meet trophy with a rate.
  int rate = snakeLength / 10 + 1;
  DELAY = DELAY - rate * 10000;
}
void checkWin() {
  // COndition to win is that snakeLength is greate than COLS/6.
  if (score > COLS/6) {
    mvprintw(LINES / 2, COLS / 2, "YOU WIN!");
    refresh();
    sleep(5);
    gameOver = TRUE;
  }
}
