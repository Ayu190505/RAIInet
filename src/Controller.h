#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <memory>
#include "Game.h"
#include "TextDisplay.h"
#include "Graphics.h"
#include "Err.h"

using namespace std;

class Controller {
    shared_ptr<Game> game;
    int numPlayers;
    bool graphicsEnabled;
    bool multipleDisplay;
    public:
    Controller(shared_ptr<Game> game, int numPlayers, bool graphicsEnabled, bool multipleDisplays);
    void run() const;
};


#endif

