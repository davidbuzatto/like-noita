#include <stdbool.h>

#include "raylib/raylib.h"

#include "Cell.h"

#define GRAVITY 800.0f
#define MAX_FALL_SPEED 500.0f

static void updateSand( Cell *grid, int row, int col, int rows, int cols, float delta );
static void updateWater( Cell *grid, int row, int col, int rows, int cols, float delta );
static void updateFire( Cell *grid, int row, int col, int rows, int cols, float delta );
static void updateSmoke( Cell *grid, int row, int col, int rows, int cols, float delta );
static void updateStone( Cell *grid, int row, int col, int rows, int cols, float delta );

static bool moveLeft( Cell *grid, int row, int col, int rows, int cols );
static bool moveRight( Cell *grid, int row, int col, int rows, int cols );
static bool moveUp( Cell *grid, int row, int col, int rows, int cols );
static bool moveUpLeft( Cell *grid, int row, int col, int rows, int cols );
static bool moveUpRight( Cell *grid, int row, int col, int rows, int cols );
static bool moveDown( Cell *grid, int row, int col, int rows, int cols );
static bool moveDownLeft( Cell *grid, int row, int col, int rows, int cols );
static bool moveDownRight( Cell *grid, int row, int col, int rows, int cols );

static int computeFallSteps( Cell *cell, float delta );

static void swapCell( Cell *grid, int pos1, int pos2 );
static bool isCellPositionValid( int row, int col, int rows, int cols );

static void drawSand( Cell *cell, int row, int col );
static void drawWater( Cell *cell, int row, int col );
static void drawFire( Cell *cell, int row, int col );
static void drawSmoke( Cell *cell, int row, int col );
static void drawStone( Cell *cell, int row, int col );

static CellUpdateFuncion updateTable[] = {
    [CELL_TYPE_SAND]  = updateSand,
    [CELL_TYPE_WATER] = updateWater,
    [CELL_TYPE_FIRE]  = updateFire,
    [CELL_TYPE_SMOKE] = updateSmoke,
    [CELL_TYPE_STONE] = updateStone,
};

static CellDrawFuncion drawTable[] = {
    [CELL_TYPE_SAND]  = drawSand,
    [CELL_TYPE_WATER] = drawWater,
    [CELL_TYPE_FIRE]  = drawFire,
    [CELL_TYPE_SMOKE] = drawSmoke,
    [CELL_TYPE_STONE] = drawStone,
};

static CellMoveFuncion smokeMoveTable[] = {
    moveUp, moveUpLeft, moveUpRight, moveLeft, moveRight
};

void resetCells( Cell *grid, int rows, int cols ) {
    for ( int row = 0; row < rows; row++ ) {
        for ( int col = 0; col < cols; col++ ) {
            grid[row * cols + col].updated = false;
        }
    }
}

void updateCell( Cell *grid, int row, int col, int rows, int cols, float delta ) {

    Cell *cell = &grid[row * cols + col];

    if ( cell->type != CELL_TYPE_EMPTY && !cell->updated ) {
        cell->updated = true;
        updateTable[cell->type]( grid, row, col, rows, cols, delta );
    }

}

void drawCell( Cell *grid, int row, int col, int rows, int cols ) {

    Cell *cell = &grid[row * cols + col];

    if ( cell->type != CELL_TYPE_EMPTY ) {
        drawTable[cell->type]( cell, row, col );
    }

}

static void updateSand( Cell *grid, int row, int col, int rows, int cols, float delta ) {

    Cell *cell = &grid[row * cols + col];
    int steps = computeFallSteps( cell, delta );

    int movedCount = 0;
    for ( int i = 0; i < steps; i++ ) {
        if ( moveDown( grid, row, col, rows, cols ) ) {
            row++;
            movedCount++;
        } else {
            break;
        }
    }

    if ( movedCount == steps ) return; // doesn't hit anything

    // hit something, tries the diagonals
    if ( GetRandomValue( 0, 1 ) == 0 ) {
        if ( moveDownLeft( grid, row, col, rows, cols ) ) return;
    } else {
        if ( moveDownRight( grid, row, col, rows, cols ) ) return;
    }

    // stopped
    Cell *restingCell = &grid[row * cols + col];
    restingCell->vel.y = 0.0f;
    restingCell->subY = 0.0f;
    
}

