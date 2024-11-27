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
    public:
    Graphics(weak_ptr<Game> &game, int playerNumber, bool multipleDisplay);
    void notify() override;
    void displayAbilities();
    void displayError(const string &Err);
    int getPlayerNumber() const;
    void displayGameOver();
};


#endif