#include "Graphics.h"
#include <sstream>
#include <vector>
#include "Ability.h"
#include <thread>
#include <chrono>

using namespace std;

const int windowWidth = 480;
const int windowHeight = 800;
const int namePadding = 50;
const auto nameColour = Xwindow::CornFlowerBlue;
const char p1Firewall = '^';
const char p2Firewall = 'v';
const char p3Firewall = '<';
const char p4Firewall = '>';
const char p1BaseChar = 'a';
const char p2Basechar = 'A';
const char p3BaseChar = 'i';
const char p4BaseChar = 'I';
const char serverPort = 'S';
const char blockedServerPort = '$';
const char warpChar = '*';
const char imprisonChar = '@';
const int player1 = 1;
const int player2 = 2;
const int player3 = 3;
const int player4 = 4;
const int padding = 15;
const int abilitiesLen = 5;

struct ColorKey {
    string label;
    int color;
};

bool PiLink(char c, int playerNumber) {
    if (playerNumber == 1) {
        return c >= 'a' && c <= 'h';
    } else if (playerNumber == 2) {
        return c >= 'A' && c <= 'H';
    } else if (playerNumber == 3) {
        return  c >= 'i' && c <= 'p';
    } else if (playerNumber == 4) {
        return  c >= 'I' && c <= 'P';
    } 
    return false;
}

Graphics::Graphics(weak_ptr<Game> &g, int playerNumber, bool multipleDisplay) : 
    g{g}, playerNumber{playerNumber}, multipleDisplay{multipleDisplay}, w{windowWidth, windowHeight} {
    auto game = g.lock();
    auto board = game->getBoard();
    auto currPlayer = game->getPlayer(playerNumber);
    auto opp = game->getPlayer((playerNumber % 2) + 1);
    int desiredPixelSize = 50;
    string fontName = "-*-helvetica-bold-r-normal--" + to_string(desiredPixelSize) + "-*-*-*-*-*-*-*";
    // area around Raiinet
    w.fillRectangle(0, 0, 10, 70, nameColour);
    w.fillRectangle(windowWidth - 10, 0, 10, 70, nameColour);
    w.fillRectangle(0, 0, windowWidth, 10, nameColour);
    w.fillRectangle(0, 70, windowWidth, 10, nameColour);

    w.drawStringFont(100, 60, "RAIINET: P" + to_string(playerNumber), fontName, nameColour);
    vector<ColorKey> colorKeys = {
        {"Data", Xwindow::Green},
        {"Virus", Xwindow::Red},
        {"Firewall", Xwindow::Orange},
        {"Warp", Xwindow::Blue},
        {"Imprison", Xwindow::Brown}
    };
    // draw keys

    int numKeys = colorKeys.size();
    int keyWidth = windowWidth / numKeys;
    int keyY = windowHeight - 80;
    for (int i = 0; i < numKeys; ++i) {
        int keyX = i * keyWidth;
        // Draw the label
        string label = colorKeys[i].label + ":";
        int labelX = keyX + 10;
        int labelY = keyY + 30;
        w.drawString(labelX, labelY, label, Xwindow::Black);
        // Draw the color box next to the label
        int boxSize = 20;
        int boxX = labelX + 60;
        int boxY = keyY + 15;
        w.fillRectangle(boxX, boxY, boxSize, boxSize, colorKeys[i].color);
    }

    // draw player boxes
    w.drawRectangle(25, 90, 200, 100);
    w.drawRectangle(255, 90, 200, 100);
    if (game->getPlayerCount() > 2) {
        w.drawRectangle(25, 610, 200, 100);
        w.drawRectangle(255, 610, 200, 100);
    }
    int size = board->getSize();
    int gridTop = 200;
    int gridBottom = windowHeight - 300;
    int gridHeight = gridBottom - gridTop;
    int gridWidth = windowWidth;

    int cellSize = game->getPlayerCount() == 4 ? 40 : 60;

    gridWidth = cellSize * size;
    gridHeight = cellSize * size;

    /*----- -----
    P1   | P2
    ----- -----
    ----- -----
    P3   | P4
    ----- -----*/
    // Center the grid horizontally
    int gridLeft = (windowWidth - gridWidth) / 2;

    // Draw horizontal lines
    for (int i = 0; i <= size; ++i) {
        int y = gridTop + i * cellSize;
        w.drawLine(gridLeft, y, gridLeft + gridWidth, y, Xwindow::Black);
    }

    // Draw vertical lines
    for (int i = 0; i <= size; ++i) {
        int x = gridLeft + i * cellSize;
        w.drawLine(x, gridTop, x, gridTop + gridHeight, Xwindow::Black);
    }
    notify();
}


