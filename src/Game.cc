#include "Game.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <compare>
#include <any>
#include "Err.h"
#include "Ability.h"
#include "Graphics.h"
#include "TextDisplay.h"

using namespace std;

const int maxAbilities = 5;

const int player1 = 1;
const int player2 = 2;
const int player3 = 3;
const int player4 = 4;

const int gameOverCondition = 4;

bool in(int x, int l, int u);

bool validPiLink(char link, int p);

bool validP1Link(char link);

bool validP2Link(char link);

bool validP3Link(char link);

bool validP4Link(char link);

bool validLink(char link);

bool validDirection(const string &direction);

void getNewCords(int &row, int &col, shared_ptr<Link> &moveLink, const string &direction);


Game::Game(int playerCount, const vector<string> &linkOrders, const vector<string> &abilities, bool graphicsEnabled) : playerCount{playerCount}, activePlayers{playerCount}, graphicsEnabled{graphicsEnabled} {
    const int size = (playerCount == 2) ? 8 : 10;
    board = make_shared<Board>(size);
    for (int i = 0; i < playerCount; ++i) {
        players.emplace_back(make_shared<Player>(linkOrders[i], size, abilities[i], i + 1));
    }
}

string Game::getAbilityName(int i) {
    // check ability index here
    if (i < 1 || i > maxAbilities) throw runtime_error(Err::invalidAbilityIndex);
    return players[currentTurn - 1]->getAbilityName(i);
}

void Game::useAbility(int abilityNumber, const string &abilityName, const std::vector<std::any> &params) {
    const int size = (playerCount == 2) ? 8 : 10;
    int playerIndex = currentTurn - 1;
    shared_ptr<Ability> ability = players[playerIndex]->getAbility(abilityNumber);
    if (ability->getIsActivated()) throw runtime_error(Err::abilityAlreadyUsed(ability->getAbilityName(), ability->getAbilityID()));

    if (abilityName == "Firewall" || abilityName == "Imprison") {
        int row = std::any_cast<int>(params[0]);
        int col = std::any_cast<int>(params[1]);

        if (!(row >= 0 && row < size && col >= 0 && col < size)) throw out_of_range(Err::invalidCoordinates);
        if (board->getCell(row,col).isLocked()) throw out_of_range(Err::invalidCoordinates);

        if (abilityName == "Firewall") useFirewall(row, col);
        else if (abilityName == "Imprison") useImprison(row, col);
    }
    else if (abilityName == "Warp") {
        int r1 = std::any_cast<int>(params[0]);
        int c1 = std::any_cast<int>(params[1]);
        int r2 = std::any_cast<int>(params[2]);
        int c2 = std::any_cast<int>(params[3]);

        if (!(r1 >= 0 && r1 < size && c1 >= 0 && c1 < size) || !(r2 >= 0 && r2 < size && c2 >= 0 && c2 < size)) throw out_of_range(Err::invalidCoordinates);
        if ((board->getCell(r1,c1).isLocked()) || (board->getCell(r2,c2).isLocked())) throw out_of_range(Err::invalidCoordinates);

        useWarp(r1, c1, r2, c2);
    }
    else {
        char link = std::any_cast<char>(params[0]);
        if (!validLink(link)) throw runtime_error(Err::invalidLink);

        if (abilityName == "LinkBoost") useLinkBoost(link);
        else if (abilityName == "Download") useDownload(link);
        else if (abilityName == "Polarise") usePolarise(link);
        else if (abilityName == "Scan") useScan(link);
        else if (abilityName == "Trojan") useTrojan(link);
    }

    players[playerIndex]->useAbility(abilityNumber);
    checkGameOver();
    notifyObservers();
}

