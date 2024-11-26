#include "Subject.h"
#include "Observer.h"

void Subject::attach(const std::shared_ptr<Observer> &o) {
    observers.emplace_back(o);
}

void Subject::detach(const std::shared_ptr<Observer> &o) {
    for (auto it = observers.begin(); it != observers.end(); ) {
        if (*it == o) {
            it = observers.erase(it);
        } else {
            ++it;
        }
    }
}

void Subject::notifyObservers() {
    for (auto &o : observers) {
        o->notify();
    }
}
