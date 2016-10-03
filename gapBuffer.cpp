#include "gapBuffer.h"

using namespace std;

// initialize a new empty buffer gap
GapBuffer::GapBuffer(int size) { 
	this->size = size;
	this->buffer = new char[size];
	this->buffer[0] = '\0';
	this->gapStart = 0;
	this->gapEnd = size;
	this->dirty = false;
}

// returns whether the buffer gap is valid
bool GapBuffer::is_valid() { 
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
bool GapBuffer::is_empty() { 
	if (this->gapStart == 0 &&
		this->gapEnd == this->size)
		return true;
	return false;
}

// returns whether the buffer gap is full
bool GapBuffer::is_full() {
	if (this->gapStart == this->gapEnd)
		return true;
	return false; 
}

// returns whether the buffer gap is dirty
bool GapBuffer::is_dirty() {
	if (this->dirty == true)
		return true;
	return false; 
}

// returns whether the buffer gap is at left
bool GapBuffer::is_at_left() { 
	if(this->gapStart == 0)
		return true;
	return false;
}

// returns whether the buffer gap is at right
bool GapBuffer::is_at_right() { 
	if (this->gapEnd == this->size)
		return true;
	return false;
}

// returns whether two buffer gaps are the same
bool GapBuffer::equals(GapBuffer b) { 
	if (this->blockOffset == b.blockOffset && this->id == b.id)
		return true;
	return false;
}

// moves the cursor to the right
void GapBuffer::move_forward() {
	if (!this->is_at_right()) {
		this->buffer[gapStart] = this->buffer[gapEnd+1];
		(this->gapStart)++;
		(this->gapEnd)++;
	} 
}

// moves the cursor to the left
void GapBuffer::move_backward() { 
	if (!this->is_at_left()) {
		this->buffer[gapEnd] = this->buffer[gapStart-1];
		(this->gapStart)--;
		(this->gapEnd)--;
	} 
}

// inserts the character c before the gap
void GapBuffer::insert_char(char c) { 
	if (!this->is_full()) {
		//this->buffer[gapStart] = c;
		//(this->gapStart)++;
		this->buffer[gapEnd] = c;
		(this->gapEnd)--;
		this->dirty = true;
	}
}

// deletes the character before the gap
void GapBuffer::delete_char() { 
	if (!this->is_empty()) {
		(this->gapStart)--;
		this->dirty = true;
	}
}

// returns the buffer value
char* GapBuffer::get_buffer() { 
	return buffer;
}

// sets the buffer value
int GapBuffer::set_buffer(char* buffer, int size) {
	if (this->gapEnd+1+size == this->size) {
		strncpy(this->buffer+this->gapEnd+1, buffer, size);
		return 0;
	}
	return 1;
}

// returns the current character just before cursor position
char GapBuffer::current_char() {
	if (this->is_at_left()) {
		return '\0';
	}
	return this->buffer[gapStart-1];
}

// returns the next character just after cursor position
char GapBuffer::next_char() {
	if (this->is_at_right()) {
		return '\0';
	}
	return this->buffer[gapEnd+1];
}