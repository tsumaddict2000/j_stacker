#include <LedControl.h>
#include "binary.h"
//I haven't used LedControl, so I'll just keep all your LED stuff as-is
LedControl lc = LedControl(14, 13, 12, 4);

//I also didn't really test any of this.. just winging it.  But I think I got it?  I don't have LedControl.

//things that don't change in value can be set with #define at the start..
//  this allows for 2 things:
//  a) you can quickly / easily change these values once without having to find all places in your code
//  b) #define is handled at compile time, so these don't take up memory.  And memory is a super limited on Arduino
#define SCREEN_WIDTH 8
#define SCREEN_HEIGHT 8
//I'm adding this one in cause it's a good example of why you might want to store these values in a #define
#define DISPLAY_COUNT 4
//Here's another great example.  You're defining a pin number, and it never changes... so you don't really
//  need this as a variable.
#define SW_pin 15

//For readability and eventually knowing what is what, when things get complicated, it might
//  be good to give these real names.
/*
int i = 0, SW_pin = 15, a = 0, x = 7, y = 6, butpress = 1, dir = 0,
    sto_x = 0, leh = 4, d = 0, sta = 1;
*/
//I deleted: i, SW_pin, d.
//  i and d are local variables.  SW_pin has been changed to a #define
int a = 0, x = 7, y = 6, butpress = 1, dir = 0,
    sto_x = 0, leh = 4, sta = 1;

//Something to be aware of:
//INT vs BYTE.  In Arduino, available memory is at a premium!  This is a super simple project, but if you start
//  dealing with more complicated things, you might run out of memory.  So it's great to be aware of things like
//  byte vs int vs float..  Bytes are 1 byte, Ints are 2 bytes, floats are 4 bytes.  Huge difference!  If your values will
//  be less than 128 (0 - 127), then you can safely use byte and save 3 bytes per variable.  This adds up like crazy.
//NOTE: Byte cannot be a negative number!  It will wrap...  -1 becomes 127.  If you give it 128, it goes back to zero, etc.
//There's stuff like unsigned int.  Which gives 0 - 64,000 ish values.  rather than -32,000 to 32,000 for int.
int positionX = 7;  // x
//your position Y goes below zero at times, so we can't blindly change it to a BYTE.  so I'm not changing either X or Y.
int positionY = 6;  // y
//This guy is zero to 4, so it can be a byte.
byte currentDisplay = 0; // a
byte stopX = 0;  //sto_x 
//I used byte again, since your platformWidth will never be more than 4.
byte platformWidth = 4; //  leh
//NOTE, I don't actually use these renamed values.. just giving you ideas.  Your main language might not be english.

//naming values stuff like "sta" will get really confusing.  Typing the full word is barely longer, and much more readable.
//You can use bool, too.  since it's just yes or no.  Bool, however, is still 1 byte.  So you could just use byte if you
//  want to keep it as a 0 or 1.
bool start = true;  // sta

bool gameOver = false;

//Direction being 0 or 1 can be a byte.  OR you can have it as an int and use -1 and 1.
//Your abs(dir - 1) is neat for giving you zero or 1..
//  You could also do:  dir = dir * -1;
//  1 * -1 = -1.
//  -1 * -1 = 1.
//Why would you use one over the other?  Generally if you're using the direction as part of a calculation, I guess.
//  abs(dir - 1) is a bit less readable, too, maybe?  As if people code for readability.  :D
//I'm leaving this as INT cause i'll write some code for -1 and 1 for direction.
//int dir = 0;


//OK! ON to the setup!
void setup() {

  //whether you use a while loop or a for loop, you can really just have the looping value be a local variable.
  //This frees up that global variable "int i"..  as in, if the system doesn't need it at the moment, then it
  //  can release the memory.  By keeping it global, it will always be allocated.
  //If you know DISPLAY_COUNT will always be less than 128, you can actually use BYTE.  this is 1 byte, rather 
  //  than int, which is 2 bytes

  //I deleted i from your global variables, so you need to define it here as a local variable.
  byte i = 0;
  while (i < 4) { //Loop to initiate and clear all displays
    lc.shutdown(i, false);
    lc.setIntensity(i, 6);
    lc.clearDisplay(i);
    i++;
  }
  //Since this is a local variable, and you aren't using it again in the current local space ("scope"), then
  //  there's no need to reset the value to zero!
  //i = 0;

  //Here's the same while loop re-written as a for loop..
/*
  for (byte i = 0; i < DISPLAY_COUNT; i++) {
    lc.shutdown(i, false);
    lc.setIntensity(i, 6);
    lc.clearDisplay(i);
  }
*/

  pinMode(SW_pin, INPUT); //button pin
}

