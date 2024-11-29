#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "Observer.h"
#include "window.h"
#include "Game.h"
#include <vector>
#include <memory>

enum class Colour {White, Black, Red, Blue, Green, Yellow, Gray};

class Graphics : public Observer {
    weak_ptr<Game> g;
    int playerNumber;
    bool multipleDisplay;
    Xwindow w;
    int newRow(int size) const;
    int newCol(int size) const;
    bool valid(int num, bool row, int size) const;
    void update(int &num, bool row) const;
    public:
    Graphics(weak_ptr<Game> &game, int playerNumber, bool multipleDisplay);
    void notify() override;
    void displayAbilities();
    void displayError(const string &Err);
    int getPlayerNumber() const;
    void displayGameOver();
};


#endif
