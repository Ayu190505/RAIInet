#include "Link.h"
#include <sstream>
#include <stdexcept>
#include "Err.h"

Link::Link(char id, LinkType t, int strength, int row, int col) : 
    id{id}, t{t}, strength{strength}, row{row}, col{col}, stepSize{1}, isVisible{false}, isDownloaded{false} {}

char Link::getId() const {
    return id;
}

LinkType Link::getType() const {
    return t;
}

int Link::getStrength() const {
    return strength;
}

int Link::getRow() const {
    return row;
}

int Link::getCol() const {
    return col;
}

bool Link::getIsVisible() const {
    return isVisible;
}

bool Link::getIsDownloaded() const {
    return isDownloaded;
}

int Link::getStepSize() const {
    return stepSize;
}

void Link::setRow(int r) {
    if (!isDownloaded) row = r;
}

void Link::setCol(int c) {
    if (!isDownloaded) col = c;
}
void Link::setStrength(int s) {
    if (!isDownloaded) strength = s;
}


void Link::setIsVisible(bool isVisible) {
    this->isVisible = isVisible;
}

void Link::linkBoost() {
    if (stepSize == 2) throw runtime_error(Err::isAlreadyBoosted);
    stepSize += 1;
}

void Link::polarise() {
    t = (t == LinkType::Data) ? LinkType::Virus : LinkType::Data;
}

void Link::scan() {
    if (isVisible) throw runtime_error(Err::isAlreadyVisible);
    isVisible = true;
}

void Link::download() {
    if (isDownloaded) throw runtime_error(Err::isAlreadyDownloaded);
    isDownloaded = true;
    isVisible = true;
}
    