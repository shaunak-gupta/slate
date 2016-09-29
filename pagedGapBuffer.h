#ifndef PAGEDGAPBUFFER_H
#define PAGEDGAPBUFFER_H
/* PAGE struct definition and related functions */

#include <ncurses.h> // might have to move this
#include "gapBuffer.h" 

#define CACHE_SIZE 500 /* Default number of lines */
#define WIN_SIZE (LINES - 2) /* Size of window, making room for bottom prompt */
#define NAME_LIMIT 256 /* Max size of a unix filename + 1 */

#endif