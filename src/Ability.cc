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
                ability = Abilities::Polarise;
                break;
            case 'S':   
                ability = Abilities::Scan;
                break;
            case 'T':   
                ability = Abilities::Trojan;
                break;
            case 'I':   
                ability = Abilities::Imprison;
                break;
            case 'W':   
                ability = Abilities::Warp;
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
        case Abilities::Polarise:   return "Polarise";
        case Abilities::Scan:       return "Scan";
        case Abilities::Trojan:   return "Trojan";
        case Abilities::Imprison: return "Imprison";
        case Abilities::Warp: return "Warp";
    }
    return "";
    
}

bool Ability::getIsActivated() { return isActivated; }

void Ability::useAbility(){
	if (isActivated) throw runtime_error(Err::abilityAlreadyUsed(getAbilityName(), getAbilityID()));
	else isActivated = !isActivated;
}
