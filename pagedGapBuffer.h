#pragma once
/* PAGE struct definition and related functions */

#include <ncurses.h> // might have to move this
#include <bits/stdc++.h>
#include "gapBuffer.h"

#define CACHE_SIZE 500 /* Default number of lines */
#define WIN_SIZE (LINES - 2) /* Size of window, making room for bottom prompt */
#define NAME_LIMIT 256 /* Max size of a unix filename + 1 */

class PagedGapBuffer {
	std::map<int,GapBuffer> gapBufferDirty;
	int maxCacheSize;
	int dirtySize;

	public:
	PagedGapBuffer(int cacheSize);
	char filename[NAME_LIMIT];
	int numlines;
	int cacheSize;
	int fileSize;
	int startOffsetWithinBuffer;
	std::list<GapBuffer> gapBufferCache;
	std::list<GapBuffer>::iterator startBuffer;
	std::list<GapBuffer>::iterator current;



	void initialize(const char* filename, int size);
	void readData (FILE* fp);
	
	bool is_empty();
	bool is_valid();
	bool is_at_left();
	bool is_at_right();
	bool is_dirty(int offset);

	void move_forward();
	void move_backward();
	void remove_current();

	void add_to_dirty(GapBuffer gapBuffer);
	void split_buffer();
};