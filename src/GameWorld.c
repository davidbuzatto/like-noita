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

static void createTest( GameWorld *gw );
static void handleMouseInput( GameWorld *gw );
static void drawHud( GameWorld *gw );

/**
 * @brief Creates a dinamically allocated GameWorld struct instance.
 */
GameWorld *createGameWorld( void ) {

    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );

    gw->rows = GetScreenHeight();
    gw->cols = GetScreenWidth();

    gw->grid = (Cell*) calloc( gw->rows * gw->cols, sizeof( Cell ) );

    gw->selectedType = CELL_TYPE_SAND;
    gw->brushRadius = 5;

    //createTest( gw );

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

    handleMouseInput( gw );

    resetCells( gw->grid, gw->rows, gw->cols );

    for ( int row = gw->rows - 1; row >= 0; row-- ) {

        if ( GetRandomValue( 0, 1 ) == 0 ) {
            for ( int col = 0; col < gw->cols; col++ ) {
                updateCell( gw->grid, row, col, gw->rows, gw->cols, delta );
            }
        } else {
            for ( int col = gw->cols - 1; col >= 0; col-- ) {
                updateCell( gw->grid, row, col, gw->rows, gw->cols, delta );
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

    drawHud( gw );

    //DrawFPS( 20, 20 );
    EndDrawing();

}

static void createTest( GameWorld *gw ) {

    // test data
    int size = 80;
    for ( int i = 200; i < 200 + size; i++ ) {

        int m = 40;

        for ( int j = 0; j < size; j++ ) {

            int pos = i * gw->cols + j + m;
            spawnCell( &gw->grid[pos], CELL_TYPE_SAND );

            pos += size * 2;
            spawnCell( &gw->grid[pos], CELL_TYPE_WATER );

            pos += size * 2;
            spawnCell( &gw->grid[pos], CELL_TYPE_FIRE );

            pos += size * 2;
            spawnCell( &gw->grid[pos], CELL_TYPE_SMOKE );

            pos += size * 2;
            spawnCell( &gw->grid[pos], CELL_TYPE_STONE );

        }

    }

    size = 20;
    for ( int i = 100; i < 100 + size; i++ ) {
        int m = 400;
        for ( int j = 0; j < size * 5; j++ ) {
            int pos = i * gw->cols + j + m;
            spawnCell( &gw->grid[pos], CELL_TYPE_STONE );
        }
    }

}

static void handleMouseInput( GameWorld *gw ) {

    if ( IsKeyPressed( KEY_ONE ) )   gw->selectedType = CELL_TYPE_SAND;
    if ( IsKeyPressed( KEY_TWO ) )   gw->selectedType = CELL_TYPE_WATER;
    if ( IsKeyPressed( KEY_THREE ) ) gw->selectedType = CELL_TYPE_FIRE;
    if ( IsKeyPressed( KEY_FOUR ) )  gw->selectedType = CELL_TYPE_SMOKE;
    if ( IsKeyPressed( KEY_FIVE ) )  gw->selectedType = CELL_TYPE_STONE;
    if ( IsKeyPressed( KEY_ZERO ) )  gw->selectedType = CELL_TYPE_EMPTY;

    float wheel = GetMouseWheelMove();
    if ( wheel != 0 ) {
        gw->brushRadius += (int) wheel;
        if ( gw->brushRadius < 1 )  gw->brushRadius = 1;
        if ( gw->brushRadius > 50 ) gw->brushRadius = 50;
    }

    if ( !IsMouseButtonDown( MOUSE_BUTTON_LEFT ) ) {
        return;
    }

    Vector2 mouse = GetMousePosition();
    int centerRow = (int) mouse.y;
    int centerCol = (int) mouse.x;
    int r = gw->brushRadius;

    for ( int dRow = -r; dRow <= r; dRow++ ) {

        for ( int dCol = -r; dCol <= r; dCol++ ) {

            // is inside circle
            if ( dRow * dRow + dCol * dCol > r * r ) {
                continue;
            }

            int row = centerRow + dRow;
            int col = centerCol + dCol;

            // out of bounds
            if ( row < 0 || row >= gw->rows || col < 0 || col >= gw->cols ) {
                continue;
            }

            int pos = row * gw->cols + col;

            // clean
            if ( gw->selectedType == CELL_TYPE_EMPTY ) {
                gw->grid[pos].type = CELL_TYPE_EMPTY;
                wakeNeighbors( gw->grid, row, col, gw->rows, gw->cols );
                continue;
            }

            // doesn't destroy an existing cell creating new ones
            if ( gw->grid[pos].type != CELL_TYPE_EMPTY ) {
                continue;
            }

            spawnCell( &gw->grid[pos], gw->selectedType );
            wakeNeighbors( gw->grid, row, col, gw->rows, gw->cols );

        }

    }

}

static void drawHud( GameWorld *gw ) {

    int x = 10;
    int y = 10;
    int s = 20;
    Color textColor = RAYWHITE;

    switch ( gw->selectedType ) {
        case 0: DrawText( "Remove", x, y, s, textColor ); break;
        case 1: DrawText( "Sand",   x, y, s, textColor ); break;
        case 2: DrawText( "Water",  x, y, s, textColor ); break;
        case 3: DrawText( "Fire",   x, y, s, textColor ); break;
        case 4: DrawText( "Smoke",  x, y, s, textColor ); break;
        case 5: DrawText( "Stone",  x, y, s, textColor ); break;
        default: break;
    }

    DrawCircleLinesV( GetMousePosition(), gw->brushRadius, RAYWHITE );

}
