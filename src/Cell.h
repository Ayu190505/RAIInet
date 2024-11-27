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
    
    bool warpActive = false;
    int warpRow = -1;
    int warpCol = -1;

    bool imprisonActive = false;
    int imprisonCounter = 0;
    int whichPlayersImprison = 0;
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
    bool isWarp() const;
    bool isImprison() const; //
    int getRow() const;
    int getCol() const;
    int getWarpRow() const;
    int getWarpCol() const;
    int getPlayersServerPort() const;
    int getPlayersFirewall() const;
    int getImprisonCounter() const; //
    int getWhichPlayersImprison() const;
    char getContent() const;
    int getWhoseFirewall() const;
    void setContent(char content);
    void setFirewall(int playerNumber, bool status);
    void setWarpCords(int row, int col);
    void setWarp(bool what);
    void setImprison(bool b); //
    void setImprisonCounter(int counter); //
    void setWhichPlayersImprison(int playerNum);
    void setServerPort(bool b); //
    void decrementImprisonCounter();
};

#endif
