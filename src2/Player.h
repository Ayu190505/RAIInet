#ifndef PLAYER_H
#define PLAYER_H
#include <vector>
#include <memory>
#include "Cell.h"

using namespace std;

class Link;
class Ability;

class Player {
    int id;
    int size;
    vector<shared_ptr<Link>> links;
    vector<shared_ptr<Link>> downloadedLinks;
    vector<shared_ptr<Ability>> abilities;
    int numOfDataDownloaded, numOfVirusDownloaded, abilityCount;
    bool eliminated = false;
    public:
    Player(const string &links, int size, const string &abilities, int id);
    string getAbilityName(int i);
    void useAbility(int i);
    void download(shared_ptr<Link> &l);
    int getDataDownloaded() const;
    int getVirusDownloaded() const;
    void setDataDownloaded(int i);
    void setVirusDownloaded(int i);
    int getRemainingAbilities() const;
    bool getEliminated() const;
    void setEliminated(bool b);
    shared_ptr<Ability> getAbility(int i);
    shared_ptr<Link> getLink(char c, int playerNumber);
};

#endif
