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
#include "raylib/raymath.h"
//#define RAYGUI_IMPLEMENTATION    // to use raygui, comment these three lines.
//#include "raylib/raygui.h"       // other compilation units must only include
//#undef RAYGUI_IMPLEMENTATION     // raygui.h

#include "Cell.h"
#include "GameWorld.h"
#include "Macros.h"
#include "ResourceManager.h"

/**
 * @brief Creates a dinamically allocated GameWorld struct instance.
 */
GameWorld *createGameWorld( void ) {

    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );

    gw->rows = GetScreenHeight();
    gw->cols = GetScreenWidth();

    gw->grid = (Cell*) calloc( gw->rows * gw->cols, sizeof( Cell ) );

    // test data
    int size = 80;
    for ( int i = 200; i < 200 + size; i++ ) {

        int m = 40;

        for ( int j = 0; j < size; j++ ) {

            int pos = i * gw->cols + j + m;
            gw->grid[pos].type = CELL_TYPE_SAND;
            gw->grid[pos].brightness = GetRandomValue( -15, 15 );

            pos += size * 2;
            gw->grid[pos].type = CELL_TYPE_WATER;
            gw->grid[pos].brightness = GetRandomValue( -15, 15 );

            pos += size * 2;
            gw->grid[pos].type = CELL_TYPE_FIRE;
            gw->grid[pos].life = GetRandomValue( 200, 500 );
            gw->grid[pos].brightness = GetRandomValue( -15, 15 );

            pos += size * 2;
            gw->grid[pos].type = CELL_TYPE_SMOKE;
            gw->grid[pos].life = GetRandomValue( 150, 400 );
            gw->grid[pos].brightness = GetRandomValue( -15, 15 );

            pos += size * 2;
            gw->grid[pos].type = CELL_TYPE_STONE;
            gw->grid[pos].brightness = GetRandomValue( -15, 15 );

        }

    }

    size = 20;
    for ( int i = 100; i < 100 + size; i++ ) {
        int m = 400;
        for ( int j = 0; j < size * 5; j++ ) {
            int pos = i * gw->cols + j + m;
            gw->grid[pos].type = CELL_TYPE_STONE;
            gw->grid[pos].brightness = GetRandomValue( -15, 15 );
        }
    }

    return gw;

}

/**
 * @brief Destroys a GameWindow object and its dependecies.
 */
void destroyGameWorld( GameWorld *gw ) {
    if ( gw != NULL ) {
        free( gw );
    }
}

/**
 * @brief Reads user input and updates the state of the game.
 */
void updateGameWorld( GameWorld *gw, float delta ) {

    resetCells( gw->grid, gw->rows, gw->cols );

    for ( int row = gw->rows - 1; row >= 0; row-- ) {

        if ( GetRandomValue( 0, 1 ) == 0 ) {
            for ( int col = 0; col < gw->cols; col++ ) {
                updateCell( gw->grid, row, col, gw->rows, gw->cols );
            }
        } else {
            for ( int col = gw->cols - 1; col >= 0; col-- ) {
                updateCell( gw->grid, row, col, gw->rows, gw->cols );
            }
        }

    }

}

/**
 * @brief Draws the state of the game.
 */
void drawGameWorld( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( BLACK );

    for ( int row = 0; row < gw->rows; row++ ) {
        for ( int col = 0; col < gw->cols; col++ ) {
            drawCell( gw->grid, row, col, gw->rows, gw->cols );
        }
    }

    DrawFPS( 20, 20 );

    EndDrawing();

}
