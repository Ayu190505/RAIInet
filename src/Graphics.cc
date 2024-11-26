#include "Graphics.h"
#include <sstream>
#include <vector>
#include <iostream>
#include "Ability.h"
#include <thread>
#include <chrono>

using namespace std;

const int windowWidth = 480;
const int windowHeight = 1000;
const int namePadding = 50;
const int cellSize = 80;
const auto nameColour = Xwindow::CornFlowerBlue;
const char p1BaseChar = 'a';
const char p2Basechar = 'A';
const int padding = 15;
const int abilitiesLen = 5;

struct ColorKey {
    string label;
    int color;
};

bool PiLink(char c, int playerNumber) {
    if (playerNumber == 1) {
        return c >= 'a' && c <= 'z';
    } else if (playerNumber == 2) {
        return c >= 'A' && c <= 'Z';
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
    string fontName = "-*-helvetica-bold-r-normal--" + std::to_string(desiredPixelSize) + "-*-*-*-*-*-*-*";
    // area around Raiinet
    w.fillRectangle(0, 0, 10, 70, nameColour);
    w.fillRectangle(windowWidth - 10, 0, 10, 70, nameColour);
    w.fillRectangle(0, 0, windowWidth, 10, nameColour);
    w.fillRectangle(0, 70, windowWidth, 10, nameColour);

    w.drawStringFont(100, 60, "RAIINET: P" + to_string(playerNumber), fontName, nameColour);
    vector<ColorKey> colorKeys = {
        {"Data", Xwindow::Green},
        {"Virus", Xwindow::Red},
        {"Unknown", Xwindow::Gray},
        {"Firewall", Xwindow::Orange},
        {"Teleport", Xwindow::Blue}
    };
    // draw keys

    int numKeys = colorKeys.size();
    int keyWidth = windowWidth / numKeys;
    int keyY = windowHeight - 300;
    for (int i = 0; i < numKeys; ++i) {
        int keyX = i * keyWidth;
        // Draw the label
        std::string label = colorKeys[i].label + ":";
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
    w.drawString(40, 110, "Player " + to_string(playerNumber) + ":");
    w.drawString(270, 110, "Player " + to_string((playerNumber % 2) + 1) + ":");
    w.drawString(40, 125, "Downloaded: D" + to_string(currPlayer->getDataDownloaded()) + ", V" + to_string(currPlayer->getVirusDownloaded()));
    w.drawString(270, 125, "Downloaded: D" + to_string(opp->getDataDownloaded()) + ", V" + to_string(opp->getVirusDownloaded()));
    w.drawString(40, 140, "Abilities: " + to_string(currPlayer->getRemainingAbilities()));
    w.drawString(270, 140, "Abilities: " + to_string(opp->getRemainingAbilities()));
    for (int i = 0; i < 2; ++i) {
        string lineI = "";
        for (int j = 0; j < 4; ++j) {
            char baseChar = playerNumber == 1 ? p1BaseChar : p2Basechar;
            char toPrint = baseChar + i * 4 + j;
            auto link = currPlayer->getLink(toPrint, playerNumber);
            lineI += toPrint;
            lineI += ": ";
            string type = (link->getType() == LinkType::Data) ? "D" : "V";
            lineI += type;
            lineI += to_string(link->getStrength());
            lineI += " ";
        }
        w.drawString(40, 155 + i * padding, lineI);
        lineI = "";
        for (int j = 0; j < 4; ++j) {
            char baseChar = playerNumber == 1 ? p2Basechar : p1BaseChar;
            char toPrint = baseChar + i * 4 + j;
            auto link = opp->getLink(toPrint, (playerNumber % 2) + 1);
            lineI += toPrint;
            lineI += ": ";
            if (!link->getIsVisible()) {
                lineI += "?? ";
            } else {
                string type = (link->getType() == LinkType::Data) ? "D" : "V";
                lineI += type;
                lineI += to_string(link->getStrength());
                lineI += " ";
            }
        }
        w.drawString(270, 155 + i * padding, lineI);
    }
    int size = board->getSize();
    int gridTop = 200;
    int gridBottom = windowHeight - 300;
    int gridHeight = gridBottom - gridTop;
    int gridWidth = windowWidth;

    int cellSize = 60;

    gridWidth = cellSize * size;
    gridHeight = cellSize * size;

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
    if (playerNumber == 1) {
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                int x = gridLeft + j * cellSize;
                int y = gridTop + i * cellSize;
                Cell &cell = board->getCell(i, j);
                int centerX = (x + cellSize / 2) - 2;
                int centerY = (y + cellSize / 2) + 3;
                if (cell.isServerPort()) {
                    w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::DarkGold);
                    w.drawString(centerX, centerY, "S");
                } else if (!cell.isEmpty()) {
                    char content = cell.getContent();
                    if (PiLink(content, playerNumber)) {
                        auto link = currPlayer->getLink(content, playerNumber);
                        if (link->getType() == LinkType::Data) w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Green);
                        else w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Red);
                        string s;
                        s+= content;
                        w.drawString(centerX, centerY, s);
                    } else {
                        // change
                        auto link = opp->getLink(content, (playerNumber % 2) + 1);
                        if (link->getIsVisible() && link->getType() == LinkType::Data) w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Green);
                        else if (link->getIsVisible()) w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Red);
                        else w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Gray);
                        string s;
                        s+= content;
                        w.drawString(centerX, centerY, s);
                    }
                } else if (cell.hasOwnFirewall(playerNumber)) {
                    w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Orange);
                    string fireWallString = (playerNumber == 1) ? "m" : "w";
                    w.drawString(centerX, centerY, fireWallString);
                } else if (cell.hasFirewall()) {
                    w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Orange);
                    string fireWallString = (playerNumber == 1) ? "w" : "m";
                    w.drawString(centerX, centerY, fireWallString);
                }
            }
        }
    } else {
        for (int i = size - 1; i >= 0; --i) {
            for (int j = 0; j < size; ++j) {
                int x = gridLeft + j * cellSize;
                int y = gridTop + (size - i - 1) * cellSize;
                Cell &cell = board->getCell(i, j);
                int centerX = (x + cellSize / 2) - 2;
                int centerY = (y + cellSize / 2) + 3;
                if (cell.isServerPort()) {
                    w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::DarkGold);
                    w.drawString(centerX, centerY, "S");
                } else if (!cell.isEmpty()) {
                    char content = cell.getContent();
                    if (PiLink(content, playerNumber)) {
                        auto link = currPlayer->getLink(content, playerNumber);
                        if (link->getType() == LinkType::Data) w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Green);
                        else w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Red);
                        string s;
                        s+= content;
                        w.drawString(centerX, centerY, s);
                    } else {
                        // change
                        auto link = opp->getLink(content, (playerNumber % 2) + 1);
                        if (link->getIsVisible() && link->getType() == LinkType::Data) w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Green);
                        else if (link->getIsVisible()) w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Red);
                        else w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Gray);
                        string s;
                        s+= content;
                        w.drawString(centerX, centerY, s);
                    }
                } else if (cell.hasOwnFirewall(playerNumber)) {
                    w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Orange);
                    string fireWallString = (playerNumber == 1) ? "m" : "w";
                    w.drawString(centerX, centerY, "m");
                } else if (cell.hasFirewall()) {
                    w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Orange);
                    string fireWallString = (playerNumber == 1) ? "w" : "m";
                    w.drawString(centerX, centerY, "w");
                }
            }
        }
    }
}


