#include "TextDisplay.h"
#include "Observer.h"
#include "Ability.h"
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <iomanip>

const int player1 = 1;
const int player2 = 2;
const int player3 = 3;
const int player4 = 4;
const int numberOfLines = 2;
const int numberOfLinksPerLine = 4;
const char p1Firewall = '^';
const char p2Firewall = 'v';
const char p3Firewall = '<';
const char p4Firewall = '>';
const char p1BaseChar = 'a';
const char p2BaseChar = 'A';
const char p3BaseChar = 'i';
const char p4BaseChar = 'I';
const char serverPort = 'S';
const int abilitiesLen = 5;

const int center = 23;

using namespace std;
int getCellPlayer(char link);
int getPlayerIndex(int player, vector<int> turns);
int mod(int x, int y);
char pnBaseChar(int n);
void player_line_void();

TextObserver::TextObserver(weak_ptr<Game> &g, ostream &out, ostream &err, int playerNumber, bool multipleDisplay) : 
    g{g}, out{out}, err{err}, playerNumber{playerNumber}, multipleDisplay{multipleDisplay} {}

void TextObserver::notify() {
    shared_ptr<Game> game = g.lock();
    if (!multipleDisplay) {
        playerNumber = game->getCurrentTurn();
    }
    auto board = game->getBoard();
    vector<int> turns_4{1, 4, 2, 3};
    vector<int> turns_2{1, 2};
    int playerCount = game->getPlayerCount(); 
    vector<int> turns = (playerCount == 4) ? turns_4 : turns_2;
 
    int bottom_shift = (playerCount == 4) ? 2 : 1;

    out << "Player " << game->getCurrentTurn() << "'s turn" << endl;
    int top = getPlayerIndex(playerNumber, turns);
    auto &top_player = game->getPlayer(turns[top]);

    int left = mod(top - 1, playerCount);
    auto &left_player = game->getPlayer(turns[left]);

    int right = mod(top + 1, playerCount);
    auto &right_player = game->getPlayer(turns[right]);

    int bottom = mod(top + bottom_shift, turns.size());
    auto &bottom_player = game->getPlayer(turns[bottom]);
    
    // top player printing
    if (playerCount == 4) out << std::string(center, ' ');
    player_line_1(turns[top]);
    out << endl;

    if (playerCount == 4) out << std::string(center, ' ');
    player_line_2(top_player->getDataDownloaded(), top_player->getVirusDownloaded());
    out << endl;

    if (playerCount == 4) out << std::string(center, ' ');
    player_line_3(top_player->getRemainingAbilities());
    out << endl;

    if (playerCount == 4) out << std::string(center, ' ');
    player_line_4_5(top_player, 0, turns[top]);
    out << endl;

    if (playerCount == 4) out << std::string(center, ' ');
    player_line_4_5(top_player, 1, turns[top]);
    out << endl;

    string s = "";
    for (int i = 0; i < board->getSize(); ++i) {
        s += "="; 
    }

    if (playerCount == 2) {out << s << endl;}
    else out << std::string(center, ' ') << s << endl;

    for (int i = 0; i < board->getSize(); ++i) {
        if (playerCount == 4) { // left player print
                switch(i) {
                    case 3: 
                        player_line_1(turns[left]);
                        break;
                    case 4: 
                        player_line_2(left_player->getDataDownloaded(), left_player->getVirusDownloaded());
                        break;
                    case 5: 
                        player_line_3(left_player->getRemainingAbilities());
                        break;
                    case 6: 
                        player_line_4_5(left_player, 0, turns[left]);
                        break;
                    case 7: 
                        player_line_4_5(left_player, 1, turns[left]);
                        break;
                    default: 
                        player_line_void();
                }
        }
        for (int j = 0; j < board->getSize(); ++j) {
            Cell &cell = board->getCell(i, j);
            int player = getCellPlayer(cell.getContent());
            if (cell.isLocked()) {
                out << setw(0) << ' ';
            } else if ((player != 0) && (game->getPlayer(player)->getEliminated())) {
                out << setw(0) << '.';
            } else if (cell.isServerPort()) {
                if (game->getPlayer(cell.getPlayersServerPort())->getEliminated()) out << '.';
                else out << setw(0) << serverPort;
            } else if (!cell.isEmpty()) {
                out << setw(0) << cell.getContent();
            } else if (cell.hasOwnFirewall(player1)){
                if (game->getPlayer(cell.getPlayersFirewall())->getEliminated()) out << '.';
                else out << setw(0) << p1Firewall;
            } else {
                out << setw(0) << cell.getContent();
            }
        }
        if (playerCount == 4) { // right player print
            out << "\t";
            switch(i) {
                case 3: 
                    player_line_1(turns[right]);
                    break;
                case 4: 
                    player_line_2(right_player->getDataDownloaded(), right_player->getVirusDownloaded());
                    break;
                case 5: 
                    player_line_3(right_player->getRemainingAbilities());
                    break;
                case 6: 
                    player_line_4_5(right_player, 0, turns[right]);
                    break;
                case 7: 
                    player_line_4_5(right_player, 1, turns[right]);
                    break;
                default: 
                    player_line_void();
            }
        }
        out << endl;
    }

    if (playerCount == 2) {out << s << endl;}
    else out << std::string(center, ' ') << s << endl;

    // top player printing
    if (playerCount == 4) out << std::string(center, ' ');
    player_line_1(turns[bottom]);
    out << endl;

    if (playerCount == 4) out << std::string(center, ' ');
    player_line_2(bottom_player->getDataDownloaded(), bottom_player->getVirusDownloaded());
    out << endl;

    if (playerCount == 4) out << std::string(center, ' ');
    player_line_3(bottom_player->getRemainingAbilities());
    out << endl;

    if (playerCount == 4) out << std::string(center, ' ');
    player_line_4_5(bottom_player, 0, turns[bottom]);
    out << endl;

    if (playerCount == 4) out << std::string(center, ' ');
    player_line_4_5(bottom_player, 1, turns[bottom]);
    out << endl << endl;
}

