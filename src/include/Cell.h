#pragma once

#include <stdbool.h>

#include "raylib/raylib.h"

typedef enum CellType {
    CELL_TYPE_EMPTY,
    CELL_TYPE_SAND,
    CELL_TYPE_WATER,
    CELL_TYPE_FIRE,
    CELL_TYPE_SMOKE,
    CELL_TYPE_STONE,
} CellType;

typedef struct Cell {
    CellType type;
    Vector2 vel;
    float subY;    // frac. fall accumulator
    int life;
    int maxLife;
    int brightness;
    bool updated;
    bool asleep;
} Cell;

typedef void (*CellUpdateFuncion)( Cell *grid, int row, int col, int rows, int cols, float delta );
typedef void (*CellDrawFuncion)( Cell *cell, int row, int col );
typedef bool (*CellMoveFuncion)( Cell *grid, int row, int col, int rows, int cols );

void resetCells( Cell *grid, int rows, int cols );
void updateCell( Cell *grid, int row, int col, int rows, int cols, float delta );
void drawCell( Cell *grid, int row, int col, int rows, int cols );

void spawnCell( Cell *cell, CellType type );
void wakeNeighbors( Cell *grid, int row, int col, int rows, int cols );
