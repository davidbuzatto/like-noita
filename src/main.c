/**
 * @file main.c
 * @author Prof. Dr. David Buzatto
 * @brief Simple particle simulator "Like" the game Noita.
 * 
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "GameWindow.h"

int main( void ) {

    GameWindow *gameWindow = createGameWindow(
        800,             // width
        450,             // height
        "Like Noita",    // title
        60,              // target FPS
        false,           // antialiasing
        false,           // resizable
        false,           // full screen
        false,           // undecorated
        false,           // always on top
        false,           // invisible background
        false,           // always run
        false,           // load resources
        false            // init audio
    );

    initGameWindow( gameWindow );

    return 0;

}