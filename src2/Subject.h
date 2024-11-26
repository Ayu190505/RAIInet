#ifndef SUBJECT_H
#define SUBJECT_H
#include <vector>
#include <memory>

class Observer;

class Subject {
    protected:
    std::vector<std::shared_ptr<Observer>> observers;
    public:
    void attach(const std::shared_ptr<Observer> &o);
    void detach(const std::shared_ptr<Observer> &o);
    void notifyObservers();
};

#endif