void Game::move(char link, const string &direction) {
    // basic validations
    if (!(validLink(link))) throw runtime_error(Err::invalidLink);
    if (!(validDirection(direction))) throw runtime_error(Err::invalidDirection);
    if (!(validPiLink(link, currentTurn))) throw runtime_error(Err::cannotMoveOpponentsLink);

    int playerIndex = currentTurn - 1;

    shared_ptr<Link> currLink = players[playerIndex]->getLink(link, currentTurn);
    
    // validating if moving a downloaded/trapped link
    if (currLink->getIsDownloaded()) throw runtime_error(Err::cannotMoveDownloadedLink);
    // TODO: check if trying to move a trapped link
    
    // get cords of cell we're moving to
    int newRow = 0, newCol = 0;
    getNewCords(newRow, newCol, currLink, direction);
    
    // check in bounds
    Cell &oldCell = board->getCell(currLink->getRow(), currLink->getCol());
    if (oldCell.isImprison()) throw runtime_error(Err::cannotMoveImprisonedLink);
    // valid iff player moves off opponents edge of the board
    bool validOB = validOutOfBounds(newRow, newCol); // throws error if not valid bounds
    if (validOB) {
        oldCell.setContent('.');
        currLink->download();
        players[playerIndex]->download(currLink);
        // finds next player turn, who is currently active
        for (int i = 0; i < playerCount; ++i) {
            currentTurn = (currentTurn % playerCount) + 1; // (1 % 2) + 1 
            if (isActive(currentTurn)) break;
        }

        for (auto i: trappedCells) {
            if (i->isImprison()) {
                i->decrementImprisonCounter(); 
                cout << i->getImprisonCounter() << endl;
            }
            // check if imprison counter is 0, deactive the trap
            if (i->getImprisonCounter() == 0) {
                i->setImprison(false);
            }
        }
        checkGameOver();
        notifyObservers();
        return;
    }
    // if bounds are valid, get the cells and see if it's self server port or contains self link
    Cell &newCell = board->getCell(newRow, newCol);

    // locked check
    if (newCell.isLocked()) throw runtime_error(Err::notInBounds); 
    // self server port
    if (newCell.isOwnServerPort(currentTurn)) throw runtime_error(Err::cannotDownloadOwnLink(true));
    // self link
    if (validPiLink(newCell.getContent(), currentTurn)) throw runtime_error(Err::cannotMoveOntoOwnLink);
    // imprison cell already trapped with link
    if ((newCell.isImprison()) && (!(newCell.isEmpty()))) throw runtime_error(Err::cannotBattleImprisonedLink);

    // Validations done
    
    // set the cell that the link is being moved from to '.'
    oldCell.setContent('.');

    // moving into an opponent's server port
    if (newCell.isServerPort()) {
        int opponentIndex = newCell.getPlayersServerPort() - 1;
        if (!(players[opponentIndex]->getEliminated())) { // only download if player not eliminated
            currLink->download();
            players[opponentIndex]->download(currLink);
        } else { // moving onto serverport of eliminated player
            newCell.setContent(currLink->getId());
            currLink->setRow(newRow);
            currLink->setCol(newCol);
        }
    } 
    
    // moving into a firewall
    else if (newCell.hasFirewall()) {
        // if self firewall
        if (newCell.hasOwnFirewall(currentTurn)) {
            if (newCell.isEmpty()) {
                newCell.setContent(currLink->getId());
                currLink->setRow(newRow);
                currLink->setCol(newCol);
            } else {
                char content = newCell.getContent();
                int opponentIndex;
                for (int i = 0; i < playerCount; ++i) {
                    if (validPiLink(content, i + 1)) opponentIndex = i;
                }
                if (!(players[opponentIndex]->getEliminated())) { // if moving onto link of non-eliminated player
                    shared_ptr<Link> opponentLink = players[opponentIndex]->getLink(content, opponentIndex + 1);
                    battle(currLink, opponentLink, opponentIndex, newCell);
                    // maybe additional logic
                } else { // if moving onto link of eliminated player
                    newCell.setContent(currLink->getId());
                    currLink->setRow(newRow);
                    currLink->setCol(newCol);
                }
                // maybe additional logic
            }
        }
        else {
            // opponent's firewall. If link is virus, then currentTurn player downloads it
            if (currLink->getType() == LinkType::Virus) {
                players[playerIndex]->download(currLink);
                currLink->download();
            } else {
                currLink->setIsVisible(true);
                if (newCell.isEmpty()) {
                    newCell.setContent(currLink->getId());
                    currLink->setRow(newRow);
                    currLink->setCol(newCol);
                } else {
                    char content = newCell.getContent();
                    int opponentIndex;
                    for (int i = 0; i < playerCount; ++i) {
                        if (validPiLink(content, i + 1)) opponentIndex = i;
                    }
                    
                    if (!(players[opponentIndex]->getEliminated())) { // if moving onto link of non-eliminated player
                        shared_ptr<Link> opponentLink = players[opponentIndex]->getLink(content, opponentIndex + 1);
                        battle(currLink, opponentLink, opponentIndex, newCell);
                    
                    } else { // if moving onto link of eliminated player
                        newCell.setContent(currLink->getId());
                        currLink->setRow(newRow);
                        currLink->setCol(newCol);
                    }
                    // maybe additional logic
                }
            }
        }
    }
    // if new cell contains a warp 
    else if (newCell.isWarp()) {
        int warpToRow = newCell.getWarpRow();
        int warpToCol = newCell.getWarpCol();
        Cell &warpCell = board->getCell(warpToRow, warpToCol);
        warpCell.setContent(currLink->getId());
        cout << warpToCol << ',' << warpToCol << endl;
        currLink->setRow(warpToRow);
        currLink->setCol(warpToCol);
        warpCell.setWarp(false);
        newCell.setWarp(false);
    }
    // else if moving into a trap
    else if (newCell.isImprison()) {
        newCell.setContent(currLink->getId());
        newCell.setImprisonCounter(playerCount + 1);
        currLink->setRow(newRow);
        currLink->setCol(newCol);
        trappedCells.push_back(&newCell);
    }
    // moving into a cell contain an opponents link
    else if (!(newCell.isEmpty())) {
        char content = newCell.getContent();
        int opponentIndex = 0;
        for (int i = 0; i < playerCount; ++i) {
            if (validPiLink(content, i + 1)) opponentIndex = i;
        }

        if (!(players[opponentIndex]->getEliminated())) { // if moving onto link of non-eliminated player
            shared_ptr<Link> opponentLink = players[opponentIndex]->getLink(content, opponentIndex + 1);
            battle(currLink, opponentLink, opponentIndex, newCell);
        } else { // if moving onto link of eliminated player
            newCell.setContent(currLink->getId());
            currLink->setRow(newRow);
            currLink->setCol(newCol);
        }
        // maybe additional logic
    
    // moving into an empty cell
    } else {
        newCell.setContent(currLink->getId());
        currLink->setRow(newRow);
        currLink->setCol(newCol);
    }

    // finds next player turn, who is currently active
    for (int i = 0; i < playerCount; ++i) {
        currentTurn = (currentTurn % playerCount) + 1;
        if (isActive(currentTurn)) break;
    }

    for (auto i: trappedCells) {
        if (i->isImprison()) {
            i->decrementImprisonCounter(); 
            cout << i->getImprisonCounter() << endl;
        }
        // check if imprison counter is 0, deactive the trap
        if (i->getImprisonCounter() == 0) {
            i->setImprison(false);
        }
    }

    checkGameOver();
    notifyObservers();
}

