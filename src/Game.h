#ifndef GAME_H
#define GAME_H
#include <any>
#include <vector>
#include <memory>
#include <string>
#include "Cell.h"
#include "Player.h"
#include "Link.h"
#include "Subject.h"
#include "Board.h"

using namespace std;

// NOTE: abilityIndex is the ability number, not index. Change the name later

class Game : public Subject {
    shared_ptr<Board> board;
    vector<shared_ptr<Player>> players;
    vector<Cell*> trappedCells;
    const int playerCount;
    int activePlayers;
    int currentTurn = 1;
    bool gameOver = false;
    int playerWon = 0;
    bool graphicsEnabled;

    // private helpers
    void battle(shared_ptr<Link> currLink, shared_ptr<Link> opponentLink, int opponentIndex, Cell &cell);
    bool validOutOfBounds(int row, int col) const;
    bool isActive(int playeri) const;
    
    public:
    Game(int playerCount, const vector<string> &linkOrders, const vector<string> &abilities, bool graphicsEnabled);

    // abilities
    void useAbility(int abilityNumber, const string &abilityName, const vector<any> &params);
    void useFirewall(int row, int col);
    void useDownload(char link);
    void useLinkBoost(char link);
    void usePolarise(char link);
    void useScan(char link);
    void useTrojan(char link);
    void useWarp(int r1, int c1, int r2, int c2);
    void useImprison(int row, int col);
    void useOmit(int row, int col);
    void useBlock(int row, int col);
    void displayAbilities(bool multipleDisplay) const;
    string getAbilityName(int index);

    // getters
    int getCurrentTurn() const;
    int getPlayerCount() const;
    bool getIsGameOver() const;
    int getPlayerWon() const;
    shared_ptr<Board> getBoard() const;
    const shared_ptr<Player>& getPlayer(int playerNumber) const;

    // other
    void move(char link, const string &direction);
    void displayErr(const string &err, bool multipleDisplay) const;
    void checkGameOver();
    void displayGameOver();
};


#endif

