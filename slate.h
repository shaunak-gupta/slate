#pragma once

#include <cstdio>
#include <cstdlib> // declared in line.h
#include <ctype.h>
#include <cstring> // declared in line.h
#include <ncurses.h> // -lncurses. declared in page.h
#include <sys/stat.h>

#include "pagedGapBuffer.h"
#include "prompt.h"

void update_status(char *info);

void load_file(PagedGapBuffer *p, char *filename);
int file_exists(char *filename);

void move_left(PagedGapBuffer *p, int *x, int *y);
void move_right(PagedGapBuffer *p, int *x, int *y);
void move_up(PagedGapBuffer *PagedGapBufferp, int *x, int *y);
void move_down(PagedGapBuffer *p, int *x, int *y);

void insert_char(PagedGapBuffer *p, char ch);

void print_page(PagedGapBuffer *p);

off_t get_file_size(const char *filename);