#pragma once

#ifndef DISPLAY_CHIP
    #define DISPLAY_CHIP        SH1106G
#endif

// ToDo include the right library here... currently only one is in place.
#include <Adafruit_SH110X.h>

/**
 * Build the class name from DISPLAY_CHIP to "Adafruit_<chip>"
 */
#define JOIN_(X,Y) X##Y
#define JOIN(X,Y) JOIN_(X,Y)
#define DISPLAY_CHIP_DRIVER_CLASS JOIN(Adafruit_,DISPLAY_CHIP)


// #define JOIN_DEFINES(a,b) a ## b

// #define DISPLAY_CHIP_DRIVER  JOIN_DEFINES(Adafruit_ , DISPLAY_CHIP)

#define DISPLAY_RESET_PIN   -1 
#define SCREEN_WIDTH        128 // OLED display width, in pixels
#define SCREEN_HEIGHT       64  // OLED display height, in pixels

// Possible number of characters in a line (with sizes 1,2...)
/*
#define SCREEN_LINE_CHARS   21
#define SCREEN_LINE_CHARS_1 SCREEN_LINE_CHARS
#define SCREEN_LINE_CHARS_2 10     
#define SCREEN_LINES         6
#define SCREEN_LINES_1       SCREEN_LINES
#define SCREEN_LINES_2       4
*/

#define SCREEN_TEXT_WHITE   SH110X_WHITE

// TODO: implement the right constructors here, so DISPLAY_CONSTRUCTOR will become valid.
// #define DISPLAY_CONSTRUCTOR Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, DISPLAY_RESET_PIN)
#define DISPLAY_CONSTRUCTOR Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, DISPLAY_RESET_PIN)