//This pretty much has the setup code..  And resetting any other values..
//Inside setup(), you can call "ClearAndReset()" so you don't have duplicate code.
void ClearAndReset()
{
	byte i = 0;
	while (i < 4) { //Loop to initiate and clear all displays
		lc.shutdown(i, false);
		lc.setIntensity(i, 6);
		lc.clearDisplay(i);
		i++;
	}
	a = 0;
	x = 7;
	y = 6;
	butpress = 1;
	dir = 0;
    sto_x = 0;
    leh = 4;
    sta = 1;
    gameOver = false;
}

void loop() {

	//let's check for gameOver.... if the game has ended, once they press a button, it just starts over immediately.
	//We could re work the loop() so that we do some kind of visual of clearing the screen, too, or instead...
	//With all my current mess of stuff here, though, I'll keep this simple.
	//We wait until the button is pressed.  Then once the button is RELEASED, it restarts.
	if (gameOver)
	{
		bool tmp_restart = false;
		while (tmp_restart == false) {
			//check for the HIGH press!
			if (digitalRead(SW_pin) == HIGH) {
				//we have the high press!!  Run our reset code!
				ClearAndReset();
				//now we want to wait until they've released the button!
				//This is probably pretty shitty.
				while (digitalRead(SW_pin) == HIGH) {
					//do nothing..
				}
				//OK, we've released the button.  time to restart.
				tmp_restart = true;
			}
		}
	}
	
  while (digitalRead(SW_pin) == LOW) { //while the button is not pressed
  	//Soo, I see what's going on here.  You're dropping a delay inside the while loop and at some point, you're 
  	//  going to press the button, which will kick us out of the while loop.
    delay(96 - 2 * ((a * 8) + (8 - y))); //variable delay to make it speed up the higher you go
    butpress = 1; //button lock variable on
    if (x < 0 || x > 6 + leh) { //if the platform is off the screen
      dir = abs(dir - 1); //switch the direction
    }
    if (dir == 0) { 
      x--; //incriment the platform right if the dir is 0
      lc.setLed(a, x + 1, y, 0); //erase the platform trail
      lc.setLed(a, x + 1, y + 1, 0);
    }
    else {
      x++; //incriment the platform left if the dir is 1
      lc.setLed(a, x - leh, y, 0); //erase the platform trail
      lc.setLed(a, x - leh, y + 1, 0);
    }

	//rewritten as a for loop.
	//I've never used LedControl...  but it seems like you set values on and off.. liike, once it's on,
	//  it stays on?  In that case, you're resetting values to 1 that don't need to be.  It's fine, and
	//  less confusing that just turning on the first or last (depending on direction)
	//The other interesting thing that you're doing is turning stuff on beyond the edge of the screen on
	//  the left and right?  Let's say our current location is 0. (left edge), and our platformWidth is 3.
	//  we're turning on 0, -1, -2. But -1 and -2 are off screen.
	//  Let's say our current location is 9, platformWidth of 3.  we're turning on 7, 8, 9.  8 and 9 are 
	//  off screen.
	byte tmp_loc;
    for (byte d = 0; d < leh; d++) {
		tmp_loc = x - d;
		//now we'll only draw when it's actually visually on screen.
		if (tmp_loc >= 0 && tmp_loc < SCREEN_WIDTH) {
			lc.setLed(a, tmp_loc, y, 1); //top pixel
			lc.setLed(a, tmp_loc, y + 1, 1); //bottom pixel
		}
    }
/*
    while (d < leh) { //loop to draw the platform based off the length
      lc.setLed(a, x - d, y, 1); //top pixel
      lc.setLed(a, x - d, y + 1, 1); //bottom pixel
      d++;
    }
    d = 0;
*/
  }

  //I think this should be where you adjust the Y value.  Not as a stand-alone on the loop..
  //It technically works, because you hold down the key (giving you a HIGH) long enough, so it skips the above
  //  while loop and adjusts the Y value.  But if you press and release the key fast enough, I think you'll
  //  get the mystery disappearance of your moving bar.. because it's being drawn on the -2 and -1 rows of the 
  //  panel, hehe..
  //Want to test it?  uncomment the "MYSTERY DISAPPEARANCE" code segment in readHIGH.
  if (y < 0) { //if y goes off the screen, warp to next sceen
    a++; //incriment screen address
    y = 6; //set y to bottom value
  }
  
  if (digitalRead(SW_pin) == HIGH && butpress == 1) { //activate once when button is pressed
    if (sta == 1) { //if its the first press of the game)
      sta = 0;
      sto_x = x;
    }
    y = y - 2; //move y up two pixels 
    butpress = 0;
    checkfall(); //check for misalignment
    //try it with checkfallExact() instead!
    
    //put your Y adjustment here, after you've done your checkfall (because your checkfall assumes the
    //  -2 value...
    //you could alternately do checkfall and THEN y = y -2... That's what I'd suggest, because the +2 and +3
    //  in checkfall is kind of confusing.
/*
    if (y < 0) { //if y goes off the screen, warp to next sceen
      a++; //incriment screen address
      y = 6; //set y to bottom value
    }
*/
	//MYSTERY_DISAPPEARANCE - for this to work, the check after checkfall has to be commented out..
	//All I'm doing is creating a delay, giving you up to 1 second to press and release the button.  During
	//  the next loop, the read will be LOW, but the Y value won't have been updated yet.
/*
	delay(1000);
*/
  }

	//And we can check here, or inside checkfall, I guess, for leh being less than 1... if leh is less than 1,
	//  then we lost..
	if (leh < 1) {
		gameOver = true;
	}

}

