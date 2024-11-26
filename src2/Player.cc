#include "Player.h"
#include <iostream>
#include "Link.h"
#include "Ability.h"
#include "Err.h"

using namespace std;

const int individualLinkLength = 2;
const int serverPortStart = 6;
const int serverPortEnd = 10;
const int p1BaseRow = 0;
const int p2BaseRow = 7;
const int maxAbilityCount = 5;

Player::Player(const string &linksString, int size, const string &abilitiesString, int id) : id{id}, size{size}, numOfDataDownloaded{0}, numOfVirusDownloaded{0}, abilityCount{static_cast<int>(abilitiesString.length())} {
    // Assume links and abilities passed are valid since they are checked in main.
    int shift  = (size == 8) ? 0 : 1;
    char baseChar;
    int row;
    int col;
    int *primaryDirection;
    cout << id << endl;
    switch (id) {
        case 1: 
            baseChar = 'a';
            row = 0;
            col = shift; 
            primaryDirection = &col;
            break;
        case 2: 
            baseChar = 'A';
            row = size - 1;
            col = shift;
            primaryDirection = &col;
            break;
        case 3: 
            baseChar = 'i';
            row = shift;
            col = 0;
            primaryDirection = &row;
            break;
        case 4: 
            baseChar = 'I';
            row = shift;
            col = size - 1;
            primaryDirection = &row;
            break;
    }
    
    for (size_t i = 0; i < linksString.length(); i += individualLinkLength) {
        bool isServerPort = i >= serverPortStart && i < serverPortEnd;
        LinkType linkType = linksString[i] == 'D' ? LinkType::Data : LinkType::Virus;
        int strength = linksString[i + 1] - '0';
        char linkID = static_cast<char>(baseChar + (i / individualLinkLength));
        int linkRow = row;
        int linkCol = col;
        if (isServerPort) {
            switch (id) {
                case 1: 
                    ++linkRow;
                    break;
                case 2: 
                    --linkRow;
                    break;
                case 3: 
                    ++linkCol;
                    break;
                case 4: 
                    --linkCol;
                    break;
            }
        }

        links.emplace_back(make_shared<Link>(linkID, linkType, strength, linkRow, linkCol));
        ++(*primaryDirection);
    }

    for (int i = 0; i < maxAbilityCount; ++i) {
        abilities.emplace_back(make_shared<Ability>(abilitiesString[i], i + 1));
    }
}

string Player::getAbilityName(int i) {
    // issue is here, abilities does not exist
    return abilities[i - 1]->getAbilityName();
}

void Player::useAbility(int i) {
    abilities[i]->useAbility();
    --abilityCount;
}

void Player::download(shared_ptr<Link> &l) {
    if (l->getType() == LinkType::Data) {
        ++numOfDataDownloaded;
    } else {
        ++numOfVirusDownloaded;
    }
}

int Player::getDataDownloaded() const {
    return numOfDataDownloaded;
}

int Player::getVirusDownloaded() const {
    return numOfVirusDownloaded;
}

void Player::setDataDownloaded(int i) {
    numOfDataDownloaded = i;
}

void Player::setVirusDownloaded(int i) {
    numOfVirusDownloaded = i;
}

bool Player::getEliminated() const {
    return eliminated;
}

int Player::getRemainingAbilities() const {
    return abilityCount;
}

void Player::setEliminated(bool b) {
    eliminated = b;
}

shared_ptr<Ability> Player::getAbility(int abilityNumber) {
    if (abilityNumber > static_cast<int>(abilities.size())) throw runtime_error(Err::invalidAbilityIndex);
    return abilities[abilityNumber - 1];
}

shared_ptr<Link> Player::getLink(char c, int playerNumber) {
    char base;
    switch (playerNumber) {
        case 1: 
            base = 'a';
            break;
        case 2: 
            base = 'A';
            break;
        case 3: 
            base = 'i';
            break;
        case 4: 
            base = 'I';
            break;
    }
    int index = static_cast<int> (c - base);
    
    return links[index];
}
