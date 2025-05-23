#include "Controller.h"
#include <limits>
#include <any>
#include "Ability.h"

void setUpObservers(const shared_ptr<Game> game, vector<shared_ptr<Observer>> &observers, int numPlayers, bool graphicsEnabled, bool multipleDisplay);

Controller::Controller(shared_ptr<Game> game, int numPlayers, bool graphicsEnabled, bool multipleDisplay) : 
    game{game}, numPlayers{numPlayers}, graphicsEnabled{graphicsEnabled}, multipleDisplay{multipleDisplay} {}

void Controller::run() const {
    vector<shared_ptr<Observer>> observers;
    setUpObservers(game, observers, numPlayers, graphicsEnabled, multipleDisplay);
    ifstream sequenceFile;
    string command;
    bool abilityUsedThisTurn = false;
    istream *in = &cin; // using pointer to make input simple
    while (true) {
        if (game->getIsGameOver()) {
            game->displayGameOver();
            break;
        }
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
                game->displayAbilities(multipleDisplay);
            } else if (command == "ability") {
                if (abilityUsedThisTurn) {
                    throw runtime_error(Err::abilityUsedThisTurn);
                }
                int abilityIndex = 0;
                if (!(*in >> abilityIndex)) throw runtime_error(Err::invalidAbilityIndex);
                string abilityName = game->getAbilityName(abilityIndex);

                // stores any type
                std::vector<std::any> params;

                // what we expect the type of param for the function to take
                std::vector<std::string> expectedParams = Ability::getExpectedParams(abilityName);
                int paramNum = 1; // to track which specific param was invalid
                for (const auto &parameterType : expectedParams) {
                    if (parameterType == "int") {
                        int input;
                        if (!(*in >> input)) throw runtime_error(Err::abilityExpectsInputOf(abilityName, paramNum, "an integer"));
                        params.push_back(input);
                    }
                    else if (parameterType == "char") {
                        char input;
                        if (!(*in >> input)) throw runtime_error(Err::abilityExpectsInputOf(abilityName, paramNum, "a char"));
                        params.push_back(input);
                    }
                    else if (parameterType == "string") {
                        string input;
                        if (!(*in >> input)) throw runtime_error(Err::abilityExpectsInputOf(abilityName, paramNum, "a string"));
                        params.push_back(input);
                    }
                    ++paramNum;
                }

                game->useAbility(abilityIndex, abilityName, params);
                abilityUsedThisTurn = true;
            } else if (command == "board") {
                // change to just display board
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
            in->clear();
            in->ignore(std::numeric_limits<int>::max(), '\n');           
            game->displayErr(e.what(), multipleDisplay);
            continue;
        }
    }
}

void setUpObservers(const shared_ptr<Game> game, vector<shared_ptr<Observer>> &observers, int numPlayers, bool graphicsEnabled, bool multipleDisplay) {
    weak_ptr<Game> g = game;
    if (multipleDisplay) {
        for (int i = 1; i <= numPlayers; ++i) {
            shared_ptr<TextObserver> t = make_shared<TextObserver>(g, cout, cerr, i, multipleDisplay);
            game->attach(t);
            observers.emplace_back(t);
            if (graphicsEnabled) {
                shared_ptr<Graphics> graphics = make_shared<Graphics>(g, i, multipleDisplay);
                game->attach(graphics);
                observers.emplace_back(graphics);
            }
        }
    } else if (graphicsEnabled) {
        int startTurn = 1;
        shared_ptr<TextObserver> t = make_shared<TextObserver>(g, cout, cerr, startTurn, multipleDisplay);
        shared_ptr<Graphics> graphics = make_shared<Graphics>(g, startTurn, multipleDisplay);
        game->attach(t);
        game->attach(graphics);
        observers.emplace_back(t);
        observers.emplace_back(graphics);
    } else {
        int startTurn = 1;
        shared_ptr<TextObserver> t = make_shared<TextObserver>(g, cout, cerr, startTurn, multipleDisplay);
        game->attach(t);
        observers.emplace_back(t);
    }
}

