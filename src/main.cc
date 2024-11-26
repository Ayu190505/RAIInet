#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <random>
#include <unordered_map>
#include "Game.h"
#include "Err.h"
#include "Controller.h"

using namespace std;

const int abilitiesLength = 5;
const int maxOfEachAbility = 2;
const int maxPlayers = 4;

string randomLinkGenerator();
void areAbilitiesValid(const string &abilities, const string &validAbilities);
string retrieveLink(ifstream &file);

int main(int argc, const char* argv[]){
    // Argument checking here
    int numPlayers = 2;
    bool multipleDisplay = false;
    string validAbilities = "LFDSP";
    vector<string> playerAbilities(maxPlayers, "LFDSP"); // index + 1 represents player number
    vector<string> playerLinkOrders(maxPlayers); // same here
    for (int i = 0; i < maxPlayers; ++i) {
        playerLinkOrders[i] = randomLinkGenerator();
    }
    bool graphicsEnabled = false;
    try { 
        for (int i = 1; i < argc; ++i) {
            string arg = argv[i];
            if (arg == "-ability1" || arg == "-ability2" || arg == "-ability3" || arg == "-ability4") {
                if ((arg == "-ability3" || arg == "-ability4") && numPlayers != maxPlayers) throw invalid_argument(Err::invalidPlayerNumber);
                int abilityIndex = arg.back() - '0' - 1;
                ++i;
                if (i >= argc) throw out_of_range(Err::insufficientArgs);
                playerAbilities[abilityIndex] = string(argv[i]);
                areAbilitiesValid(playerAbilities[abilityIndex], validAbilities);
            } else if (arg == "-link1" || arg == "-link2" || arg == "-link3" || arg == "-link4") {
                if ((arg == "-link3" || arg == "-link4") && numPlayers != maxPlayers) throw invalid_argument(Err::invalidPlayerNumber);
                int linkIndex = arg.back() - '0' - 1;
                ++i;
                if (i >= argc) throw out_of_range(Err::insufficientArgs);
                ifstream file{argv[i]};
                playerLinkOrders[linkIndex] = retrieveLink(file);
            } else if (arg == "graphics") {
                graphicsEnabled = true;
            } else if (arg == "multipleDisplay") {
                multipleDisplay = true;
            } else if (arg == "player-four") {
                numPlayers = maxPlayers;
            } else {
                throw invalid_argument(Err::invalidCommandLineArg + string(argv[i]));
            }
        }
    } catch(const exception &e) {
        cerr << e.what() << endl;
        return 1;
    }

    playerLinkOrders.resize(numPlayers);
    playerAbilities.resize(numPlayers);

    // Welcome message
    cout << R"(
        .----------------.  .----------------.  .----------------.  .----------------.  .----------------.  .----------------.  .----------------.               .----------------.  .----------------. 
        | .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. |             | .--------------. || .--------------. |
        | | _____  _____ | || |  _________   | || |   _____      | || |     ______   | || |     ____     | || | ____    ____ | || |  _________   | |             | |  _________   | || |     ____     | |
        | ||_   _||_   _|| || | |_   ___  |  | || |  |_   _|     | || |   .' ___  |  | || |   .'    `.   | || ||_   \  /   _|| || | |_   ___  |  | |             | | |  _   _  |  | || |   .'    `.   | |
        | |  | | /\ | |  | || |   | |_  \_|  | || |    | |       | || |  / .'   \_|  | || |  /  .--.  \  | || |  |   \/   |  | || |   | |_  \_|  | |             | | |_/ | | \_|  | || |  /  .--.  \  | |
        | |  | |/  \| |  | || |   |  _|  _   | || |    | |   _   | || |  | |         | || |  | |    | |  | || |  | |\  /| |  | || |   |  _|  _   | |             | |     | |      | || |  | |    | |  | |
        | |  |   /\   |  | || |  _| |___/ |  | || |   _| |__/ |  | || |  \ `.___.'\  | || |  \  `--'  /  | || | _| |_\/_| |_ | || |  _| |___/ |  | |             | |    _| |_     | || |  \  `--'  /  | |
        | |  |__/  \__|  | || | |_________|  | || |  |________|  | || |   `._____.'  | || |   `.____.'   | || ||_____||_____|| || | |_________|  | |             | |   |_____|    | || |   `.____.'   | |
        | |              | || |              | || |              | || |              | || |              | || |              | || |              | |             | |              | || |              | |
        | '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' |             | '--------------' || '--------------' |
        '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'               '----------------'  '----------------'
    )" << endl;

    cout << R"(

                                    .----------------.  .----------------.  .----------------.  .----------------.  .-----------------. .----------------.  .----------------.   
                                    | .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. |  
                                    | |  _______     | || |      __      | || |     _____    | || |     _____    | || | ____  _____  | || |  _________   | || |  _________   | |  
                                    | | |_   __ \    | || |     /  \     | || |    |_   _|   | || |    |_   _|   | || ||_   \|_   _| | || | |_   ___  |  | || | |  _   _  |  | |  
                                    | |   | |__) |   | || |    / /\ \    | || |      | |     | || |      | |     | || |  |   \ | |   | || |   | |_  \_|  | || | |_/ | | \_|  | |  
                                    | |   |  __ /    | || |   / ____ \   | || |      | |     | || |      | |     | || |  | |\ \| |   | || |   |  _|  _   | || |     | |      | |  
                                    | |  _| |  \ \_  | || | _/ /    \ \_ | || |     _| |_    | || |     _| |_    | || | _| |_\   |_  | || |  _| |___/ |  | || |    _| |_     | |  
                                    | | |____| |___| | || ||____|  |____|| || |    |_____|   | || |    |_____|   | || ||_____|\____| | || | |_________|  | || |   |_____|    | |  
                                    | |              | || |              | || |              | || |              | || |              | || |              | || |              | |  
                                    | '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' |  
                                    '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'   

    )" << endl;

    // start of game loop within controller class
    shared_ptr<Game> game = make_shared<Game>(numPlayers, playerLinkOrders, playerAbilities, graphicsEnabled);
    Controller controller{game, numPlayers, graphicsEnabled, multipleDisplay};
    controller.run();
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
