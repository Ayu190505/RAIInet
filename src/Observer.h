#ifndef OBSERVER_H
#define OBSERVER_H
#include <string>

class Observer {
    public:
    virtual void notify() = 0;
    virtual ~Observer() = default;
};

#endif
