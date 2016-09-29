#ifndef TEXT_H
#define TEXT_H

#include <cstdio>
#include <cstdlib> // declared in line.h
#include <ctype.h>
#include <cstring> // declared in line.h
#include <ncurses.h> // -lncurses. declared in page.h

#include "pagedGapBuffer.h"
#include "prompt.h"

void update_status(char *info);

int count_lines(FILE *fp);
void load_file(PAGE *p, char* filename);
void save_file(PAGE *p);
int file_exists(char *filename);

void move_left(int *x, int *y);
void move_right(PAGE *p, int *x, int *y);
void move_up(PAGE *p, int *x, int *y);
void move_down(PAGE *p, int *x, int *y);


#endif

