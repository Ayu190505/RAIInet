#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <memory>
#include <algorithm>
#include <random>
#include <unordered_map>
#include "Game.h"
#include "TextDisplay.h"
#include "Graphics.h"
#include "Err.h"

using namespace std;

const int abilitiesLength = 5;
const int maxOfEachAbility = 2;
const int numPlayers = 2; // to quickly change player counts

string randomLinkGenerator();
void areAbilitiesValid(const string &abilities, const string &validAbilities);
string retrieveLink(ifstream &file);

int main(int argc, const char* argv[]){
    // Argument checking here
    string validAbilities = "LFDSP";
    vector<string> playerAbilities(numPlayers, "LFDSP"); // index + 1 represents player number
    vector<string> playerLinkOrders(numPlayers); // same here
    for (int i = 0; i < numPlayers; ++i) {
        playerLinkOrders[i] = randomLinkGenerator();
    }
    bool graphicsEnabled = false;
    try { 
        for (int i = 1; i < argc; ++i) {
            string arg = argv[i];
            if (arg == "-ability1") {
                ++i;
                if (i >= argc) throw out_of_range(Err::insufficientArgs);
                playerAbilities[0] = string(argv[i]);
                areAbilitiesValid(playerAbilities[0], validAbilities);
            } else if (arg == "-ability2") {
                ++i;
                if (i >= argc) throw out_of_range(Err::insufficientArgs);
                playerAbilities[1] = string(argv[i]);
                areAbilitiesValid(playerAbilities[1], validAbilities);                
            } else if (arg == "-link1") {
                ++i;
                if (i >= argc) throw out_of_range(Err::insufficientArgs);
                ifstream file{argv[i]};
                playerLinkOrders[0] = retrieveLink(file);
            } else if (arg == "-link2") {
                ++i;
                if (i >= argc) throw out_of_range(Err::insufficientArgs);
                ifstream file{argv[i]};
                playerLinkOrders[1] = retrieveLink(file);
            } else if (arg == "graphics") {
                graphicsEnabled = true;
            } else {
                throw invalid_argument(Err::invalidCommandLineArg + string(argv[i]));
            }
        }
    } catch(const exception &e) {
        cerr << e.what() << endl;
        return 1;
    }

    // Start of Game Loop
    shared_ptr<Game> game = make_shared<Game>(numPlayers, playerLinkOrders, playerAbilities, graphicsEnabled);
    weak_ptr<Game> g = game;
    shared_ptr<TextObserver> t = make_shared<TextObserver>(g, cout, cerr, 1);
    shared_ptr<TextObserver> t2 = make_shared<TextObserver>(g, cout, cerr, 2);
    shared_ptr<TextObserver> t3 = make_shared<TextObserver>(g, cout, cerr, 3);
    shared_ptr<TextObserver> t4 = make_shared<TextObserver>(g, cout, cerr, 4);
    //shared_ptr<Graphics> graphics = make_shared<Graphics>(g, 1);
    //shared_ptr<Graphics> graphics2 = make_shared<Graphics>(g, 2);
    
    game->attach(t);
    game->attach(t2);
    //game->attach(t3);
    //game->attach(t4);
    
   // game->attach(t2);
    //game->attach(graphics);
    //game->attach(graphics2);
    ifstream sequenceFile;
    string command;
    bool abilityUsedThisTurn = false;
    istream *in = &cin; // using pointer to make input simple
    while (true) {
        if (!(*in >> command)) {
            if (in == &sequenceFile) {
                sequenceFile.close();
                in = &cin;
                continue;
            } else {
                break; // EOF detected from cin
            }
        }
        try {
            if (command == "sequence") {
                string newFile;
                if (!(*in >> newFile))
                sequenceFile.close();
                sequenceFile.clear();
                sequenceFile.open(newFile);
                if (!sequenceFile.is_open()) throw runtime_error(Err::invalidFile);
                in = &sequenceFile;
            } else if (command == "abilities") {
                game->displayAbilities();
            } else if (command == "ability") {
                if (abilityUsedThisTurn) {
                    throw runtime_error(Err::abilityUsedThisTurn);
                }
                int abilityIndex = 0;
                if (!(*in >> abilityIndex)) throw runtime_error(Err::invalidAbilityIndex);
                string abilityName = game->getAbilityName(abilityIndex);
                if (abilityName == "Firewall") {
                    int row, col;
                    if (!(*in >> row >> col)) throw runtime_error(Err::expectedCoordinatesForFireWall);
                    game->useAbility(abilityIndex, row, col);
                } else {
                    char c;
                    if (!(*in >> c)) throw runtime_error(Err::expectedLinkIdentity);
                    game->useAbility(abilityIndex, abilityName, c);
                }
                abilityUsedThisTurn = true;
            } else if (command == "board") {
                game->notifyObservers();
            } else if (command == "quit") {
                break;
            } else if (command == "move") {
                char c;
                string direction;
                if (!(*in >> c >> direction)) throw runtime_error(Err::expectedLinkAndDir);
                game->move(c, direction);
                abilityUsedThisTurn = false;
            } else {
                throw runtime_error(Err::invalidRuntimeCommand);
            }
        } catch(const exception &e) {
            game->displayErr(e.what());
            continue;
        }
    }
}

string randomLinkGenerator() {
    vector<string> links = {
        "D1", "D2", "D3", "D4",
        "V1", "V2", "V3", "V4"
    };
    random_device rd;  
    mt19937 rng(rd()); 
    shuffle(links.begin(), links.end(), rng);
    string linkOrder;
    for (const auto &link : links) {
        linkOrder += link;
    }
    return linkOrder;
}

void areAbilitiesValid(const string &s, const string &validAbilities) {
    if (s.length() != abilitiesLength) throw(Err::invalidAbilities);
    unordered_map<char, int> mp;
    for (size_t i = 0; i < validAbilities.size(); ++i) {
        mp[validAbilities[i]] = 0;
    }
    for (size_t i = 0; i < s.size(); ++i) {
        char c = toupper(s[i]);
        if (mp.find(c) != mp.end() && mp[c] < maxOfEachAbility) {
            ++mp[c];
        } else {
            throw invalid_argument(Err::invalidAbilities);
        }
    }
}

string retrieveLink(ifstream &file) {
    const int requiredCount = 1;
    vector<string> links = {
        "D1", "D2", "D3", "D4",
        "V1", "V2", "V3", "V4"
    };
    unordered_map<string, int> mp;
    for (size_t i = 0; i < links.size(); ++i) {
        mp[links[i]] = 0;
    }
    string s, linkOrder;
    while (file >> s) {
        transform(s.begin(), s.end(), s.begin(), [] (unsigned char c) {
            return toupper(c);
        });
        if (mp.find(s) != mp.end()) {
            ++mp[s];
            linkOrder += s;
        } else {
            throw invalid_argument(Err::invalidLinks);
        }
    }
    for (const auto &elem : mp) {
        if (elem.second != requiredCount) {
            throw invalid_argument(Err::invalidLinks);
        }
    }
    return linkOrder;
}