void Game::useFirewall(int row, int col) {
    Cell &cell = board->getCell(row, col);
    // check if trying to place Firewall on a server port
    if (cell.isServerPort()) throw runtime_error(Err::cannotUseAbilityonOtherAbility("Firewall", "Server Port"));
    // check if trying to place firewall on a firewall
    if (cell.hasFirewall()) throw runtime_error(Err::cannotUseAbilityonOtherAbility("Firewall", "Firewall"));
    if (cell.isImprison()) throw runtime_error(Err::cannotUseAbilityonOtherAbility("Firewall", "Imprisoned cell"));
    if (cell.isWarp()) throw runtime_error(Err::cannotUseAbilityonOtherAbility("Firewall", "Warp"));
    // assuming we cannot place a firewall directly onto our opponents links
    for (int i = 0; i < playerCount; ++i) {
        if (i + 1 != currentTurn) {
            if (validPiLink(cell.getContent(), i + 1)) throw runtime_error(Err::cannotPlaceFirewallDirectlyOnOpp);
        }
    } 
    
    cell.setFirewall(currentTurn, true);
}

void Game::useWarp(int r1, int c1, int r2, int c2) {
    Cell &cell1 = board->getCell(r1, c1);
    Cell &cell2 = board->getCell(r2, c2);
    
    // maybe wanna move this into the cell setwarp function?
    if (cell1.isServerPort() || cell2.isServerPort()) throw runtime_error(Err::cannotUseAbilityonOtherAbility("Warp", "Server Port"));
    if (cell1.hasFirewall() || cell2.hasFirewall()) throw runtime_error(Err::cannotUseAbilityonOtherAbility("Warp", "Firewall"));
    if (!(cell1.isEmpty() && cell2.isEmpty())) throw runtime_error(Err::cannotUseAbilityonOtherAbility("Warp", "Link"));
    if (cell1.isWarp() || cell2.isWarp()) throw runtime_error(Err::cannotUseAbilityonOtherAbility("Warp", "Warp"));
    if (cell1.isImprison() || cell2.isImprison()) throw runtime_error(Err::cannotUseAbilityonOtherAbility("Warp", "Imprison"));

    cell1.setWarp(true);
    cell1.setWarpCords(r2, c2);
    cell2.setWarp(true);
    cell2.setWarpCords(r1, c1);
}

