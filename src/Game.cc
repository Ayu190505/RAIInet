#include "Game.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <compare>
#include "Err.h"
#include "Ability.h"
#include "Graphics.h"
#include "TextDisplay.h"

using namespace std;

const int maxAbilities = 5;
const int numCol = 8;

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
    const int numRow = (playerCount == 2) ? 8 : 10;
    board = make_shared<Board>(numRow);
    for (int i = 0; i < playerCount; ++i) {
        players.emplace_back(make_shared<Player>(linkOrders[i], numRow, abilities[i], i + 1));
    }
}

string Game::getAbilityName(int i) {
    // check ability index here
    if (i < 1 || i > maxAbilities) throw runtime_error(Err::invalidAbilityIndex);
    return players[currentTurn - 1]->getAbilityName(i);
}

// update useAbility to check if already used ability before calling each function
void Game::useAbility(int abilityNumber, int row, int col) {
    const int numRow = (playerCount == 2) ? 8 : 10;
    int playerIndex = currentTurn - 1;
    shared_ptr<Ability> ability = players[playerIndex]->getAbility(abilityNumber);
    if (ability->getIsActivated()) throw runtime_error(Err::abilityAlreadyUsed(ability->getAbilityName(), ability->getAbilityID()));
    if (!(row >= 0 && row < numRow && col >= 0 && col < numCol)) throw out_of_range(Err::invalidCoordinates);
    useFirewall(row, col);
    players[playerIndex]->useAbility(abilityNumber);
    checkGameOver(); // is this necessary? you can't win after placing a firewall
    notifyObservers();
}

void Game::useAbility(int abilityNumber, int r1, int c1, int r2, int c2) {
    const int numRow = (playerCount == 2) ? 8 : 10;
    int playerIndex = currentTurn - 1;
    shared_ptr<Ability> ability = players[playerIndex]->getAbility(abilityNumber);
    if (ability->getIsActivated()) throw runtime_error(Err::abilityAlreadyUsed(ability->getAbilityName(), ability->getAbilityID()));
    if (!(r1 >= 0 && r1 < numRow && c1 >= 0 && c1 < numCol) || !(r2 >= 0 && r2 < numRow && c2 >= 0 && c2 < numCol)) throw out_of_range(Err::invalidCoordinates);
    useWarp(r1, c1, r2, c2);
    players[playerIndex]->useAbility(abilityNumber);
    notifyObservers();
}

void Game::useAbility(int abilityNumber, const string &abilityName, char link) {
    int playerIndex = currentTurn - 1;
    shared_ptr<Ability> ability = players[playerIndex]->getAbility(abilityNumber);
    if (ability->getIsActivated()) throw runtime_error(Err::abilityAlreadyUsed(ability->getAbilityName(), ability->getAbilityID()));
    if (!validLink(link)) throw runtime_error(Err::invalidLink);
    if (abilityName == "LinkBoost") {
        useLinkBoost(link);
    } else if (abilityName == "Download") {
        useDownload(link);
    } else if (abilityName == "Polarise") {
        usePolarise(link);
    } else if (abilityName == "Scan") {
        useScan(link);
    } else if (abilityName == "Trojan") {
        useTrojan(link);
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
    // valid iff player moves off opponents edge of the board
    bool validOB = validOutOfBounds(newRow, newCol); // throws error if not valid bounds
    if (validOB) {
        oldCell.setContent('.');
        currLink->download();
        players[playerIndex]->download(currLink);
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
        warpCell.setWarp(false);
        newCell.setWarp(false);
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

    // cout << "Player " << currentTurn << "'s Turn" << endl;
    checkGameOver();
    notifyObservers();
}

void Game::useFirewall(int row, int col) {
    Cell &cell = board->getCell(row, col);
    // check if trying to place Firewall on a server port
    if (cell.isServerPort()) throw runtime_error(Err::cannotPlaceFirewallOnSP);
    // check if trying to place firewall on a firewall
    if (cell.hasFirewall()) throw runtime_error(Err::cannotPlaceFirewallOnFw);
    // TODO: check for teleport, trap
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
    
    if (cell1.isServerPort() || cell2.isServerPort()) throw runtime_error(Err::cannotUseAbilityonOtherAbility("Warp", "Server Port"));
    if (cell1.hasFirewall() || cell2.hasFirewall()) throw runtime_error(Err::cannotUseAbilityonOtherAbility("Warp", "Firewall"));
    if (!(cell1.isEmpty() && cell2.isEmpty())) throw runtime_error(Err::cannotUseAbilityonOtherAbility("Warp", "Link"));
    if (cell1.isWarp() || cell2.isWarp()) throw runtime_error(Err::cannotUseAbilityonOtherAbility("Warp", "Warp"));

    cell1.setWarp(true);
    cell1.setWarpCords(r2, c2);
    cell2.setWarp(true);
    cell2.setWarpCords(r1, c1);
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
                players[i]->setEliminated(true);
                --activePlayers;
                gameOver = (activePlayers > 1) ? false : true;
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

void Game::displayAbilities() const {
    for (const auto& o : observers) {
        if (auto textObserver = dynamic_pointer_cast<TextObserver>(o)) {
            if (textObserver->getPlayerNumber() == currentTurn) {
                textObserver->displayAbilities();
            }
        } else if (auto graphicsObserver = dynamic_pointer_cast<Graphics>(o)) {
            if (graphicsObserver->getPlayerNumber() == currentTurn) {
                graphicsObserver->displayAbilities();
            }
        }
    }
}

void Game::displayErr(const string &err) const {
    for (const auto& o : observers) {
        if (auto textObserver = dynamic_pointer_cast<TextObserver>(o)) {
            if (textObserver->getPlayerNumber() == currentTurn) {
                textObserver->displayError(err);
            }
        } else if (auto graphicsObserver = dynamic_pointer_cast<Graphics>(o)) {
            if (graphicsObserver->getPlayerNumber() == currentTurn) {
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
