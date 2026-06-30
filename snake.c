#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct Cell {
  int x;
  int y;
  struct Cell *next;
  struct Cell *prev;
  struct Cell *end;
} Cell;

int show_snake(struct Cell *cell, int disp);
int add_cell(struct Cell *cell, int dx, int dy);
int move_snake(struct Cell *cell, int key);
int is_moving_key(int key);
int check_food(struct Cell *cell, struct Cell *food, int lastkey);
int exists_cell(int x, int y, struct Cell *cells);
int add_food(struct Cell *pfood, struct Cell *snake_head);
int check_boundaries(struct Cell *cell, int key);
int check_snake_body(struct Cell *cell);

int main(int argn, char **argv) {

  initscr();
  keypad(stdscr, TRUE);
  clear();
  refresh();
  timeout(16);
  nodelay(stdscr, TRUE);

  noecho();

  // char key = 0, lastkey = 0;
  int x = 20, y = 20, key, lastkey, dx = 0, dy = 0;

  //Cell head = {x, y, NULL, NULL, NULL};
  Cell *head = malloc(sizeof(Cell));
  head->x = x;
  head->y = y;
  head->prev = head->next = NULL;
  head->end = head;

  //  Cell food = {23, 20, NULL, NULL};
  Cell *food = malloc(sizeof(Cell));
  food->x = 23;
  food->y = 20;
  food->prev = NULL;
  food->next = NULL;
  food->end = NULL;

  while (key != 'q') {
    int status = 0;
    clear();

    key = getch();
    flushinp();

    if (key == ERR) {
      key = lastkey;
      move(6, 0);
      //printw("key is ERR. lastkey is %c\n", lastkey);
    }
    switch (key) {
      case KEY_RIGHT:
      case 'l':
        if (lastkey == 'h' || lastkey == KEY_LEFT) {
          key = 'h';
          break;
        }
        ++x;
        lastkey = 'l';
        break;
      case KEY_LEFT:
      case 'h':
        if (lastkey == 'l' || lastkey == KEY_RIGHT) {
          key = 'l';
          break;
        }
        --x;
        lastkey = 'h';
        break;
      case KEY_UP:
      case 'k':
        if (lastkey == 'j' || lastkey == KEY_DOWN) {
          key = 'j';
          break;
        }
        --y;
        lastkey = 'k';
        break;
      case KEY_DOWN:
      case 'j':
        if (lastkey == 'k' || lastkey == KEY_UP) {
          key = 'k';
          break;
        }
        ++y;
        lastkey = 'j';
        break;
      case ' ':
        add_cell(head, -1, 0);
      default:
    }

    // addch('#' | A_BOLD);
    refresh();
    fflush(stdout);

    move(0, 0);
    //printw("%d (%d, %d) status %d\n", key, y, x, status);
    if ((head)->next == NULL) {
      //printw("NULL\n");
    } else {
      //printw("non-null (%d, %d)", (&head)->next->y, (&head)->next->x);
    }
    refresh();

    show_snake(food, 'o');
    move_snake(head->end, key);
    show_snake(head, '#');
    check_food(head, food, key);
    check_boundaries(head, key);
    //check_snake_body(head);
    refresh();
    usleep(100000);
    //  flushinp();
  }

  endwin();
}

int show_snake(struct Cell *cell, int disp) {
  Cell *cp = cell;
  move(cell->y, cell->x);
  addch(disp | A_BOLD);
  cell = cell->next;
  while (cell != NULL) {
    move(cell->y, cell->x);
    addch(disp | A_BOLD);
    cell = cell->next;
  }
  move(cp->y, cp->x);
  return 0;
}

int add_cell(struct Cell *cell, int dx, int dy) {
  Cell *head = cell;
  while (cell->next != NULL) {
    cell = cell->next;
  }
  cell->next = malloc(sizeof(Cell));
  cell->next->x = cell->x + dx;
  cell->next->y = cell->y + dy;
  cell->next->next = NULL;
  cell->next->prev = cell;

  head->end = cell->next;

  return cell->next->x;
}

