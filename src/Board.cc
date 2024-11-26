#include "Board.h"
#include "Err.h"
#include <stdexcept>
#include <iostream>

Board::Board(int numRow) : size{numRow} {
    grid.resize(size);
    for (int i = 0; i < size; ++i) {
        grid[i].resize(size);
        for (int j = 0; j < size; ++j) {
            Cell c{i, j, size};
            grid[i][j] = c;
        }
    }
}

Cell& Board::getCell(int row, int col){
    if (row < size && col < size) {
        return grid[row][col];
    }
    throw std::out_of_range(Err::invalidCoordinates);
}

int Board::getSize() const {
    return size;
}