void Graphics::notify() {
    int currPlayerNumber = playerNumber;
    auto game = g.lock();
    if (!multipleDisplay) {
        currPlayerNumber = game->getCurrentTurn();
        int desiredPixelSize = 50;
        string fontName = "-*-helvetica-bold-r-normal--" + to_string(desiredPixelSize) + "-*-*-*-*-*-*-*";
        w.fillRectangle(90, 20, windowWidth - 150, 40, Xwindow::White);
        w.drawStringFont(100, 60, "RAIINET: P" + to_string(currPlayerNumber), fontName, nameColour);
    }
    auto board = game->getBoard();
    auto currPlayer = game->getPlayer(currPlayerNumber);
    // draw player boxes
    auto p1 = game->getPlayer(player1);
    auto p2 = game->getPlayer(player2);
    w.fillRectangle(40, 95, 182, 90, Xwindow::White);
    w.fillRectangle(270, 95, 182, 90, Xwindow::White);
    w.drawString(40, 110, "Player " + to_string(player1) + ": " + (p1->getEliminated() ? "(X_X)" : ""));
    w.drawString(270, 110, "Player " + to_string(player2) + ": " + (p2->getEliminated() ? "(X_X)" : ""));
    w.drawString(40, 125, "Downloaded: " + to_string(p1->getDataDownloaded()) + "D, " + to_string(p1->getVirusDownloaded()) + "V");
    w.drawString(270, 125, "Downloaded: " + to_string(p2->getDataDownloaded()) + "D, " + to_string(p2->getVirusDownloaded()) + "V");
    w.drawString(40, 140, "Abilities: " + to_string(p1->getRemainingAbilities()));
    w.drawString(270, 140, "Abilities: " + to_string(p2->getRemainingAbilities()));
    if (game->getPlayerCount() > 2) {
        auto p3 = game->getPlayer(player3);
        auto p4 = game->getPlayer(player4);
        w.fillRectangle(40, 615, 182, 90, Xwindow::White);
        w.fillRectangle(270, 615, 182, 90, Xwindow::White);
        w.drawString(40, 630, "Player " + to_string(player3) + ": " + (p3->getEliminated() ? "(X_X)" : ""));
        w.drawString(270, 630, "Player " + to_string(player4) + ": " + (p4->getEliminated() ? "(X_X)" : ""));
        w.drawString(40, 645, "Downloaded: " + to_string(p3->getDataDownloaded()) + "D, " + to_string(p3->getVirusDownloaded()) + "V");
        w.drawString(270, 645, "Downloaded: " + to_string(p4->getDataDownloaded()) + "D, " + to_string(p4->getVirusDownloaded()) + "V");
        w.drawString(40, 660, "Abilities: " + to_string(p3->getRemainingAbilities()));
        w.drawString(270, 660, "Abilities: " + to_string(p4->getRemainingAbilities()));
    }

    for (int i = 0; i < 2; ++i) {
        string lineI = "";
        for (int j = 0; j < 4; ++j) {
            char baseChar = p1BaseChar;
            char toPrint = baseChar + i * 4 + j;
            auto link = p1->getLink(toPrint, player1);
            lineI += toPrint;
            lineI += ": ";
            if (player1 == currPlayerNumber || link->getIsVisible() || p1->getEliminated()) {
                string type = (link->getType() == LinkType::Data) ? "D" : "V";
                lineI += type;
                lineI += to_string(link->getStrength());
                lineI += " ";
            } else {
                lineI += "?? ";
            }
        }
        w.drawString(40, 155 + i * padding, lineI);
        lineI = "";
        for (int j = 0; j < 4; ++j) {
            char baseChar = p2Basechar;
            char toPrint = baseChar + i * 4 + j;
            auto link = p2->getLink(toPrint, player2);
            lineI += toPrint;
            lineI += ": ";
            if (player2 == currPlayerNumber || link->getIsVisible() || p2->getEliminated()) {
                string type = (link->getType() == LinkType::Data) ? "D" : "V";
                lineI += type;
                lineI += to_string(link->getStrength());
                lineI += " ";
            } else {
                lineI += "?? ";
            }
        }
        w.drawString(270, 155 + i * padding, lineI);
    }

    if (game->getPlayerCount() > 2) {
        auto p3 = game->getPlayer(player3);
        auto p4 = game->getPlayer(player4);
        for (int i = 0; i < 2; ++i) {
        string lineI = "";
        for (int j = 0; j < 4; ++j) {
            char baseChar = p3BaseChar;
            char toPrint = baseChar + i * 4 + j;
            auto link = p3->getLink(toPrint, player3);
            lineI += toPrint;
            lineI += ": ";
            if (player3 == currPlayerNumber || link->getIsVisible() || p3->getEliminated()) {
                string type = (link->getType() == LinkType::Data) ? "D" : "V";
                lineI += type;
                lineI += to_string(link->getStrength());
                lineI += " ";
            } else {
                lineI += "?? ";
            }
        }
        w.drawString(40, 675 + i * padding, lineI);
        lineI = "";
        for (int j = 0; j < 4; ++j) {
            char baseChar = p4BaseChar;
            char toPrint = baseChar + i * 4 + j;
            auto link = p4->getLink(toPrint, player4);
            lineI += toPrint;
            lineI += ": ";
            if (player4 == currPlayerNumber || link->getIsVisible() || p4->getEliminated()) {
                string type = (link->getType() == LinkType::Data) ? "D" : "V";
                lineI += type;
                lineI += to_string(link->getStrength());
                lineI += " ";
            } else {
                lineI += "?? ";
            }
        }
        w.drawString(270, 675 + i * padding, lineI);
    }

    }

    int size = board->getSize();
    int gridTop = 200;
    int gridWidth = windowWidth;
    int cellSize = game->getPlayerCount() == 4 ? 40 : 60;
    gridWidth = cellSize * size;

    // Center the grid horizontally
    int gridLeft = (windowWidth - gridWidth) / 2;

    for (int i = newRow(size); valid(i, true, size); update(i, true)) {
        for (int j = newCol(size); valid(j, false, size); update(j, false)) {
            int x, y;
            if (playerNumber == 1) {
                x = gridLeft + j * cellSize;
                y = gridTop + i * cellSize;
            } else if (playerNumber == 2){
                x = gridLeft + (size - 1 - j) * cellSize; // j: 0 to n - 1
                y = gridTop + (size - 1 - i) * cellSize; // i: n-1 to 0
            } else if (playerNumber == 3) {
                x = gridLeft + (size - 1 - j) * cellSize; 
                y = gridTop + i * cellSize;
            } else {
                x = gridLeft + j * cellSize;
                y = gridTop + (size - 1 - i) * cellSize; 
            }

            Cell cell;
            if (playerNumber > 2) cell = board->getCell(j, i);
            else cell = board->getCell(i, j);
            int centerX = (x + cellSize / 2) - 2;
            int centerY = (y + cellSize / 2) + 3;
            if (cell.isLocked()) {
                w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Black);      
            } else if (cell.isServerPort()) {
                w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::DarkGold);
                if (cell.isOwnServerPort(currPlayerNumber) && cell.getBlocked()) w.drawString(centerX, centerY, "$");
                else w.drawString(centerX, centerY, "S");
            } else if (!cell.isEmpty()) {
                char content = cell.getContent();
                if (PiLink(content, currPlayerNumber)) {
                    auto link = currPlayer->getLink(content, currPlayerNumber);
                    if (link->getType() == LinkType::Data) w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Green);
                    else w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Red);
                    string s;
                    s+= content;
                    w.drawString(centerX, centerY, s);
                } else {
                    int oppNumber = 0;
                    for (int i = 1; i <= game->getPlayerCount(); ++i) {
                        if (PiLink(content, i)) {
                            oppNumber = i;
                            break;
                        }
                    }
                    auto opp = game->getPlayer(oppNumber);
                    auto link = opp->getLink(content, oppNumber);
                    if (link->getIsVisible() && link->getType() == LinkType::Data) w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Green);
                    else if (link->getIsVisible()) w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Red);
                    else w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Gray);
                    string s;
                    s+= content;
                    w.drawString(centerX, centerY, s);
                }
            } else if (cell.hasFirewall()) {
                int playerFirewall = cell.getPlayersFirewall();
                w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Orange);
                string fireWallString = "";
                if (playerFirewall == player1) {
                    fireWallString += p1Firewall;
                } else if(playerFirewall == player2) {
                    fireWallString += p2Firewall;
                } else if (playerFirewall == player3) {
                    fireWallString += p3Firewall;
                } else {
                    fireWallString += p4Firewall;
                }
                w.drawString(centerX, centerY, fireWallString);
            } else if (cell.isWarp()) {
                w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Blue);
                string warpString = "";
                warpString += warpChar;
                w.drawString(centerX, centerY, warpString);
            } else if (cell.isImprison()) {
                int whichPlayersImprison = cell.getWhichPlayersImprison();
                if (whichPlayersImprison == game->getCurrentTurn()) {
                    w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Brown);
                    string imprisonString = "";
                    imprisonString += imprisonChar;
                    w.drawString(centerX, centerY, imprisonString);
                } else {
                    w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::White);          
                }
            }
            else {
                w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::White);
            }
        }
    }
}