void Game::useImprison(int row, int col) {
    Cell &cell = board->getCell(row, col);

    if (cell.isServerPort()) throw runtime_error(Err::cannotUseAbilityonOtherAbility("Imprison", "Server Port"));
    if (cell.hasFirewall()) throw runtime_error(Err::cannotUseAbilityonOtherAbility("Imprison", "Firewall"));
    if (!(cell.isEmpty())) throw runtime_error(Err::cannotUseAbilityonOtherAbility("Imprison", "Link"));
    if (cell.isWarp()) throw runtime_error(Err::cannotUseAbilityonOtherAbility("Imprison", "Warp"));
    if (cell.isImprison()) throw runtime_error(Err::cannotUseAbilityonOtherAbility("Imprison", "Imprison"));

    cell.setImprison(true);
    cell.setWhichPlayersImprison(currentTurn);
    //trappedCells.push_back(&cell);
}

void Game::useScan(char l) {
    // if player tries to scan their own link, return error
    if (validPiLink(l, currentTurn)) {
        throw runtime_error(Err::cannotUseAbilityOnOwnLink("Scan"));
    }

    // get player indices
    int otherPlayerIndex;
    
    // get the link to be scanned and execute the download
    for (int i = 0; i < playerCount; ++i) {
        if ((i + 1 != currentTurn) && (validPiLink(l, i + 1))) otherPlayerIndex = i;
    }

    shared_ptr<Link> currLink = players[otherPlayerIndex]->getLink(l, otherPlayerIndex + 1);

    // if downloaded already visible
    currLink->scan();
}

void Game::useDownload(char link) {
    // if player tries to download their own link, return error
    if (validPiLink(link, currentTurn)) throw runtime_error(Err::cannotUseAbilityOnOwnLink("Download"));
    
    // get player indices
    int playerIndex = currentTurn - 1;
    int otherPlayerIndex;
    // get the link to be scanned and execute the download
    for (int i = 0; i < playerCount; ++i) {
        if ((i + 1 != currentTurn) && (validPiLink(link, i + 1))) otherPlayerIndex = i;
    }

    // get the link to be downloaded and execute the download
    shared_ptr<Link> downloadedLink = players[otherPlayerIndex]->getLink(link, otherPlayerIndex + 1);

    // line directly below checks if already downloaded, if so throws error
    downloadedLink->download();
    players[playerIndex]->download(downloadedLink);
    int row = downloadedLink->getRow(), col = downloadedLink->getCol();
    Cell &cell = board->getCell(row, col);
    cell.setContent('.');
}


void Game::usePolarise(char link) {
    // get the link to be polarised
    shared_ptr<Link> currLink;
    for (int i = 0; i < playerCount; ++i) {
        if (validPiLink(link, i + 1)) currLink = players[i]->getLink(link, i + 1);
    }
    
    // check if already downloaded, then no point polarising
    if (currLink->getIsDownloaded()) throw runtime_error(Err::isAlreadyDownloaded);
    currLink->polarise();
}

void Game::useTrojan(char link) {
    // can't use Trojan on opponent(s) links
    if (!(validPiLink(link, currentTurn))) throw runtime_error(Err::cannotUseAbilityOnOtherLink("LinkBoost"));
    // get the link to trojan
    shared_ptr<Link> currLink;
    for (int i = 0; i < playerCount; ++i) {
        if (validPiLink(link, i + 1)) currLink = players[i]->getLink(link, i + 1);
    }

    // you can't apply trojan to a downloaded link
    if (currLink->getIsDownloaded()) throw runtime_error(Err::isAlreadyDownloaded);
    // the function below checks if the link is already a trojan
    currLink->trojan();
}

