#include "slate.h"

using namespace std;

int y_offset = 0; // TODO: move to local scope
int tab_offset = 0;
int max_x, max_y;

#define DEBUG

void print_loc(int x, int y) {
	#ifdef DEBUG
	int oldx, oldy;
	getyx(stdscr, oldy, oldx);
	mvprintw(0, COLS - 20, "x: %d y: %d o: %d", x, y, y_offset);
	move(oldy, oldx);
	#endif
}

int main(int argc, char *argv[]) {	
	PagedGapBuffer page(CACHE_SIZE);

	if(argc > 1) {
        if(file_exists(argv[1])) {
		    load_file(&page, argv[1]);
        }
        else {
        	page.initialize(argv[1], BUFFER_SIZE-GAP_SIZE);
        }
	}
	else { // initialize
		page.initialize("untitled.txt", BUFFER_SIZE-GAP_SIZE);
	}
	
	/* curses interface */
	initscr();
	noecho();
	keypad(stdscr, true);

	getmaxyx(stdscr, max_y, max_x);

	int beg = 0;
	int end = WIN_SIZE;
	int y, x; // position on screen

	update_status("Press F4 to quit");
	
	print_page(&page);
	//getyx(stdscr, y, x);
	move(0,0);
	x = y = 0;

    char status[NAME_LIMIT + 10];
	while(true) {
        print_loc(x, y);
		beg = 0 + y_offset;
		end = WIN_SIZE + y_offset;
		int ch = getch();
		update_status("Press F4 to quit"); // default text
		switch(ch) {
			case KEY_F(4):
                if(prompt_yesno("Are you sure you want to quit?"))
				    goto endnc;
	            print_page(&page);
				break;
			case KEY_F(5):
			//	save_file(&page);
                sprintf(status, "Saved as \'%s\'", page.filename);
				update_status(status);
				break;
            case KEY_F(6):
                prompt_string("Save As:", page.filename, NAME_LIMIT);
            //  save_file(&page);
                sprintf(status, "Saved as \'%s\'", page.filename);
				print_page(&page);
				update_status(status);
                break;
			case KEY_UP:
				move_up(&page, &x, &y);
				break;
			case KEY_DOWN:
				move_down(&page, &x, &y);
				break;
			case KEY_LEFT:
				move_left(&page, &x, &y);
				break;
			case KEY_RIGHT:
				move_right(&page, &x, &y);
				break;
			case KEY_DC:
			case 127: // backspace key...
			case KEY_BACKSPACE:
				remove_char(&page); 					// delete
				move_left(&page, &x, &y);					  // char behind cursor
				print_page(&page);
				move(y, x);
				break;
            /*
            case '\t':
                for(i = 0; i < TAB_WIDTH; i++)
                {
                    insert_char(&page.text[y + y_offset], ' ', x - 1);
                    print_page(&page, beg, end);
                    move_right(&page, &x, &y);
                }
                break;
            */
			case '\n': // newline
				insert_char(&page, '\n');
				print_page(&page);
				move_right(&page, &x, &y);
				break;
			default: // all other chars
				if( isprint(ch) )
				{
					insert_char(&page, ch);
					print_page(&page);
					move_right(&page, &x, &y);
				}
		}
	}
endnc:	
	/* end curses */
    endwin();
	//dest_page(&page);
	return EXIT_SUCCESS;
} // main

// prints a message at the bottom of the window
void update_status(char *info) {
	int oldy, oldx; getyx(stdscr, oldy, oldx);
	
	attron(A_REVERSE);
	move(LINES - 1, 0);
	clrtoeol();
	printw(info);
	attroff(A_REVERSE);	
	move(oldy, oldx);
}

// inserts a character in the buffer
void insert_char(PagedGapBuffer *p, char ch) {
	if (p->current->is_full()) {
		p->split_buffer();
	}
	p->current->insert_char(ch);
}

// removes a character from the buffer
void remove_char(PagedGapBuffer *p) {
	if (!p->current->is_at_left()) {
		p->current->delete_char();
	}
	else if (p->current == p->gapBufferCache.begin())
		return;
	else {
		p->move_backward();
		p->current->delete_char();
	}

	// if the entire buffer gap is empty
	if (p->current->is_empty()) {
		p->remove_current();
	}
}

/* movement */
void move_left(PagedGapBuffer *p, int *x, int *y) {
	if (!p->current->is_at_left()) {
		p->current->move_backward();
	}
	else if (p->current == p->gapBufferCache.begin())
		return;
	else {
		p->move_backward();
		p->current->move_backward();
	}
	if(*x > 0) 
		move(*y, --(*x));
	else {
		int count = 0;
		list<GapBuffer>::iterator g = p->current;
		int position = g->gapStart-1;
		bool flag = true;
		while (flag) {
			while (position < 0) {
				if (g->equals(*(p->gapBufferCache.begin()))) {
					flag = false;
					break;
				}
				else {
					g--;
					position = g->gapStart-1;
				}
			}
			if (!flag || g->buffer[position] == '\n')
				break;
			count++;
			position--;
		}
		count = count % (max_x+1);				// if the previous line has more characters than the window width
		--(*y);
		*x = count;
		move(*y, *x);
	}
}

