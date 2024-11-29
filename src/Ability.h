#ifndef ABILITY_H
#define ABILITY_H
#include <string>
#include <vector>

class Ability {
    int id;
    enum class Abilities {
        LinkBoost = 'L',
        Firewall = 'F',
        Download = 'D',
        Polarise = 'P',
        Scan = 'S', 
        Trojan = 'T', 
        Imprison = 'I', 
        Warp = 'W', 
        Omit = 'O',
        Block = 'B'
    };

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
        static std::vector<std::string> getExpectedParams(const std::string &abilityName);
};

#endif
