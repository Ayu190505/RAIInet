#ifndef ABILITY_H
#define ABILITY_H
#include <string>

class Ability {
    int id;
    enum class Abilities {LinkBoost = 'L', Firewall = 'F', Download = 'D', Polarize = 'P', Scan = 'S', Teleport = 'T', Imprison = 'I', Undecided = 'U'};
    Abilities ability;
    bool isActivated;
    public:
        Ability(char code, int id);
        ~Ability();
        int getAbilityID();
        std::string getAbilityName(); 
        bool getIsActivated();
        void useAbility();
};

#endif
