#pragma once

#include <cstring>
#include <cstdlib>
#include <list>
#include <bits/stdc++.h>

#define BUFFER_SIZE 1280 /* size of buffer - fixed */
#define GAP_SIZE 256
#define TAB_WIDTH 4

class GapBuffer
{	
	public:
	GapBuffer(int size);
	int gapStart;
	int gapEnd;
	int blockOffset;
	char *buffer;
	int size; // size of array, not string
	int id;
	bool dirty;

	bool is_valid();
	bool is_empty();
	bool is_full();
	bool is_dirty();
	bool is_at_left();
	bool is_at_right();
	bool equals(GapBuffer b);

	char current_char();
	char next_char();

	char* get_buffer();
	int set_buffer(char* buffer, int size);

	void move_forward();
	void move_backward();
	void insert_char(char c);
	void delete_char();
};