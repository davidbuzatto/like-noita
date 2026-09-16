#include <stdbool.h>

#include "raylib/raylib.h"

#include "Cell.h"

static void updateSand( Cell *grid, int row, int col, int rows, int cols );
static void updateWater( Cell *grid, int row, int col, int rows, int cols );
static void updateFire( Cell *grid, int row, int col, int rows, int cols );
static void updateSmoke( Cell *grid, int row, int col, int rows, int cols );
static void updateStone( Cell *grid, int row, int col, int rows, int cols );

static bool moveLeft( Cell *grid, int row, int col, int rows, int cols );
static bool moveRight( Cell *grid, int row, int col, int rows, int cols );
static bool moveUp( Cell *grid, int row, int col, int rows, int cols );
static bool moveUpLeft( Cell *grid, int row, int col, int rows, int cols );
static bool moveUpRight( Cell *grid, int row, int col, int rows, int cols );
static bool moveDown( Cell *grid, int row, int col, int rows, int cols );
static bool moveDownLeft( Cell *grid, int row, int col, int rows, int cols );
static bool moveDownRight( Cell *grid, int row, int col, int rows, int cols );

static void swapCell( Cell *grid, int pos1, int pos2 );
static bool isCellPositionValid( int row, int col, int rows, int cols );

static void drawSand( int row, int col );
static void drawWater( int row, int col );
static void drawFire( int row, int col );
static void drawSmoke( int row, int col );
static void drawStone( int row, int col );

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

void resetCells( Cell *grid, int rows, int cols ) {
    for ( int row = 0; row < rows; row++ ) {
        for ( int col = 0; col < cols; col++ ) {
            grid[row * cols + col].updated = false;
        }
    }
}

void updateCell( Cell *grid, int row, int col, int rows, int cols ) {

    Cell *cell = &grid[row * cols + col];

    if ( cell->type != CELL_TYPE_EMPTY && !cell->updated ) {
        cell->updated = true;
        updateTable[cell->type]( grid, row, col, rows, cols );
    }

}

void drawCell( Cell *grid, int row, int col, int rows, int cols ) {

    Cell *cell = &grid[row * cols + col];

    if ( cell->type != CELL_TYPE_EMPTY ) {
        drawTable[cell->type]( row, col );
    }

}

static void updateSand( Cell *grid, int row, int col, int rows, int cols ) {

    if ( moveDown( grid, row, col, rows, cols ) ) return;

    if ( GetRandomValue( 0, 1 ) == 0 ) {
        if ( moveDownLeft( grid, row, col, rows, cols ) ) return;
    } else {
        if ( moveDownRight( grid, row, col, rows, cols ) ) return;
    }
    
}

static void updateWater( Cell *grid, int row, int col, int rows, int cols ) {
    
    if ( moveDown( grid, row, col, rows, cols ) ) return;

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

}

static void updateFire( Cell *grid, int row, int col, int rows, int cols ) {
    
    Cell *cell = &grid[row * cols + col];
    cell->life--;

    if ( cell->life <= 0 ) {
        cell->type = CELL_TYPE_SMOKE;
        cell->life = GetRandomValue( 150, 400 );
        return;
    }

}

static void updateSmoke( Cell *grid, int row, int col, int rows, int cols ) {
    
    Cell *cell = &grid[row * cols + col];
    cell->life--;

    if ( cell->life == 0 ) {
        cell->type = CELL_TYPE_EMPTY;
        return;
    }

    if ( moveUp( grid, row, col, rows, cols ) ) return;

    if ( GetRandomValue( 0, 1 ) == 0 ) {
        if ( moveUpLeft( grid, row, col, rows, cols ) ) return;
    } else {
        if ( moveUpRight( grid, row, col, rows, cols ) ) return;
    }

}

static void updateStone( Cell *grid, int row, int col, int rows, int cols ) {
    // don't do anything!
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

static void swapCell( Cell *grid, int pos1, int pos2 ) {
    Cell c = grid[pos1];
    grid[pos1] = grid[pos2];
    grid[pos2] = c;
}

static bool isCellPositionValid( int row, int col, int rows, int cols ) {
    return row >= 0 && row < rows && col >= 0 && col < cols;
}

static void drawSand( int row, int col ) {
    DrawPixel( col, row, ORANGE );
}

static void drawWater( int row, int col ) {
    DrawPixel( col, row, BLUE );
}

static void drawFire( int row, int col ) {
    DrawPixel( col, row, RED );
}

static void drawSmoke( int row, int col ) {
    DrawPixel( col, row, LIGHTGRAY );
}

static void drawStone( int row, int col ) {
    DrawPixel( col, row, DARKGRAY );
}
