#include "pagedGapBuffer.h"
#include "gapBuffer.h" 

using namespace std;


// initializes a new paged gap buffer
PagedGapBuffer::PagedGapBuffer(int cacheSize) {
	this->maxCacheSize = cacheSize;
	this->dirtySize = 0;
	this->cacheSize = 0;
}

// initializes the gap buffers and other variables
void PagedGapBuffer::initialize(const char* filename, int size) {
	this->cacheSize = min(maxCacheSize, (int)ceil(size*1.0/(BUFFER_SIZE-GAP_SIZE)));
	this->fileSize = size;
	strcpy(this->filename, filename);

	for(int i = 0; i < this->cacheSize; i++) {
    	GapBuffer *b = new GapBuffer(BUFFER_SIZE);
		b->blockOffset = i;
		b->id = 0;
		b->gapStart = 0;
		b->gapEnd = GAP_SIZE-1;
		if (i == this->cacheSize-1)
			b->gapEnd = BUFFER_SIZE - (size - (BUFFER_SIZE-GAP_SIZE)*i) - 1;
		this->gapBufferCache.push_back(*b);
	}
	this->startBuffer = this->gapBufferCache.begin();
	this->startOffsetWithinBuffer = 0;
	this->current = this->gapBufferCache.begin();
}

// loads data into the gap buffers
void PagedGapBuffer::readData (FILE* fp) {
	int size;
	char buffer[BUFFER_SIZE-GAP_SIZE];
	size = this->fileSize;
	for(auto ctr = this->gapBufferCache.begin(); ctr != this->gapBufferCache.end(); ctr++) {
        int bytes = fread(buffer, 1, min(BUFFER_SIZE-GAP_SIZE, size), fp);
        size -= bytes;    
    	ctr->set_buffer(buffer, bytes);
    	if (bytes < size && bytes < BUFFER_SIZE-GAP_SIZE)
    		break;
	}
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
	for (auto buffer = gapBufferCache.begin(); buffer != gapBufferCache.end(); buffer++) {
		if (buffer->equals(*current))
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
	if (this->current->equals(this->gapBufferCache.front()))
		return true;
	return false;
}

// returns whether current position is at the far right end
bool PagedGapBuffer::is_at_right() {
	if (this->current->equals(this->gapBufferCache.back()))
		return true;
	return false;
}

// returns whether the gap buffer at given offset is dirty
/*bool PagedGapBuffer::is_dirty(int offset) {
	if (this->gapBufferDirty.find(offset)->second.equals(this->gapBufferDirty.end()))
		return false;
	return true;
}*/

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
	this->gapBufferDirty.insert(make_pair(gapBuffer.blockOffset, gapBuffer));
}

// splits the current gap buffer into two
void PagedGapBuffer::split_buffer() {
	GapBuffer *n = new GapBuffer(BUFFER_SIZE);
	char bufferCopy[BUFFER_SIZE];
	int position, gapOriginal;
	bool current_change = true;
	strncpy(this->current->buffer, bufferCopy, BUFFER_SIZE);
	gapOriginal = this->current->gapStart;
	this->current->gapStart = min(BUFFER_SIZE/2, gapOriginal);
	this->current->gapEnd = min(BUFFER_SIZE, this->current->gapStart + BUFFER_SIZE/2);
	if (this->current->gapStart == gapOriginal)
		current_change = false;
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
	n->dirty = true;
	n->blockOffset = this->current->blockOffset;
	n->id = this->current->id + 1;	

	for (auto buffer = gapBufferCache.begin(); buffer != gapBufferCache.end() && buffer->blockOffset == n->blockOffset; buffer++) {
		buffer->id += 1;
	}

	this->move_forward();
	this->gapBufferCache.insert(current, *n);
	if (current_change == false)
		this->move_backward();
}