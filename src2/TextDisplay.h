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
    void player_line_1(int num);
    void player_line_2(int data, int virus);
    void player_line_3(int abilities_rem);
    void player_line_4_5(const shared_ptr<Player>& player, int i, int n);
    void player_line_void();
    public:
    TextObserver(weak_ptr<Game> &g, ostream &out, ostream&err, int playerNumber);
    void notify() override;
    //void displayAbilities() override;
    int getPlayerNumber() const override;
    void displayError(const string &errMsg) override;
};

#endif