//Sooo, maybe I'm lost a bit here..  Am I missing something?
//I'm seeing this as:
//  a) No matter what, we only erase 1 platform piece, what ever is just off the edge of the tower, and 
//       it depends on which direction we're going.
//  b) If we're off by more than 1 from sto_x, there will be visual overhang, and there will be floating
//       vusuals.
//  c) Even though we only erase 1 thing visually, the new width of the platform is adjusted by how far
//       off we were.  And when we get below zero, the game keeps playing, but doesn't have anything to display.
//I understand that before we get here, we ALWAYS redraw the position of the platform (the remainder
//  of the if (read == LOW) after the delay call) to the next turn's position.  Soo, if we're CLOSE (off
//  by 1), it's easy to just say "clear the space left of our tower and clear the space right of our tower".
//  Because we now our tower's right edge position, and the current width of the platform.  And visually,
//  we've only gone 1 position off.  Easy.  Clear it.
//But if we're 2 off, we're kinda screwed.

//Soooo, the question is... how do you want it to perform?  If we are off by 2, does the platform shrink by 2?
//  Or does the platform shrink by 1, and we adjust the visual so that it looks like we were only off by 1?
//I mean, at what point to do you say "Look, buddy, you pressed the buttom when the platform wasn't even on 
//  the tower..... that's a fail..."  Or do you still just shrink it by 1?

//It looks like your current overhang-shift thing is just for the next round of tower edge indication.
void checkfall() {
  if (x != sto_x) { //If x is not equal to the previous x value
    lc.setLed(a, sto_x - leh, y + 2, 0); //erase the overhange
    lc.setLed(a, sto_x - leh, y + 3, 0);
    lc.setLed(a, sto_x + 1, y + 2, 0);
    lc.setLed(a, sto_x + 1, y + 3, 0);
    leh = leh - abs(x - sto_x); //subtract the difference in x from the length
    if (x > sto_x) { //if x overhangs to the left
      x = x - abs(x - sto_x); //shift x to be on the tower
      sto_x = x; //set previous x value
    }
  }
  sto_x = x; //set previous x value
}