void Graphics::displayAbilities() {
    string abilities = "";
    auto game = g.lock();
    int currPlayer = playerNumber;
    if (!multipleDisplay) {
        currPlayer = game->getCurrentTurn();
    }
    auto player = game->getPlayer(currPlayer);
    for (int i = 0; i < abilitiesLen - 1; ++i) {
        auto ability = player->getAbility(i + 1);
        abilities += to_string(ability->getAbilityID()) +  ": " + ability->getAbilityName() + "[" + (ability->getIsActivated() ? "Used]" : "X] ");
    }
    auto ability = player->getAbility(abilitiesLen);
    abilities += to_string(ability->getAbilityID()) +  ": " + ability->getAbilityName() + "[" + (ability->getIsActivated() ? "Used]" : "X]");
    string fontName = "-*-helvetica-bold-r-normal--10-*-*-*-*-*-*-*";
    w.drawStringFont(0, windowHeight - 75, abilities, fontName, Xwindow::Black);
    chrono::milliseconds timespan(3000);
    this_thread::sleep_for(timespan);
    w.fillRectangle(0, windowHeight - 85, windowWidth, 18, Xwindow::White);
}

int Graphics::getPlayerNumber() const {
    return playerNumber;
}

void Graphics::displayError(const string &err) {
    auto game = g.lock();
    string fontName = "-*-helvetica-bold-r-normal--10-*-*-*-*-*-*-*";
    w.drawStringFont(0, windowHeight - 75, err, fontName, Xwindow::Black);
    chrono::milliseconds timespan(3000);
    this_thread::sleep_for(timespan);
    w.fillRectangle(0, windowHeight - 85, windowWidth, 18, Xwindow::White);
}

