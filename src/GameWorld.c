/**
 * @file GameWorld.c
 * @author Prof. Dr. David Buzatto
 * @brief GameWorld implementation.
 * 
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "raylib/raylib.h"
//#include "raylib/raymath.h"
//#define RAYGUI_IMPLEMENTATION    // to use raygui, comment these three lines.
//#include "raylib/raygui.h"       // other compilation units must only include
//#undef RAYGUI_IMPLEMENTATION     // raygui.h

#include "GameWorld.h"
#include "Macros.h"
#include "ResourceManager.h"

static const int GRAVITY = 1000;
static const float MAX_FALL_SPEED = 600.0f;

static void resetNextGridFrame( GameWorld *gw );

static void updateCurrentGridFrame( GameWorld *gw, float delta );
static void drawCurrentGridFrame( GameWorld *gw );

static bool checkBounds( int row, int col, int rows, int cols );

/**
 * @brief Creates a dinamically allocated GameWorld struct instance.
 */
GameWorld *createGameWorld( void ) {

    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );

    gw->rows = GetScreenHeight();
    gw->cols = GetScreenWidth();

    gw->gridFrame1 = (int*) malloc( gw->rows * gw->cols * sizeof( int ) );
    gw->gridFrame2 = (int*) malloc( gw->rows * gw->cols * sizeof( int ) );
    gw->gridFrameVels = (float*) malloc( gw->rows * gw->cols * sizeof( float ) );

    gw->currentGridFrame = gw->gridFrame1;

    int size = 50;
    for ( int row = 0; row < size; row++ ) {
        for ( int col = 0; col < size; col++ ) {
            int pos = row * gw->cols + col;
            gw->currentGridFrame[pos] = 1;
            gw->gridFrameVels[pos] = 100.0f;
        }
    }

    return gw;

}

/**
 * @brief Destroys a GameWindow object and its dependecies.
 */
void destroyGameWorld( GameWorld *gw ) {
    if ( gw != NULL ) {
        free( gw->gridFrame1 );
        free( gw->gridFrame2 );
        free( gw->gridFrameVels );
        free( gw );
    }
}

/**
 * @brief Reads user input and updates the state of the game.
 */
void updateGameWorld( GameWorld *gw, float delta ) {

    resetNextGridFrame( gw );
    updateCurrentGridFrame( gw, delta );

}

/**
 * @brief Draws the state of the game.
 */
void drawGameWorld( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( BLACK );

    drawCurrentGridFrame( gw );

    DrawFPS( 20, 20 );

    EndDrawing();

}

static void resetNextGridFrame( GameWorld *gw ) {

    int *currentGridFrame = gw->currentGridFrame;
    int *nextGridFrame;

    if ( gw->currentGridFrame == gw->gridFrame1 ) {
        nextGridFrame = gw->gridFrame2;
    } else {
        nextGridFrame = gw->gridFrame1;
    }

    for ( int row = 0; row < gw->rows; row++ ) {
        for ( int col = 0; col < gw->cols; col++ ) {
            int pos = row * gw->cols + col;
            nextGridFrame[pos] = 0;
        }
    }
}

static void updateCurrentGridFrame( GameWorld *gw, float delta ) {

    int *currentGridFrame = gw->currentGridFrame;
    int *nextGridFrame;

    if ( gw->currentGridFrame == gw->gridFrame1 ) {
        nextGridFrame = gw->gridFrame2;
    } else {
        nextGridFrame = gw->gridFrame1;
    }

    for ( int row = gw->rows - 1; row >= 0; row-- ) {
        for ( int col = 0; col < gw->cols; col++ ) {

            int pos = row * gw->cols + col;

            int nextRow = row + (int) ( gw->gridFrameVels[pos] * delta );
            int nextCol = col;
            int nextPos = nextRow * gw->cols + nextCol;

            int neighborRow = row + 1;
            int neighborCol = col;
            int neighborPos = neighborRow * gw->cols + neighborCol;

            if ( checkBounds( neighborRow, neighborCol, gw->rows, gw->cols ) ) {
                if ( gw->currentGridFrame[neighborPos] == 1 ) {
                    return;
                }
            }

            float nextVel = gw->gridFrameVels[pos];
            if ( nextVel > MAX_FALL_SPEED ) {
                nextVel = MAX_FALL_SPEED;
            }

            switch ( currentGridFrame[pos] ) {

                case 1:

                    // check boundaries
                    if ( checkBounds( nextRow, nextCol, gw->rows, gw->cols ) ) {
                        nextGridFrame[nextPos] = 1;
                        gw->gridFrameVels[nextPos] = nextVel;
                    } else if ( nextRow >= gw->rows ) {
                        nextGridFrame[pos] = 1;
                        gw->gridFrameVels[pos] = 0.0f;
                    }

                    break;

                default:
                    break;

            }

        }
    }

    gw->currentGridFrame = nextGridFrame;

}

static void drawCurrentGridFrame( GameWorld *gw ) {

    int *currentGridFrame = gw->currentGridFrame;

    for ( int row = 0; row < gw->rows; row++ ) {
        for ( int col = 0; col < gw->cols; col++ ) {
            int pos = row * gw->cols + col;
            switch ( currentGridFrame[pos] ) {
                case 1:
                    DrawPixel( col, row, BLUE );
                    break;
                default:
                    break;
            }
        }
    }

}

static bool checkBounds( int row, int col, int rows, int cols ) {
    return row >= 0 && row < rows && col >= 0 && col < cols;
}