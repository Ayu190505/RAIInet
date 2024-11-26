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
    Xwindow w;
    int playerNumber;
    public:
    Graphics(weak_ptr<Game> &game, int playerNumber);
    void notify() override;
    //void displayAbilities() override;
    void displayError(const string &Err) override;
    int getPlayerNumber() const override;
};


#endif