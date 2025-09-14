#pragma once

#define SCREEN_WIDTH        128 // OLED display width, in pixels
#define SCREEN_HEIGHT       64  // OLED display height, in pixels
#define DISPLAY_RESET_PIN   -1 

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




#define COLOR_WHITE   SH110X_WHITE
#define COLOR_BLACK   SH110X_BLACK

// TODO: implement the right constructors, so DISPLAY_CONSTRUCTOR will become valid.
// #define DISPLAY_CONSTRUCTOR Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, DISPLAY_RESET_PIN)
#define DISPLAY_CONSTRUCTOR Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, DISPLAY_RESET_PIN)
