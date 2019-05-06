
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "bot.h"

using namespace std;

const int MAX_ROWS = 40;
const int MAX_COLS = 40;
const int MAX_NUM = 10;

int ROWS;  // global variables
int COLS;
int NUM;
int lumber_goal = 300;

enum Phase {COLLECT_P,BUILD_P};
Phase current_phase = COLLECT_P;

int setup_check[] = {0,0,0,0,0,0};
enum Build_Phase {WALK_BP,BUILD_BP,SETUP_BP};
Build_Phase current_bp = SETUP_BP;

/* onStart: 
An Initialization procedure called at the start of the game.
You can use it to initialize certain global variables, or do 
something else before the actual simulation starts.
Parameters:
    rows: number of rows
    cols: number of columns
    num:  number of dwarfs
    log:  a cout-like log */

void onStart(int rows, int cols, int num, std::ostream &log) {
  log << "Start!" << endl; // Print a greeting message

  ROWS = rows; // Save values in global variables
  COLS = cols;
  NUM = num;
}

/* onAction: 
A procedure called each time an idle dwarf is choosing 
their next action.
Parameters:
    dwarf:   dwarf choosing an action
    day:     day (1+)
    hours:   number of hours in 24-hour format (0-23)
    minutes: number of minutes (0-59)
    log:     a cout-like log  */

bool is_tree (Dwarf &dwarf, int r, int c) {
	if ((dwarf.look(r, c) == PINE_TREE) || (dwarf.look(r, c) == APPLE_TREE)) {
		return true;
	} else {
		return false;
	}
}

bool is_zombie(Dwarf &dwarf, int r, int c) {
	if ((dwarf.look(r, c) == ZOMBIE) || (dwarf.look(r, c) == PUMPKIN_ZOMBIE)) {
		return true;
	} else {
		return false;
	}
}

void onAction(Dwarf &dwarf, int day, int hours, int minutes, ostream &log) {
  // Get current position of the dwarf
  int r = dwarf.row();
  int c = dwarf.col();
	//First phase - Collect atleast 310 lumber for all the fences
	//Second phase - Plase down 30 fences.
	
	//Handle Phase Switch
	if(dwarf.lumber() > lumber_goal) {
				log<<"Entering Build Phase"<<endl;
				current_phase = BUILD_P; //keep getting wood till 300
	}
	switch(current_phase) {
		case COLLECT_P : {
			// Look if there is a tree on the right from the dwarf
			  if (is_tree(dwarf, r, c+1)) {
				// If there is a pine tree, chop it
				log << "Found a tree -- chop" << endl;
				dwarf.start_chop(EAST);
				return;
			  } else if (is_tree(dwarf, r, c-1)) {
				// If there is a pine tree, chop it
				log << "Found a tree -- chop" << endl;
				dwarf.start_chop(WEST);
				return;
			  } else if (is_tree(dwarf, r+1,c)) {
				// If there is a pine tree, chop it
				log << "Found a tree -- chop" << endl;
				dwarf.start_chop(SOUTH);
				return;
			  } else if (is_tree(dwarf, r-1,c)) {
				// If there is a pine tree, chop it
				log << "Found a tree -- chop" << endl;
				dwarf.start_chop(NORTH);
				return;
			  } else {
				// Otherwise, move to a random location
				int rr = 0; int cc = 0;
				if((is_tree(dwarf,r+1,c+1)) ||(is_tree(dwarf,r+2,c))) {
					rr = (r+1);
					cc = c;
				} else if ((is_tree(dwarf,r-1,c-1)) || (is_tree(dwarf,r-2,c))) {
					rr = (r-1);
					cc = c;
				} else if(is_tree(dwarf,r,c+2)) {
					rr = r;
					cc = (c+1);
				} else if(is_tree(dwarf,r,c-2)) {
					rr = r;
					cc = (c-1);
				} else if (is_tree(dwarf,r+2,c+2)) {
					rr = (r+2);
					cc = c;
				} else if (is_tree(dwarf,r-2,c-2)) {
					rr = (r-2);
					cc = c;
				} else {
					rr = rand() % ROWS;
					cc = rand() % COLS;
				}
				log << "Walk to " << rr << " " << cc << endl;
				dwarf.start_walk(rr, cc);
				return;
			  }
			break;
		}
		//Fence building phase
		case BUILD_P : {
			//Check to see if dwarf is in starting position
			if(setup_check[dwarf.name()] == 0){
				current_bp = SETUP_BP;
			} else {
				//First chop trees in way. If not blocked, build or walk
				if(is_tree(dwarf, r, c+1)) {
					dwarf.start_chop(EAST);
				} else if(is_tree(dwarf, r, c-1)) {
					dwarf.start_chop(WEST);
				} else if(dwarf.look(r,c-1) != FENCE) {
					current_bp = BUILD_BP;
				} else if(dwarf.look(r,c-1) == FENCE) {
					current_bp = WALK_BP;
				}
			}
			//Possible Build Phase Actions
			switch(current_bp) {
				case WALK_BP : {
					//After building wall move over
					dwarf.start_walk((r),(++c));
					log<<dwarf.name()<<"/ -Build Phase - WALK"<<endl;
					break;
				}
				case BUILD_BP : {
					if((r == (dwarf.name() +5)) && (c < 12)) {
						//Build wall to left every step
						if(is_tree(dwarf, r-1,c)) {
							dwarf.start_chop(WEST);
						} else if(is_tree(dwarf, r+1,c)) {
							dwarf.start_chop(EAST);
						} {
							dwarf.start_build(WEST);
						}
						log<<dwarf.name()<<"/ -Build Phase - BUILD"<<endl;
					} else {
						dwarf.start_walk(dwarf.name(),c);
						log<<r<<"   "<<c<<" Not in right spot"<<endl;
					}
					break;
				}
				case SETUP_BP : {
					//Try to walk to starting spot.
					if(is_tree(dwarf,dwarf.name()+5,5)) {
						dwarf.start_walk(dwarf.name()+5,4);
						log<<dwarf.name()<<" -Tree at SETUP, move left"<<endl;
					} else if(is_tree(dwarf,dwarf.name()+5,4)) {
						dwarf.start_walk(dwarf.name()+5,6);
						log<<dwarf.name()<<" -Tree at left, move right"<<endl;
					} else {
						dwarf.start_walk(dwarf.name()+5,5);
						log<<dwarf.name()<<" -Tree at right, move left"<<endl;
					}
					if((r == dwarf.name()+5) && (c ==5)) {
						//If successful setup, update dwarf condition
						setup_check[dwarf.name()] = 1;
					} else {
						//If not try spot to left and chop tree in way
						dwarf.start_walk(dwarf.name()+5,4);
					}
					log<<dwarf.name()<<"/ -Build Phase - SETUP"<<endl;
					break;
				}
			}
			break;
		}
		
	}
}