static void updateWater( Cell *grid, int row, int col, int rows, int cols, float delta ) {
    
    Cell *cell = &grid[row * cols + col];
    int steps = computeFallSteps( cell, delta );

    int movedCount = 0;
    for ( int i = 0; i < steps; i++ ) {
        if ( moveDown( grid, row, col, rows, cols ) ) {
            row++;
            movedCount++;
        } else {
            break;
        }
    }

    if ( movedCount == steps ) return;

    if ( GetRandomValue( 0, 1 ) == 0 ) {
        if ( moveDownLeft( grid, row, col, rows, cols ) ) return;
    } else {
        if ( moveDownRight( grid, row, col, rows, cols ) ) return;
    }

    if ( GetRandomValue( 0, 1 ) == 0 ) {
        if ( moveLeft( grid, row, col, rows, cols ) ) return;
    } else {
        if ( moveRight( grid, row, col, rows, cols ) ) return;
    }

    Cell *restingCell = &grid[row * cols + col];
    restingCell->vel.y = 0.0f;
    restingCell->subY = 0.0f;

}

static void updateFire( Cell *grid, int row, int col, int rows, int cols, float delta ) {
    
    Cell *cell = &grid[row * cols + col];
    cell->life--;

    if ( cell->life <= 0 ) {
        cell->type = CELL_TYPE_SMOKE;
        cell->life = GetRandomValue( 150, 400 );
        cell->brightness = GetRandomValue( -15, 15 );
        return;
    }

}

static void updateSmoke( Cell *grid, int row, int col, int rows, int cols, float delta ) {
    
    Cell *cell = &grid[row * cols + col];
    cell->life--;

    if ( cell->life <= 0 ) {
        cell->type = CELL_TYPE_EMPTY;
        return;
    }

    // shuffles move table (Fisher-Yates)
    for ( int i = 4; i > 0; i-- ) {
        int j = GetRandomValue( 0, i );
        CellMoveFuncion t = smokeMoveTable[i];
        smokeMoveTable[i] = smokeMoveTable[j];
        smokeMoveTable[j] = t;
    }

    for ( int i = 0; i < 5; i++ ) {
        if ( smokeMoveTable[i]( grid, row, col, rows, cols ) ) return;
    }

}

static void updateStone( Cell *grid, int row, int col, int rows, int cols, float delta ) {
    // do nothing!
}

static bool moveLeft( Cell *grid, int row, int col, int rows, int cols ) {

    int pos = row * cols + col;
    
    int nRow = row;
    int nCol = col - 1;
    int nPos = nRow * cols + nCol;

    if ( isCellPositionValid( nRow, nCol, rows, cols ) ) {
        if ( grid[nPos].type == CELL_TYPE_EMPTY ) {
            swapCell( grid, pos, nPos );
            return true;
        }
    }

    return false;
}

static bool moveRight( Cell *grid, int row, int col, int rows, int cols ) {

    int pos = row * cols + col;
    
    int nRow = row;
    int nCol = col + 1;
    int nPos = nRow * cols + nCol;

    if ( isCellPositionValid( nRow, nCol, rows, cols ) ) {
        if ( grid[nPos].type == CELL_TYPE_EMPTY ) {
            swapCell( grid, pos, nPos );
            return true;
        }
    }

    return false;

}

static bool moveUp( Cell *grid, int row, int col, int rows, int cols ) {

    int pos = row * cols + col;
    
    int nRow = row - 1;
    int nCol = col;
    int nPos = nRow * cols + nCol;

    if ( isCellPositionValid( nRow, nCol, rows, cols ) ) {
        if ( grid[nPos].type == CELL_TYPE_EMPTY ) {
            swapCell( grid, pos, nPos );
            return true;
        }
    }

    return false;

}

static bool moveUpLeft( Cell *grid, int row, int col, int rows, int cols ) {

    int pos = row * cols + col;
    
    int nRow = row - 1;
    int nCol = col - 1;
    int nPos = nRow * cols + nCol;

    if ( isCellPositionValid( nRow, nCol, rows, cols ) ) {
        if ( grid[nPos].type == CELL_TYPE_EMPTY ) {
            swapCell( grid, pos, nPos );
            return true;
        }
    }

    return false;

}

