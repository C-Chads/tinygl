#ifndef OPENIMGUI_IMPL
extern float omg_cursorpos[2]; //Defaults to zero
extern float omg_cursorpos_presuck[2]; //Defaults to zero
extern int omg_cursor_has_been_sucked;
extern int omg_cursor_was_inside;  //Set 
extern float omg_buttonjump[2]; //Defaults to zero
// Setting for users using 
extern int omg_bstate_old;
extern int omg_udlr_old[4];
extern int omg_udlr[4];
// cursor button
extern int omg_cb; //Set to zero every iteration.
#endif

#ifndef OPENIMGUI_H
#define OPENIMGUI_H

#include <math.h>
//PROTOTYPE FOR THE OPENIMGUISTANDARD PROPOSAL

//Licensed to you under the CC0 license.



//This is the standard for an intuitive immediate-mode gui specification which gracefully solves many of the shortcomings of 
//other immediate mode gui standards.

//1) How elements are drawn across different environments
//2) How keyboard/gamepad cursor navigation is handled
//3) How the same GUI rendering code can be transported between backends.

//This is a standard for immediate mode GUI elements which can be implemented anywhere and gracefully decreases in feature level based on platform.

//If your target platform can render text and it can render boxes, then it can run openimgui.

// The screen's top left corner is 0,0 and bottom right is 1,1

// All coordinates and dimensions are specified relative to that.

//HOW CURSOR BUTTON IS HANDLED~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//Beginning of your frame...

//omg_cb = 0; 
//if(just_touched || just_mouseleftbuttondown || just_button_down) omg_cb = 1; //Pressed!
//if(just_released_touch || just_mouseleftbutton up || just_button_up) omg_cb = 2; //Released!

//Gui code this frame...

//HOW CURSOR POSITION IS HANDLED:
// On platforms with touch or mouse input, the polling of cursor position will occur like this~~~~~~~~~~~~~
// omg_cursor_has_been_sucked = 0;
// omg_cursorpos[0] = device_cursorpos.x / (float) screenWidth;
// omg_cursorpos[1] = device_cursorpos.x / (float) screenHeight;
// Clamp the cursorpos (if necessary)
// omg_cursorpos[0] = omg_clampf(omg_cursorpos[0]);
// omg_cursorpos[1] = omg_clampf(omg_cursorpos[1]);
// omg_cursorpos_presuck[0] = -1;
// omg_cursorpos_presuck[1] = -1;

// On platforms which use buttons to navigate menu elements...~~~~~~~~~~~~~
// omg_cursor_has_been_sucked = 0;
// if(buttonleft) omg_cursorpos[0] -= omg_buttonjump[0];
// if(buttonright) omg_cursorpos[0] += omg_buttonjump[0];
// if(buttonup) omg_cursorpos[1] -= omg_buttonjump[1];
// if(buttondown) omg_cursorpos[1] += omg_buttonjump[1];
// Clamp the cursorpos
// omg_cursorpos[0] = omg_wrapf(omg_cursorpos[0]);
// omg_cursorpos[1] = omg_wrapf(omg_cursorpos[1]);
// omg_cursorpos_presuck[0] = omg_cursorpos[0];
// omg_cursorpos_presuck[1] = omg_cursorpos[1];

// HOW BUTTON SUCKING WORKS ~~~~~~~~~~~~~~

// On platforms without cursor input such as game consoles, there needs to be an ergonomic way to navigate menus.

// This is achieved by simulating a virtual mouse cursor in the game and "Sucking" it into the closest sucking box.

// We keep track of the cursorposition every frame as well as the position before an attempt to "suck" it has been made.
// This allows us to determine (By testing, for every graphical object) whether or not the cursorposition should be "sucked" into
// the graphical object.

// Normalized cursor position
#ifdef OPENIMGUI_IMPL
float omg_cursorpos[2]; //Defaults to zero
float omg_cursorpos_presuck[2]; //Defaults to zero
int omg_cursor_has_been_sucked;
int omg_cursor_was_inside;  //Set 
float omg_buttonjump[2]; //Defaults to zero
// Setting for users using 
int omg_bstate_old = 0;
int omg_udlr_old[4] = {0,0,0,0};
int omg_udlr[4];
// cursor button
int omg_cb; //Set to zero every iteration.
#endif
//Used for determining the closest button in sucking mode.
static inline float omg_sqrlinelength(float x1, float y1, float x2, float y2){
	return ((x1-x2) * (x1-x2) + (y1-y2) * (y1-y2));
}
//Used for clamping cursor position to the screen.
static inline float omg_clampf(float x){
	return (x>1.0)?1.0: (x<0.0)?0.0:x;
}
//Used for wrapping the cursor position to the screen in button cursor mode.
static inline float omg_wrapf(float x){
	float f = fmod(x, 1);
	if(f<0.0) (f = 1.0 + f);
	return f;
}