void Game::useLinkBoost(char link) {
    // if player tries to linkboost other player's link, return error
    if (!(validPiLink(link, currentTurn))) throw runtime_error(Err::cannotUseAbilityOnOtherLink("LinkBoost")); 

    int playerIndex = currentTurn - 1;
    
    // get the link to be linkboosted
    shared_ptr<Link> currLink = players[playerIndex]->getLink(link, currentTurn);

    // if player tries to linkboost already downloaded link, return error
    if (currLink->getIsDownloaded()) throw runtime_error(Err::isAlreadyDownloaded);
    // this is executed if no errors thrown and updates ability state to being activated
    currLink->linkBoost();
}

void Game::checkGameOver() {
    for (int i = 0; i < playerCount; ++i) {
        // Check if (i + 1)'th player won by downloading enough data
        if (players[i]->getDataDownloaded() >= gameOverCondition) {
            gameOver = true;
            playerWon = i + 1;
        }
        // Check if (i + 1)'th player lost by downloading too many viruses
        if (players[i]->getVirusDownloaded() >= gameOverCondition) {
            
            if (playerCount > 2) {
                if (players[i]->getEliminated()) continue;
                players[i]->setEliminated(true);
                for (int j = 0; j < board->getSize(); ++j) {
                    for (int k = 0; k < board->getSize(); ++k) {
                        Cell &c = board->getCell(j,k);
                        if (!(c.isEmpty())) {
                            if (validPiLink(c.getContent(), i+1)) c.setContent('.');
                        } 
                        
                        if (c.hasOwnFirewall(i+1)) c.setFirewall(0, false);
                        // keep imprison and warp, they do not belong to anyone
                        if (c.isOwnServerPort(i+1)) c.setServerPort(false);
                    }
                }
                
                --activePlayers;
                // finds next player turn, who is currently active
                for (int i = 0; i < playerCount; ++i) {
                    currentTurn = (currentTurn % playerCount) + 1;
                    if (isActive(currentTurn)) break;
                }
                
                gameOver = (activePlayers > 1) ? false : true;
                if (gameOver) playerWon = currentTurn;

            } else {
                gameOver = true;
                playerWon = (i == 0) ? player2 : player1;
            }
            
        }
    }
}

int Game::getCurrentTurn() const {
    return currentTurn;
}

int Game::getPlayerCount() const {
    return playerCount;
}

bool validLink(char link) {
    return validP1Link(link) || validP2Link(link) || validP3Link(link) || validP4Link(link);
}

bool validDirection(const string &direction) {
    return direction == "up" || direction == "down" || direction == "left" || direction == "right";
}

void Game::battle(shared_ptr<Link> currLink, shared_ptr<Link> oppLink, int opponentIndex, Cell &cell) {
    currLink->setIsVisible(true);
    oppLink->setIsVisible(true);
    int currStrength = currLink->getStrength();
    int oppStrength = oppLink->getStrength();
    int playerIndex = currentTurn - 1;
    bool currLinkWon = currStrength >= oppStrength;
    
    if (currLinkWon) {
        // player downloads and also link status gets updated
        players[playerIndex]->download(oppLink);
        oppLink->download();

        // set the cell with currLink contents
        currLink->setCol(cell.getCol());
        currLink->setRow(cell.getRow());
        cell.setContent(currLink->getId());
    }
    else {
        // otherwise opponent simply downloads currLink and link status is updated
        players[opponentIndex]->download(currLink);
        currLink->download();
    }
}

void getNewCords(int &row, int &col, shared_ptr<Link> &moveLink, const string &direction) {
    int increment = moveLink->getStepSize();
    if (direction == "up") {
        row = moveLink->getRow() - increment;
        col = moveLink->getCol();
    }
    else if (direction == "down") {
        row = moveLink->getRow() + increment;
        col = moveLink->getCol();
    }
    else if (direction == "left") {
        col = moveLink->getCol() - increment;
        row = moveLink->getRow();
    }
    else {
        col = moveLink->getCol() + increment;
        row = moveLink->getRow();
    }
}

