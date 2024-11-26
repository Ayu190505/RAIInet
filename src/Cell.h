#ifndef CELL_H
#define CELL_H
#include <string>
#include <memory>

class Cell {
    int row;
    int col;
    int size;
    char c;
    int whichPlayersFirewall = 0; // 0 refers to no player
    int whichPlayersServerPort = 0; // same
    bool locked;
    public:
    Cell() {}
    Cell(int row, int col, int size);
    Cell(const Cell &other);
    bool hasFirewall() const;
    bool hasOwnFirewall(int playerNumber) const;
    bool hasOpponnentFirewall(int playerNumber) const;
    bool isServerPort() const;
    bool isOwnServerPort(int playerNumber) const;
    bool isOpponentServerPort(int playerNumber) const;
    bool isEmpty() const;
    bool isLocked() const;
    int getRow() const;
    int getCol() const;
    int getPlayersServerPort() const;
    int getPlayersFirewall() const;
    char getContent() const;
    void setContent(char content);
    void setFirewall(int playerNumber, bool status);
};

#endif