static bool moveUpRight( Cell *grid, int row, int col, int rows, int cols ) {

    int pos = row * cols + col;
    
    int nRow = row - 1;
    int nCol = col + 1;
    int nPos = nRow * cols + nCol;

    if ( isCellPositionValid( nRow, nCol, rows, cols ) ) {
        if ( grid[nPos].type == CELL_TYPE_EMPTY ) {
            swapCell( grid, pos, nPos );
            return true;
        }
    }

    return false;

}

static bool moveDown( Cell *grid, int row, int col, int rows, int cols ) {

    int pos = row * cols + col;

    int nRow = row + 1;
    int nCol = col;
    int nPos = nRow * cols + nCol;

    if ( isCellPositionValid( nRow, nCol, rows, cols ) ) {
        if ( grid[nPos].type == CELL_TYPE_EMPTY ) {
            swapCell( grid, pos, nPos );
            return true;
        }
    }

    return false;

}

static bool moveDownLeft( Cell *grid, int row, int col, int rows, int cols ) {

    int pos = row * cols + col;
    
    int nRow = row + 1;
    int nCol = col - 1;
    int nPos = nRow * cols + nCol;

    if ( isCellPositionValid( nRow, nCol, rows, cols ) ) {
        if ( grid[nPos].type == CELL_TYPE_EMPTY ) {
            swapCell( grid, pos, nPos );
            return true;
        }
    }

    return false;

}

static bool moveDownRight( Cell *grid, int row, int col, int rows, int cols ) {

    int pos = row * cols + col;
    
    int nRow = row + 1;
    int nCol = col + 1;
    int nPos = nRow * cols + nCol;

    if ( isCellPositionValid( nRow, nCol, rows, cols ) ) {
        if ( grid[nPos].type == CELL_TYPE_EMPTY ) {
            swapCell( grid, pos, nPos );
            return true;
        }
    }

    return false;

}

static int computeFallSteps( Cell *cell, float delta ) {

    cell->vel.y += GRAVITY * delta;
    if ( cell->vel.y > MAX_FALL_SPEED ) {
        cell->vel.y = MAX_FALL_SPEED;
    }

    cell->subY += cell->vel.y * delta;

    int steps = (int) cell->subY;
    cell->subY -= steps;

    return steps;

}

static void swapCell( Cell *grid, int pos1, int pos2 ) {
    Cell c = grid[pos1];
    grid[pos1] = grid[pos2];
    grid[pos2] = c;
}

static bool isCellPositionValid( int row, int col, int rows, int cols ) {
    return row >= 0 && row < rows && col >= 0 && col < cols;
}

static void drawSand( Cell *cell, int row, int col ) {
    DrawPixel( col, row, ColorBrightness( ORANGE, cell->brightness / 100.0f ) );
}

static void drawWater( Cell *cell, int row, int col ) {
    DrawPixel( col, row, ColorBrightness( BLUE, cell->brightness / 100.0f ) );
}

static void drawFire( Cell *cell, int row, int col ) {
    DrawPixel( col, row, ColorBrightness( RED, cell->brightness / 100.0f ) );
}

static void drawSmoke( Cell *cell, int row, int col ) {
    DrawPixel( col, row, ColorBrightness( LIGHTGRAY, cell->brightness / 100.0f ) );
}

static void drawStone( Cell *cell, int row, int col ) {
    DrawPixel( col, row, ColorBrightness( DARKGRAY, cell->brightness / 100.0f ) );
}

void spawnCell( Cell *cell, CellType type ) {

    cell->type = type;
    cell->vel = (Vector2) { 0, 0 };
    cell->subY = 0;
    cell->brightness = GetRandomValue( -15, 15 );

    switch ( type ) {
        case CELL_TYPE_FIRE:
            cell->life = GetRandomValue( 200, 500 );
            break;
        case CELL_TYPE_SMOKE:
            cell->life = GetRandomValue( 150, 400 );
            break;
        default:
            cell->life = 0;
            break;
    }

}
