#ifndef CELL_H
#define CELL_H
#include <string>
#include <memory>

class Cell {
    int row;
    int col;
    int size;
    char c; // character on the cell
    int whichPlayersFirewall = 0; // 0 refers to no player
    int whichPlayersServerPort = 0; // same
    bool locked; // for 4 players, locked indicates if the cell is one of the corners
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

    // firewall
    bool hasFirewall() const;
    bool hasOwnFirewall(int playerNumber) const;
    bool hasOpponnentFirewall(int playerNumber) const;
    int getPlayersFirewall() const;
    int getWhoseFirewall() const;

    void setFirewall(int playerNumber, bool status);

    // serverPort
    bool isServerPort() const;
    bool isOwnServerPort(int playerNumber) const;
    bool isOpponentServerPort(int playerNumber) const;
    int getPlayersServerPort() const;

    void setServerPort(bool b); 

    // warp
    bool isWarp() const;
    int getWarpRow() const;
    int getWarpCol() const;

    void setWarpCords(int row, int col);
    void setWarp(bool what);

    // imprison
    bool isImprison() const;
    int getImprisonCounter() const; //
    int getWhichPlayersImprison() const;

    void setImprison(bool b); //
    void setImprisonCounter(int counter); //
    void setWhichPlayersImprison(int playerNum);

    // normal 
    bool isEmpty() const;
    bool isLocked() const;
    int getRow() const;
    int getCol() const;
    char getContent() const;

    // other
    void setContent(char content);
    void decrementImprisonCounter();
};

#endif