//Here's another checkfall.
//This one clears exactly to where you are.  If you're off, you lose.
//This covers any lost game.  If our platform size is 1, then this catches the lost game.
void checkfallExact()
{
	if (x != sto_x) {
		//first off, let's check to see if we're way off..
		if (abs(x - sto_x) >= leh) {
			//we are completely off the tower.  We lose.  Might as well leave the visual of how bad they messed
			//  up to really rub it in their face...
			gameOver = true;
			return;
		}
		//OK, at this point we're CLOSE, but not quite..  we need to clear everything to the left of the tower
		//  and everything to the right of the tower.  This is an either-or thing.
		if (x < sto_x) {
			//use leh-1, since we're including X in our size... if X is 6, and size is 4, then platform is in 
			//  positions 3, 4, 5, 6.  6 - (4-1) = 3.  3 is the left-most position of our platform.
			for (byte i = x - (leh-1); i <= sto_x - leh; i++) {
				//let's ignore anything that's below zero.. cause it's off-screen..
				if (i >= 0) {
					//clear it!
					lc.setLed(a, i, y + 2, 0); //erase the overhange
					lc.setLed(a, i, y + 3, 0);
				}
			}
		}
		else {
			for (byte i = sto_x+1; i <= x; i++) {
				//let's ignore anything that's above our screen size.. cause it's off-screen..
				if (i < SCREEN_WIDTH) {
					//clear it!
					lc.setLed(a, i, y + 2, 0); //erase the overhange
					lc.setLed(a, i, y + 3, 0);
				}
			}
		}

		//OK, now we need to adjust our size of the platform..
		leh = leh - abs(x - sto_x);

		//And now adjust our X value or stopX value!
		//IF we went too far right, then we adjust our X value to stopX, and keep stopX where it is.
		//IF we went too far LEFT, then we adjust our stopX value to our X value
		if (x < sto_x) {
			sto_x = x;
		}
		else {
			x = sto_x;
		}
	}

	//x IS the same as stopX value.  Which means the platform lined up perfectly.  There's nothing to delete, and
	//  we don't need to update the stopX value.
}

//BELOW:  One way to make your actual code easier to work with and read as things get complicated.

//This removes your need to worry about stuff like current display and positioning in your actual code.
//Just build your actual game going from zero and up to displaycount * display_size, etc..
//So you'd use game position starting at 2, etc..  And it would just go up to 32 (or really screen height * 
//  screen_count)
//Personally, I'd start it at zero, and draw my second row at y+1.
//NOTE, I didn't change your stuff to support this immediately.. you'd have to mess around a bit.
//Personally, especially with LED grids, I find it a lot easier to work with x,y coordinates and then have some
//  translating thing that puts them into the correct place in the right panel.  I guess using LedControl kind
//  of handles that for you, though.   I have been using FastLED with WS2811 LEDs, etc..  and with that, you
//  define your array of LEDs, and need to know which one is tied to which grid location.
//This isn't reeally needed here.  But if you write your code so that it's independent of the LED placement, etc,
//  then it would be easier to change it for a different type of LEDs?  Maybe.. maybe not.
//Really, you'd probably use this more like:
/*
void DrawPixel(int in_x, int in_y, int value) {
	CalcYScreenPosition(in_y);
	lc.setLed(currentDisplay, in_x, positionY, value);
}
*/
//then everywhere in your code, you'd use DrawPixel() instead of your current lc.setLed() calls..
//And you wouldn't really need to use global variables to hold the currentDisplay value..

void CalcYScreenPosition(byte in_y)
{
	//we know that each screen is 8x8.  we have 4 screens.  Y value is essentually 0 - 31.
	//In reality, Y value is 6, 4, 2, 0.  with currentDisplay ("a") being 0, 1, 2, 3.
	//Let's say Y comes in as 4.  we need to get the values 0 for "a" and 4 for "y".
	//Let's say Y comes in as 10.  we need to get the values 1 for "a" and 6 for "y".
	//  in_y / SCREEN_HEIGHT.
	//  4 / 8 = 0.  these are byte or int values, so we get int results.  zero.
	//  10 / 8 = 1.
	//  31 / 8 = 3.   Perfect.  This gives us our current_display! 
	currentDisplay = in_y / SCREEN_HEIGHT;

	//now to get where in that display we are!  Since we are working our way UP, we subtract from our SCREEN_HEIGHT.
	//  But first we need the left over inside our current screen..
	//leftover = SCREEN_HEIGHT - (in_y - (currentDisplay * SCREEN_HEIGHT)).
	//if in_y is 4:   8 - (4 - (0 * 8)) = 8 - (4 - 0) = 8 - 4 = 4.  Well that's a shitty example, haha.
	//if in_y is 10:  8 - (10- (1 * 8)) = 8 - (10- 8) = 8 - 2 = 6.
	positionY = SCREEN_HEIGHT - (in_y - (currentDisplay * screen_HEIGHT));

	//and that's really all there is here.  You could do a return, but you have these values globally, so whatevs.
}

