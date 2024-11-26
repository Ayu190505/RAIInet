#include "Ability.h"
#include <iostream>
#include "Err.h"

Ability::Ability(char code, int id) : id{id}, isActivated{false} {
    switch (code)
        {
            case 'L':   
                ability = Abilities::LinkBoost;
                break;
            case 'F':   
                ability = Abilities::Firewall;
                break;
            case 'D':   
                ability = Abilities::Download;
                break;
            case 'P':   
                ability = Abilities::Polarize;
                break;
            case 'S':   
                ability = Abilities::Scan;
                break;
            case 'T':   
                ability = Abilities::Teleport;
                break;
            case 'I':   
                ability = Abilities::Imprison;
                break;
            case 'U':   
                ability = Abilities::Undecided;
                break;
            default: throw(Err::invalidAbility + ", hence " + std::to_string(code) + " is invalid.");
        }
}

Ability::~Ability() {}

int Ability::getAbilityID() {
    return id;
}

std::string Ability::getAbilityName(){
    switch (ability)
    {
        case Abilities::LinkBoost:   return "LinkBoost";
        case Abilities::Firewall:   return "Firewall";
        case Abilities::Download: return "Download";
        case Abilities::Polarize:   return "Polarize";
        case Abilities::Scan:       return "Scan";
        case Abilities::Teleport:   return "Teleport";
        case Abilities::Imprison: return "Imprison";
        case Abilities::Undecided: return "Undecided";
    }
    return "";
    
}

bool Ability::getIsActivated() { return isActivated; }

void Ability::useAbility(){
	if (isActivated) throw runtime_error(Err::abilityAlreadyUsed(getAbilityName(), getAbilityID()));
	else isActivated = !isActivated;
}
