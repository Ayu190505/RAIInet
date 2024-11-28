#ifndef ABILITY_H
#define ABILITY_H
#include <string>

class Ability {
    int id;
    enum class Abilities {LinkBoost = 'L', Firewall = 'F', Download = 'D', Polarise = 'P', Scan = 'S', Trojan = 'T', Imprison = 'I', Warp = 'W'};
    Abilities ability;
    bool isActivated;
    public:
        Ability(char code, int id);
        ~Ability();

        // getters
        bool getIsActivated();
        int getAbilityID();

        // other
        std::string getAbilityName(); 
        void useAbility();
        static std::string getValidAbilities();
};

#endif