void Graphics::displayGameOver() {
    auto game = g.lock();
    w.fillRectangle(0, 0, windowWidth, windowHeight, Xwindow::DarkGold);
    int desiredPixelSize = 50;
    string fontName = "-*-helvetica-bold-r-normal--" + to_string(desiredPixelSize) + "-*-*-*-*-*-*-*";
    string toDisplay = "Player " + to_string(game->getPlayerWon()) + " Wins!";
    w.drawStringFont(50, 200, "Congratulations!!", fontName);
    w.drawStringFont(80, 300, toDisplay, fontName);
    chrono::milliseconds timespan(5000);
    this_thread::sleep_for(timespan);
}

int Graphics::newRow(int size) const { 
    switch (playerNumber) {
        case 1:
            return 0;
            break;
        case 2:
            return size - 1;
            break;
        case 3:
            return 0;
            break;
        case 4:
            return size - 1;
            break;
    }
    return 0;
}

int Graphics::newCol(int size) const { 
    switch (playerNumber) {
        case 1:
            return 0;
            break;
        case 2:
            return size - 1;
            break;
        case 3:
            return size - 1;
            break;
        case 4:
            return 0;
            break;
    }
    return 0;
}

bool Graphics::valid(int num, bool row, int size) const {
    bool b = true;
    switch (playerNumber) {
        case 1:
            b = num < size;
            break;
        case 2:
            b = num >= 0;
            break;
        case 3:
            if (row) b = num < size;
            else b = num >= 0;
            break;
        case 4:
            if (row) b = num >= 0;
            else b = num < size;
            break;
    }
    
    return b;
}

void Graphics::update(int &num, bool row) const {
    int shift = 0;
    switch (playerNumber) {
        case 1:
            shift = 1;
            break;
        case 2:
            shift = -1;
            break;
        case 3:
            shift = row ? 1 : -1;
            break;
        case 4:
            shift = row ? -1 : 1;
            break; 
    }

    num += shift;
}