int move_snake(struct Cell *cell, int key) {
  if (!is_moving_key(key)) {
    return 1;
  }
  while (cell != NULL) {
    if (cell->prev == NULL) {
      switch (key) {
        case KEY_RIGHT:
        case 'l':
          cell->x += 1;
          move(5, 0);
          //printw("RIGHT\n");
          break;
        case KEY_LEFT:
        case 'h':
          cell->x -= 1;
          move(5, 0);
          //printw("LEFT\n");
          break;
        case KEY_UP:
        case 'k':
          move(5, 0);
          //printw("UP\n");
          cell->y -= 1;
          break;
        case KEY_DOWN:
        case 'j':

          move(5, 0);
          //printw("DOWN\n");
          cell->y += 1;
          break;
        default:
      }
    } else {
      move(5, 0);
      //printw("PREV: (%d, %d)\n", cell->x, cell->y);
      cell->x = cell->prev->x;
      cell->y = cell->prev->y;
      //printw("NOW: (%d, %d)\n", cell->x, cell->y);
    }
    cell = cell->prev;
  }
  return 0;
}

int is_moving_key(int key) {
  return key == 'l' || key == KEY_RIGHT || key == 'h' || key == KEY_LEFT ||
  key == 'k' || key == KEY_UP || key == 'j' || key == KEY_DOWN;
}

int check_food(struct Cell *cell, struct Cell *food, int lastkey) {
  do {
    if (cell->x == food->x && cell->y == food->y) {
      int x = 0, y = 0;
      switch (lastkey) {
        case KEY_RIGHT:
        case 'l':
          ++x;
          break;
        case KEY_LEFT:
        case 'h':
          --x;
          break;
        case KEY_UP:
        case 'k':
          --y;
          break;
        case KEY_DOWN:
        case 'j':
          ++y;
          break;
        default:
          return -1;
      }
      add_cell(cell, -x, -y);
      if (food->prev != NULL) {
        food->prev->next = food->next;
      }
      if (food->next != NULL) {
        food->next->prev = food->prev;
      } else {
        // printw("adding food...");
        refresh();
        add_food(food, cell);
      }
      return 0;
    }

  } while ((food = food->next) != NULL);

  return 0;
}

int add_food(struct Cell *pfood, struct Cell *snake_head) {

  // printw("add_food()");
  int x = random() % getmaxx(stdscr);
  int y = random() % getmaxy(stdscr);
  while (exists_cell(x, y, snake_head)) {

    x = random() % getmaxx(stdscr);
    y = random() % getmaxy(stdscr);
  }
  pfood->x = x;
  pfood->y = y;
  pfood->end = NULL;
  pfood->prev = NULL;
  pfood->next = NULL;
  return 0;
}

int exists_cell(int x, int y, struct Cell *cells) {
  while (cells != NULL) {
    if (cells->x == x && cells->y == y) {
      return 1;
    }
    cells = cells->next;
  }
  return 0;
}

int check_boundaries(struct Cell *cell, int key) {
  if (cell->y < 0){
    cell->y = getmaxy(stdscr);
  }
  if (cell->y > getmaxy(stdscr)){
    cell->y = 0;
  }
  if (cell->x < 0){
    cell->x = getmaxx(stdscr);
  }
  if( cell->x > getmaxx(stdscr)){
    cell->x = 0;

  }
  //  move_snake(cell, key);
  return 0;
}

int check_snake_body(struct Cell *cell){
  Cell *head = cell;
  do{
    cell = cell->next;
    if(head->x == cell->x && cell->y){
      head->x = 0;
      head->y = 0;
//      cell = cell->end;
//      do{
//        cell = cell->prev;
//        if(cell == NULL){
//        } else{
//
//          if (head->x != cell->x && head->y != cell->y){
//            free(cell->next);
//          }
//        }
//      } while(cell != NULL);
//      head->prev= head->next = NULL;
//      head->end = head;
//      break;
    }
  } while(cell != NULL);
  return 0;
}