static inline void omg_update_keycursor(int _up, int _down, int _left, int _right, int bstate){
	
	omg_cursor_was_inside = 0;
	int up = _up && ! omg_udlr_old[0];
	int down = _down && ! omg_udlr_old[1];
	int left = _left && ! omg_udlr_old[2];
	int right = _right && ! omg_udlr_old[3];
	omg_udlr[0] = up;
	omg_udlr[1] = down;
	omg_udlr[2] = left;
	omg_udlr[3] = right;
	omg_udlr_old[0] = _up;
	omg_udlr_old[1] = _down;
	omg_udlr_old[2] = _left;
	omg_udlr_old[3] = _right;
	omg_cursor_has_been_sucked = 0;
	omg_cursorpos_presuck[0] = omg_cursorpos[0];
	omg_cursorpos_presuck[1] = omg_cursorpos[1];
	if(up)   omg_cursorpos[1] -= omg_buttonjump[1];
	if(down) omg_cursorpos[1] += omg_buttonjump[1];
	if(left) omg_cursorpos[0] -= omg_buttonjump[0];
	if(right)omg_cursorpos[0] += omg_buttonjump[0];
	
	//Clamp the cursorpos
	omg_cursorpos[0] = omg_wrapf(omg_cursorpos[0]);
	omg_cursorpos[1] = omg_wrapf(omg_cursorpos[1]);
	omg_cursorpos_presuck[0] = omg_cursorpos[0];
	omg_cursorpos_presuck[1] = omg_cursorpos[1];
	//printf("BEGIN! Cx = %f, Cy = %f\n", omg_cursorpos[0], omg_cursorpos[1]);
	omg_cb = 0; 
	if(bstate && !omg_bstate_old) omg_cb = 1;
	else if (!bstate && omg_bstate_old) omg_cb = 2;
	omg_bstate_old = bstate;
}

