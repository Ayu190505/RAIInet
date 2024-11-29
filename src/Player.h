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

    // getters
    string getAbilityName(int i);
    int getDataDownloaded() const;
    int getVirusDownloaded() const;
    shared_ptr<Ability> getAbility(int i);
    shared_ptr<Link> getLink(char c, int playerNumber);
    int getRemainingAbilities() const;
    bool getEliminated() const;

    // setters
    void setDataDownloaded(int i);
    void setVirusDownloaded(int i);
    void setEliminated(bool b);

    // other
    void useAbility(int abilityNumber);
    void download(shared_ptr<Link> &l);
};

#endif