void Graphics::notify() {
    auto game = g.lock();
    if (!multipleDisplay) {
        playerNumber = game->getCurrentTurn();
        int desiredPixelSize = 50;
        string fontName = "-*-helvetica-bold-r-normal--" + std::to_string(desiredPixelSize) + "-*-*-*-*-*-*-*";
        w.fillRectangle(90, 20, windowWidth - 150, 40, Xwindow::White);
        w.drawStringFont(100, 60, "RAIINET: P" + to_string(playerNumber), fontName, nameColour);
    }
    auto board = game->getBoard();
    auto currPlayer = game->getPlayer(playerNumber);
    auto opp = game->getPlayer((playerNumber % 2) + 1);
    // draw player boxes
    w.fillRectangle(40, 95, 182, 90, Xwindow::White);
    w.fillRectangle(270, 95, 182, 90, Xwindow::White);
    w.drawString(40, 110, "Player " + to_string(playerNumber) + ":");
    w.drawString(270, 110, "Player " + to_string((playerNumber % 2) + 1) + ":");
    w.drawString(40, 125, "Downloaded: D" + to_string(currPlayer->getDataDownloaded()) + ", V" + to_string(currPlayer->getVirusDownloaded()));
    w.drawString(270, 125, "Downloaded: D" + to_string(opp->getDataDownloaded()) + ", V" + to_string(opp->getVirusDownloaded()));
    w.drawString(40, 140, "Abilities: " + to_string(currPlayer->getRemainingAbilities()));
    w.drawString(270, 140, "Abilities: " + to_string(opp->getRemainingAbilities()));
    for (int i = 0; i < 2; ++i) {
        string lineI = "";
        for (int j = 0; j < 4; ++j) {
            char baseChar = playerNumber == 1 ? p1BaseChar : p2Basechar;
            char toPrint = baseChar + i * 4 + j;
            auto link = currPlayer->getLink(toPrint, playerNumber);
            lineI += toPrint;
            lineI += ": ";
            string type = (link->getType() == LinkType::Data) ? "D" : "V";
            lineI += type;
            lineI += to_string(link->getStrength());
            lineI += " ";
        }
        w.drawString(40, 155 + i * padding, lineI);
        lineI = "";
        for (int j = 0; j < 4; ++j) {
            char baseChar = playerNumber == 1 ? p2Basechar : p1BaseChar;
            char toPrint = baseChar + i * 4 + j;
            auto link = opp->getLink(toPrint, (playerNumber % 2) + 1);
            lineI += toPrint;
            lineI += ": ";
            if (!link->getIsVisible()) {
                lineI += "?? ";
            } else {
                string type = (link->getType() == LinkType::Data) ? "D" : "V";
                lineI += type;
                lineI += to_string(link->getStrength());
                lineI += " ";
            }
        }
        w.drawString(270, 155 + i * padding, lineI);
    }
    int size = board->getSize();
    int gridTop = 200;
    int gridWidth = windowWidth;
    int cellSize = 60;
    gridWidth = cellSize * size;

    // Center the grid horizontally
    int gridLeft = (windowWidth - gridWidth) / 2;

    if (playerNumber == 1) {
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                int x = gridLeft + j * cellSize;
                int y = gridTop + i * cellSize;
                Cell &cell = board->getCell(i, j);
                int centerX = (x + cellSize / 2) - 2;
                int centerY = (y + cellSize / 2) + 3;
                if (cell.isServerPort()) {
                    w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::DarkGold);
                    w.drawString(centerX, centerY, "S");
                } else if (!cell.isEmpty()) {
                    char content = cell.getContent();
                    if (PiLink(content, playerNumber)) {
                        auto link = currPlayer->getLink(content, playerNumber);
                        if (link->getType() == LinkType::Data) w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Green);
                        else w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Red);
                        string s;
                        s+= content;
                        w.drawString(centerX, centerY, s);
                    } else {
                        // change
                        auto link = opp->getLink(content, (playerNumber % 2) + 1);
                        if (link->getIsVisible() && link->getType() == LinkType::Data) w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Green);
                        else if (link->getIsVisible()) w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Red);
                        else w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Gray);
                        string s;
                        s+= content;
                        w.drawString(centerX, centerY, s);
                    }
                } else if (cell.hasOwnFirewall(playerNumber)) {
                    w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Orange);
                    string fireWallString = (playerNumber == 1) ? "m" : "w";
                    w.drawString(centerX, centerY, "m");
                } else if (cell.hasFirewall()) {
                    w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Orange);
                    string fireWallString = (playerNumber == 1) ? "w" : "m";
                    w.drawString(centerX, centerY, "w");
                } else {
                    w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::White);
                }
            }
        }
    } else {
        for (int i = size - 1; i >= 0; --i) {
            for (int j = 0; j < size; ++j) {
                int x = gridLeft + j * cellSize;
                int y = gridTop + (size - i - 1) * cellSize;
                Cell &cell = board->getCell(i, j);
                int centerX = (x + cellSize / 2) - 2;
                int centerY = (y + cellSize / 2) + 3;
                if (cell.isServerPort()) {
                    w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::DarkGold);
                    w.drawString(centerX, centerY, "S");
                } else if (!cell.isEmpty()) {
                    char content = cell.getContent();
                    if (PiLink(content, playerNumber)) {
                        auto link = currPlayer->getLink(content, playerNumber);
                        if (link->getType() == LinkType::Data) w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Green);
                        else w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Red);
                        string s;
                        s+= content;
                        w.drawString(centerX, centerY, s);
                    } else {
                        // change
                        auto link = opp->getLink(content, (playerNumber % 2) + 1);
                        if (link->getIsVisible() && link->getType() == LinkType::Data) w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Green);
                        else if (link->getIsVisible()) w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Red);
                        else w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Gray);
                        string s;
                        s+= content;
                        w.drawString(centerX, centerY, s);
                    }
                } else if (cell.hasOwnFirewall(playerNumber)) {
                    w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Orange);
                    string fireWallString = (playerNumber == 1) ? "m" : "w";
                    w.drawString(centerX, centerY, fireWallString);
                } else if (cell.hasFirewall()) {
                    w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::Orange);
                    string fireWallString = (playerNumber == 1) ? "w" : "m";
                    w.drawString(centerX, centerY, fireWallString);
                } else {
                    w.fillRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2, Xwindow::White);
                }
            }
        }
    }
}