//for mouse cursors and touch input.
static inline void omg_update_mcursor(float ncx, float ncy, int bstate){
	omg_cursor_has_been_sucked = 0;
	omg_cursor_was_inside = 0;
	omg_cursorpos[0] = ncx;
	omg_cursorpos[1] = ncy;
	// Clamp the cursorpos (if necessary)
	omg_cursorpos[0] = omg_clampf(omg_cursorpos[0]);
	omg_cursorpos[1] = omg_clampf(omg_cursorpos[1]);
	omg_cursorpos_presuck[0] = -1;
	omg_cursorpos_presuck[1] = -1;

	omg_cb = 0; 
	if(bstate && !omg_bstate_old) omg_cb = 1;
	else if (!bstate && omg_bstate_old) omg_cb = 2;
	omg_bstate_old = bstate;
}
static inline int omg_boxtest(float x, float y, float xdim, float ydim, float cx, float cy){
	if((x <= cx) &&
			(x+xdim >= cx) &&
			(y <= cy) &&
			(y+ydim >= cy))
		return 1;
	return 0;
}
static inline int omg_box_retval(float x, float y, float xdim, float ydim){
	if(omg_cursorpos_presuck[0] == -1) 
		return omg_boxtest(x,y,xdim,ydim,	omg_cursorpos[0],omg_cursorpos[1]);
	return omg_boxtest(x,y,xdim,ydim,		omg_cursorpos_presuck[0],omg_cursorpos_presuck[1]);
}
static inline void omg_box_suck(float x, float y, float xdim, float ydim, int sucks, float buttonjumpx, float buttonjumpy){
	 if(omg_cursorpos_presuck[0] != -1 && sucks){ //Do not attempt to suck if this graphical element does not suck or sucking is not enabled.
		int btest = omg_boxtest(x,y,xdim,ydim, omg_cursorpos_presuck[0], omg_cursorpos_presuck[1]);
		 if(!omg_cursor_has_been_sucked){
		 	//We are free to try to suck up the cursor without a check.
			omg_cursorpos[0] = x + xdim/2.0;
			omg_cursorpos[1] = y + ydim/2.0;
			omg_cursor_has_been_sucked = 1;
		  	omg_buttonjump[0] = buttonjumpx;
		  	omg_buttonjump[1] = buttonjumpy;
		  	if(btest) omg_cursor_was_inside = 1;
		  	//puts("Initial grab...\n");
		  	//printf("Cx = %f, Cy = %f\n", omg_cursorpos[0], omg_cursorpos[1]);
		} else if (
		(!omg_cursor_was_inside && //Cursor was not inside.
		omg_sqrlinelength(x+xdim/2.0, y+ydim/2.0, 			omg_cursorpos_presuck[0],  omg_cursorpos_presuck[1]) < 
		           omg_sqrlinelength(omg_cursorpos[0], omg_cursorpos[1], omg_cursorpos_presuck[0],  omg_cursorpos_presuck[1])
		 ) || //Cursor was inside, if it's inside this one as well, pick the closest.
		 (!omg_cursor_was_inside && btest) ||
		  (
		  	btest && 
			omg_sqrlinelength(x+xdim/2.0, y+ydim/2.0, 			omg_cursorpos_presuck[0],  omg_cursorpos_presuck[1]) < 
		    omg_sqrlinelength(omg_cursorpos[0], omg_cursorpos[1], omg_cursorpos_presuck[0],  omg_cursorpos_presuck[1])
		  )
		           ){
		           //The box is closer than the current suck position.
			omg_cursorpos[0] = x+xdim/2.0;
			omg_cursorpos[1] = y+ydim/2.0;
			omg_cursor_has_been_sucked = 1;
		  omg_buttonjump[0] = buttonjumpx;
		  omg_buttonjump[1] = buttonjumpy;
		  //if(boxtest(x,y,xdim,ydim)) omg_cursor_was_inside = 1;
		  omg_cursor_was_inside = omg_boxtest(x,y,xdim,ydim, omg_cursorpos_presuck[0], omg_cursorpos_presuck[1]);
		  //puts("Found a different button!\n");
		  //printf("Cx = %f, Cy = %f\n", omg_cursorpos[0], omg_cursorpos[1]);
		}
	}
}
// OMG_BOX:
// Draws a box on the screen.
// Returns whether or not the cursor was inside it this frame (NOT IF IT GOT __SUCKED__ INSIDE IT!)
// x,y are the top left corner.
// xdim, ydim, are the width and height of the box.
// hints is a set of implementation-specific parameters describing the nature of how the box is drawn,
// sucks indicates whether or not the cursor position is "sucked" into the button (See: HOW BUTTON SUCKING WORKS)
// buttonjumpx and buttonjumpy are the amount by which the cursor will jump in X and Y when pressing the menu navigation arrows.
// The return value is determined like this:
// if(omg_cursorpos_presuck[0] == -1) return omg_boxtest(omg_cursorpos) else
//	return boxtest(omg_cursorpos_presuck)
// The suck test works like this:
// if(omg_cursorpos_presuck[0] != -1 && sucks){ //Do not attempt to suck if this graphical element does not suck or sucking is not enabled.
// if(!omg_cursor_has_been_sucked){ //We are free to try to suck up the cursor without a check.
//	omg_cursorpos[0] = x+xdim/2.0;
//	omg_cursorpos[1] = y+ydim/2.0;
//	omg_cursor_has_been_sucked = 1;
//  omg_buttonjump[0] = buttonjumpx;
//  omg_buttonjump[1] = buttonjumpy;
//} else if (omg_sqrlinelength(x+xdim/2.0, y+ydim/2.0, omg_cursorpos_presuck[0],  omg_cursorpos_presuck[1]) < 
//           omg_sqrlinelength(omg_cursorpos[0], omg_cursorpos[1], omg_cursorpos_presuck[0],  omg_cursorpos_presuck[1])){ //The box is closer than the current suck position.
//	omg_cursorpos[0] = x+xdim/2.0;
//	omg_cursorpos[1] = y+ydim/2.0;
//	omg_cursor_has_been_sucked = 1;
//  omg_buttonjump[0] = buttonjumpx;
//  omg_buttonjump[1] = buttonjumpy;
//}}
//When sucking is enabled (omg_cursorpos_presuck[0] != -1) the box test will be performed on cursorpos_presuck.
//You can use the above static inline functions as a reference for your implementation.

int omg_box(float x, float y, float xdim, float ydim, int sucks, float buttonjumpx, float buttonjumpy, int hints);

// OMG_TEXTBOX:
// Draws a box... with text in it
// All the args are the same, and its return value is the same, except now it can draw text.
// It should handle all the same hints as omg_box.
// the hintstext variable should handle all 
// The textsize is an implementation-specific indication of how large the text in the box should be.
// The x and y dimensions of the box are automatically deduced from text.
// Text containing newlines will extend the Y dimension of the box,
// and the longest line of text will determine the x dimension of the box.
// Otherwise, it is functionally identical to omg_box.
int omg_textbox(float x, float y, const char* text, int textsize, int sucks, float buttonjumpx, float buttonjumpy, int hints, int hintstext);


#endif