int getPlayerIndex(int player, vector<int> turns) {
    for (unsigned int i = 0; i < turns.size(); ++i) if (turns[i] == player) return i;
    return 0;
}

int mod(int x, int y) {return (x < 0) ? y -(-x % y) : x % y;} 

int getCellPlayer(char link) {
    if (link >= 'a' && link <= 'z') return 1;
    else if (link >= 'A' && link <= 'Z') return 2;
    else if (link >= 'n' && link <= 'u') return 3;
    else if (link >= 'N' && link <= 'U') return 4;
    else return 0;
}

void TextObserver::player_line_1(int num) {
    string s = "Player " + to_string(num) + ":";
    out << std::left << setw(center) << s;
}
void TextObserver::player_line_2(int data, int virus) {
    string s = "Downloaded: " + to_string(data) + "D, " + to_string(virus) + 'V';
    out << std::left << setw(center) << s;
}
void TextObserver::player_line_3(int abilities_rem) {
    string s = "Abilities: " + to_string(abilities_rem);
    out << std::left << setw(center) << s;
}
void TextObserver::player_line_4_5(const shared_ptr<Player>& player, int i, int n) { // use i = 0 -> line 4, use i = 1, line 5
    string s = "";
    for (int j = 0; j < numberOfLinksPerLine; ++j) {
        char toPrint = pnBaseChar(n) + i * numberOfLinksPerLine + j;
        shared_ptr<Link> currLink = player->getLink(toPrint, n);
        string type = "?";
        if ((n == playerNumber) || (currLink->getIsVisible())) {
            type = currLink->getType() == LinkType :: Data ? "D" : "V";
            type += to_string(currLink->getStrength());
        } 
        s += std::string(1, toPrint) + ": " + type + " ";
    }
    out << std::left << setw(center) << s;
}

void TextObserver::player_line_void() {
    out << std::string(center, ' ');
}

char pnBaseChar(int n) {
    switch(n) {
        case 1: return p1BaseChar;
        case 2: return p2BaseChar;
        case 3: return p3BaseChar;
        case 4: return p4BaseChar;
    }
    return '\0';
}

void TextObserver::displayAbilities() {
    string abilities = "";
    auto game = g.lock();
    auto player = game->getPlayer(playerNumber);
    for (int i = 0; i < abilitiesLen - 1; ++i) {
        auto ability = player->getAbility(i + 1);
        abilities += to_string(ability->getAbilityID()) +  ": " + ability->getAbilityName() + "[" + (ability->getIsActivated() ? "Used]" : "Unused], ");
    }
    auto ability = player->getAbility(abilitiesLen);
    abilities += to_string(ability->getAbilityID()) +  ": " + ability->getAbilityName() + "[" + (ability->getIsActivated() ? "Used]" : "Unused]");
    out << abilities << endl;
}

int TextObserver::getPlayerNumber() const {
    return playerNumber;
}

void TextObserver::displayError(const string &errMsg) {
    err << errMsg << endl;
}