// moves the cursor to the right
void move_right(PagedGapBuffer *p, int *x, int *y) {
	if (!p->current->is_at_right()) {
		p->current->move_forward();
	}
	else if (p->current != p->gapBufferCache.end() && next(p->current) == p->gapBufferCache.end())
		return;
	else {
		p->move_forward();
		p->current->move_forward();
	}
	if (p->current->current_char() == '\n' || *x == max_x) {
		*x = 0;
		++(*y);
	}
	else
		++(*x);

	move(*y, *x);	
}

// move the cursor position up
void move_up(PagedGapBuffer *p, int *x, int *y) {
	int count = 0;
	int chars = 0;
	int initial_pos = *x;
	bool flag = true;
	while (count <= 1) {
		if (p->current->is_at_left()) {
			if (p->is_at_left()) {
				flag = false;
				break;
			}
			else {
				p->move_backward();
			}
		}
		if (p->current->current_char() == '\n')
			count++;
		p->current->move_backward();
		chars++;
		if (chars == max_x+1 && count==0)
			break;
	}

	if (!flag && count==0) {
		*x = 0;
		move(*y, *x);
		return;
	}

	if (chars == max_x+1 && count==0) {
		--(*y);
		move(*y, *x);
		return;
	}

	count = 0;
	while (true) {
		if (p->current->is_at_right()) {
			if (p->is_at_right())
				break;
			else {
				p->move_forward();
			}
		}
		p->current->move_forward();
		count++;
		if (p->current->current_char() == '\n' || count-1 == initial_pos)
			break;		
	}

	*x = count-1;			// set the x position to the end of the line if it ends early
	--(*y);					// move to the previous line
	move(*y, *x);
}

// move the cursor position down
void move_down(PagedGapBuffer *p, int *x, int *y) {
	int count = 0;
	int initial_pos = *x;
	bool flag = true;
	while (flag) {
		if (p->current->is_at_right()) {
			if (p->is_at_right())
				break;
			else {
				p->move_forward();
			}
		}
		p->current->move_forward();
		count++;
		if (p->current->current_char() == '\n' || count == max_x+1)
			flag = false;
	}

	// reached the end of pagedGapBuffer, move cursor to the end of the line
	if (flag) {
		*x += count;
		move(*y, *x);
		return;
	}
	if (count == max_x+1) {
		++(*y);
		move(*y, *x);
		return;
	}
	count = 0;
	while (true) {
		if (p->current->is_at_right()) {
			if (p->is_at_right()) {			// encountered end of pagedGapBuffer
				break;
			}
			else {
				p->move_forward();
			}
		}
		p->current->move_forward();
		count++;
		if (p->current->current_char() == '\n' || count-1 == initial_pos)
			break;		
	}

	if (p->current->is_at_left() && !p->is_at_left()) {
		p->move_backward();
	}
	p->current->move_backward();
	*x = count-1;				// set the x position to the end of the line if it ends early
	++(*y);					// move to the next line
	move(*y, *x);
}

// load file data into file buffer
void load_file(PagedGapBuffer *p, char *filename) {
	FILE *fp = fopen(filename, "r");
	int size = get_file_size(filename);
	p->initialize(filename, size-1);

    if(fp == NULL) { // file doesn't exist yet. don't bother reading
        return;
    }

    p->readData(fp);
	fclose(fp);
}

/*
void save_file(PAGE *p)
{
	FILE *fp = fopen(p->filename, "w");
	int line, col;

	for(line = 0; line < p->numlines; line++)
	{
		col = 0;
		while(p->text[line].line[col] != '\0')
		{
			fputc(p->text[line].line[col], fp);
			col++;
		}
		fputc('\n', fp);
	}

	fclose(fp);

} // save_file
*/

// print the page to the screen
void print_page(PagedGapBuffer *p) {
	int position = p->startOffsetWithinBuffer;
	int y, x;
	bool flag = true;
	x = y = 0;
	for (list<GapBuffer>::iterator ctr = p->startBuffer; ctr != p->gapBufferCache.end() && flag; ctr++) {
		while (position < ctr->gapStart) {
			if (ctr->buffer[position] == '\n') {
				y++;
				x=0;
				clrtoeol();
				move(y,x);
			}
			else {
				mvaddch(y,x,ctr->buffer[position]);
				x++;
			}
			position++;
			if (x > max_x) {
				y++;
				x=0;
			}
			if (y > max_y) {
				flag = false;
				break;
			}
		}
		position = ctr->gapEnd + 1;
		while (position < ctr->size) {
			if (ctr->buffer[position] == '\n') {
				y++;
				x=0;
				clrtoeol();
				move(y,x);
			}
			else {
				mvaddch(y,x,ctr->buffer[position]);
				x++;
			}
			position++;
			if (x > max_x) {
				y++;
				x=0;
			}
			if (y > max_y) {
				flag = false;
				break;
			}
		}		
	}
	clrtoeol();
	y++;
	x=0;
	move(y,x);
	clrtoeol();
}

int file_exists(char *filename)
{
    FILE *fp = fopen(filename, "r");
    if(fp != NULL) {
        fclose(fp);
        return 1;
    }
    return 0;
}

// get file size
off_t get_file_size(const char *filename) {
    struct stat st; 

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1; 
}