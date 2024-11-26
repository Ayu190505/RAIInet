#ifndef OBSERVER_H
#define OBSERVER_H
#include <string>

class Observer {
    public:
    virtual void notify() = 0;
    virtual int getPlayerNumber() const = 0;
    //virtual void displayAbilities() = 0;
    virtual void displayError(const std::string &err) = 0;
    virtual ~Observer() = default;
};

#endif

