#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include "Cell.h"

class Board {
private:
    std::vector<std::vector<Cell>> grid;
    int size;
public:
    Board(int numRow);

    // getter
    int getSize() const;

    // other
    Cell& getCell(int row, int col);
};

#endif