void Graphics::displayAbilities() {
    string abilities = "";
    auto game = g.lock();
    auto player = game->getPlayer(playerNumber);
    for (int i = 0; i < abilitiesLen - 1; ++i) {
        auto ability = player->getAbility(i + 1);
        abilities += to_string(ability->getAbilityID()) +  ": " + ability->getAbilityName() + "[" + (ability->getIsActivated() ? "Used]" : "NUsed] ");
    }
    auto ability = player->getAbility(abilitiesLen);
    abilities += to_string(ability->getAbilityID()) +  ": " + ability->getAbilityName() + "[" + (ability->getIsActivated() ? "Used]" : "NUsed]");
    string fontName = "lucidasans-10";
    w.drawStringFont(0, windowHeight - 300, abilities, fontName, Xwindow::Black);
    chrono::milliseconds timespan(3000);
    this_thread::sleep_for(timespan);
    w.fillRectangle(0, windowHeight - 315, windowWidth, 18, Xwindow::White);
}

int Graphics::getPlayerNumber() const {
    return playerNumber;
}

void Graphics::displayError(const string &err) {
    string fontName = "lucidasans-10";
    w.drawStringFont(0, windowHeight - 300, err, fontName, Xwindow::Black);
    chrono::milliseconds timespan(3000);
    this_thread::sleep_for(timespan);
    w.fillRectangle(0, windowHeight - 315, windowWidth, 18, Xwindow::White);
}