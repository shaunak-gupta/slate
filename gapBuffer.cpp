#include "gapBuffer.h"

class BufferGap
{
	char *buffer;
	int size; // size of array, not string
	int blockOffset;
	int id;
	bool flag;

	public:
	BufferGap(int size);
	int gapStart;
	int gapEnd;

	bool is_valid();
	bool is_empty();
	bool is_full();
	bool is_at_left();
	bool is_at_right();

	void move_forward();
	void move_backward();
	void insert_char(char c);
	void delete_char();
};

// initialize a new empty buffer gap
BufferGap::BufferGap(int size) { 
	this->size = size;
	this->buffer = new char[size];
	this->buffer[0] = '\0';
	this->gapStart = 0;
	this->gapEnd = size;
	this->flag = false;
}

// returns whether the buffer gap is valid
bool BufferGap::is_valid() { 
	if (this->size>0 && 
		sizeof(this->buffer)==this->size && 
		this->gapStart >= 0 && 
		this->gapStart <= this->size &&
		this->gapEnd >= 0 &&
		this->gapEnd <= this->size &&
		this->gapStart <= this->gapEnd)
		return true;
	return false;
}

// returns whether the buffer gap is empty
bool BufferGap::is_empty() { 
	if (this->gapStart == 0 &&
		this->gapEnd == this->size)
		return true;
	return false;
}

// returns whether the buffer gap is full
bool BufferGap::is_full() {
	if (this->gapStart == this->gapEnd)
		return true;
	return false; 
}

// returns whether the buffer gap is at left
bool BufferGap::is_at_left() { 
	if(this->gapStart == 0)
		return true;
	return false;
}

// returns whether the buffer gap is at right
bool BufferGap::is_at_right() { 
	if (this->gapEnd == this->size)
		return true;
	return false;
}

// moves the cursor to the right
void BufferGap::move_forward() {
	if (!this->is_at_right()) {
		this->buffer[gapStart] = this->buffer[gapEnd];
		(this->gapStart)++;
		(this->gapEnd)++;
	} 
}

// moves the cursor to the left
void BufferGap::move_backward() { 
	if (!this->is_at_left()) {
		(this->gapStart)--;
		(this->gapEnd)--;
		this->buffer[gapEnd] = this->buffer[gapStart];
	} 
}

// inserts the character c before the gap
void BufferGap::insert_char(char c) { 
	if (!this->is_full()) {
		this->buffer[gapStart] = c;
		(this->gapStart)++;
		this->flag = true;
	}
}

// deletes the character before the gap
void BufferGap::delete_char() { 
	if (!this->is_empty()) {
		(this->gapStart)--;
		this->flag = true;
	}
}