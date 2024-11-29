#ifndef TEXTDISPLAY_H
#define TEXTDISPLAY_H
#include <vector>
#include <iostream>
#include <memory>
#include "Observer.h"
#include "Game.h"

using namespace std;

class TextObserver : public Observer {
    weak_ptr<Game> g;
    ostream &out;
    ostream &err;
    int playerNumber;
    bool multipleDisplay;
    void player_line_1(int num);
    void player_line_2(int data, int virus);
    void player_line_3(int abilities_rem);
    void player_line_4_5(const shared_ptr<Player>& player, int i, int n);
    void player_line_void();
    int newRow(int size) const;
    int newCol(int size) const;
    bool valid(int num, bool row, int size) const;
    void update(int &num, bool row) const;

    public:
    TextObserver(weak_ptr<Game> &g, ostream &out, ostream&err, int playerNumber, bool multipleDisplay);
    void notify() override;
    void displayAbilities();
    int getPlayerNumber() const;
    void displayError(const string &errMsg);
    void displayGameOver() const;
};

#endif

