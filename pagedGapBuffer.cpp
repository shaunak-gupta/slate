#include "pagedGapBuffer.h"

class PagedGapBuffer {

	char filename[NAME_LIMIT];
	int numlines;
	int size; // size of array

	list<GapBuffer> gapBufferCache;
	map<int,GapBuffer> gapBufferDirty;
	list<GapBuffer>::iterator current;
	int cacheSize;
	int maxCacheSize;
	int dirtySize;

	public:
	PagedGapBuffer(int cacheSize);
	
	bool is_empty();
	bool is_valid();
	bool is_at_left();
	bool is_at_right();
	bool is_dirty(int offset);

	void move_forward();
	void move_backward();
	void remove_current();

	void add_to_dirty(int gapBufferIndex);
	void split_buffer();
}

// initializes a new paged gap buffer
PagedGapBuffer::PagedGapBuffer(int cacheSize) {
	this->maxCacheSize = cacheSize;
	this->position = 0;
	this->dirtySize = 0;
	this->cacheSize = 0;
	this->current = this->gapBufferCache.begin();
}

// returns whether the paged gap buffer is empty
bool PagedGapBuffer::is_empty() {
	if (this->cacheSize == true)
		return true;
	return false;
}

// returns whether the paged gap buffer is valid
bool PagedGapBuffer::is_valid() {
	// empty paged gap buffer is valid
	if (this->is_empty())
		return true;

	// paged gap buffer with a single empty buffer gap is valid
	if (this->cacheSize == 1 && 
		this->gapBufferCache.front().is_empty())
		return true;

	// ensure that gaps are aligned properly
	// for gap buffers to the left of current, gaps should be aligned to the right
	// for gap buffers to the right of current, gaps should be aligned to the left
	bool flag = true;
	for (GapBuffer & buffer : gapBufferCache) {
		if (buffer == current)
			flag = false;
		else if (flag == true && !buffer->is_at_right())
			return false;
		else if (flag == false && !buffer->is_at_left())
			return false;
	}
	return true;
}

// returns whether current position is at the far left end
bool PagedGapBuffer::is_at_left() {
	if (this->current == this->gapBufferCache.front())
		return true;
	return false;
}

// returns whether current position is at the far right end
bool PagedGapBuffer::at_right() {
	if (this->current == this->gapBufferCache.back())
		return true;
	return false;
}

// returns whether the gap buffer at given offset is dirty
bool PagedGapBuffer::is_dirty(int offset) {
	if (this->gapBufferDirty.find(offset) == this->gapBufferDirty.end())
		return false;
	return true;
}

// moves current position to the right
void PagedGapBuffer::move_forward() {
	(this->current)++;

}

// moves current position to the left
void PagedGapBuffer::move_backward() {
	(this->current)--;
}

// removes current gap buffer from the page
void PagedGapBuffer::remove_current() {
	this->current = this->gapBufferCache.erase(this->current);
}

// adds current gap buffer to the dirty list
void PagedGapBuffer::add_to_dirty(GapBuffer gapBuffer) {
	this->gapBufferDirty.insert(make_pair(gapBuffer->blockOffset, gapBuffer));
}

// splits the current gap buffer into two
void PagedGapBuffer::split_buffer() {
	BufferGap *n = new BufferGap(BUFFER_SIZE);
	char bufferCopy[BUFFER_SIZE];
	int position, gapOriginal;
	strncpy(this->current->buffer, bufferCopy, BUFFER_SIZE);
	gapOriginal = this->current->gapStart;
	this->current->gapStart = min(BUFFER_SIZE/2, gapOriginal);
	this->current->gapEnd = min(BUFFER_SIZE, this->current->gapStart + BUFFER_SIZE/2);
	for (position = 0; this->current->gapEnd+position<BUFFER_SIZE; position++) {
		this->current->buffer[this->current->gapEnd+position]
		= bufferCopy[this->current->gapStart+position];
	}
	if (position == 0) {
		strncpy(n->buffer, bufferCopy + this->current->gapStart, gapOriginal - this->current->gapStart);
		n->gapStart = gapOriginal - this->current->gapStart;
		n->gapEnd = n->gapStart + BUFFER_SIZE/2;
		strncpy(n->buffer + n->gapEnd, bufferCopy + gapOriginal, BUFFER_SIZE - gapOriginal);
	}
	else {
		n->gapStart = 0;
		n->gapEnd = BUFFER_SIZE/2;
		strncpy(n->buffer + BUFFER_SIZE/2, bufferCopy + BUFFER_SIZE/2, BUFFER_SIZE/2);
	}
	n->flag = 1;
	n->blockOffset = this->current->blockOffset;
	n->size = this->current->size;
	
}