bool Game::validOutOfBounds(int row, int col) const {
    int size = board.get()->getSize(); // 10 or 8
    int lower = 1, upper = 8;
    lower -= (size == 8);
    upper -= (size == 8);       
    
    if (in(row, 0, size - 1) && in(col, 0, size - 1)) {
        return false;
    } else if (row < 0) { // P1 side
        if (in(col, lower, upper)) {
            if (isActive(1)) {
                if (currentTurn != 1) return true; // if currentTUrn == 1 then error, else true;
            } 
        }  
    } else if (row >= size) { // P2 side
        if (in(col, lower, upper)) {
            if (isActive(2)) {
                if (currentTurn != 2) return true;
            }
        }
    } else if (col < 0) { // P3 side
        if (in(row, lower, upper)) {
            if (isActive(3)) {
                if (currentTurn != 3) return true;
            }
        }
    } else if (col >= size) { // P4 side
        if (in(row, lower, upper)) {
            if (isActive(4)) {
                if (currentTurn != 4) return true;
            }
        }
    }  
    // out of edge from own side returns wrong error message
    throw runtime_error(Err::notInBounds); 
    
}

void Game::displayAbilities(bool multipleDisplay) const {
    for (const auto& o : observers) {
        if (auto textObserver = dynamic_pointer_cast<TextObserver>(o)) {
            if (!multipleDisplay) {
                textObserver->displayAbilities();
            } else if (textObserver->getPlayerNumber() == currentTurn) {
                textObserver->displayAbilities();
            }
        } else if (auto graphicsObserver = dynamic_pointer_cast<Graphics>(o)) {
            if (!multipleDisplay) {
                graphicsObserver->displayAbilities();
            } else if (graphicsObserver->getPlayerNumber() == currentTurn) {
                graphicsObserver->displayAbilities();
            }
        }
    }
}

void Game::displayGameOver() {
    for (auto &o : observers) {
        if (auto textObserver = dynamic_pointer_cast<TextObserver>(o)) {
            textObserver->displayGameOver();
        } else if (auto graphicsObserver = dynamic_pointer_cast<Graphics>(o)) {
            graphicsObserver->displayGameOver();
        }
    }
}

int Game::getPlayerWon() const {
    return playerWon;
}

void Game::displayErr(const string &err, bool multipleDisplay) const {
    for (const auto& o : observers) {
        if (auto textObserver = dynamic_pointer_cast<TextObserver>(o)) {
            if (!multipleDisplay) {
                textObserver->displayError(err);
            } else if (textObserver->getPlayerNumber() == currentTurn) {
                textObserver->displayError(err);
            }
        } else if (auto graphicsObserver = dynamic_pointer_cast<Graphics>(o)) {
            if (!multipleDisplay) {
                graphicsObserver->displayError(err);
            } else if (graphicsObserver->getPlayerNumber() == currentTurn) {
                graphicsObserver->displayError(err);
            }
        }
    }
}

const shared_ptr<Player>& Game::getPlayer(int playerNumber) const {
    return players[playerNumber - 1];
}

shared_ptr<Board> Game::getBoard() const {
    return board;
}

// isActive() checks if player is playing
bool Game::isActive(int playeri) const {
    if (in(playeri, 3, 4) && (players.size() == 2)) throw runtime_error(Err::notInBounds);
    return !(players[playeri - 1].get()->getEliminated());
}
bool in(int x, int l, int u) {
    return (x >= l) && (x <= u);
}

bool Game::getIsGameOver() const {
    return gameOver;
}

bool validPiLink(char link, int p) {
    if (p == 1) return validP1Link(link);
    else if (p == 2) return validP2Link(link);
    else if (p == 3) return validP3Link(link);
    else if (p == 4) return validP4Link(link);
    else return false;
}

bool validP1Link(char link) {
    return (link >= 'a') && (link <= 'h');
}

bool validP2Link(char link) {
    return (link >= 'A') && (link <= 'H');
}

bool validP3Link(char link) {
    return (link >= 'i') && (link <= 'p');
}

bool validP4Link(char link) {
    return (link >= 'I') && (link <= 'P');
}
