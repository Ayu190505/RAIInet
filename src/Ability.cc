#include "Ability.h"
#include <iostream>
#include <vector>
#include "Err.h"
#include <string>

const std::string ValidAbilities = "LFDPSTIWOB";

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
            case 'O':
                ability = Abilities::Omit;
                break;
            case 'B':
                ability = Abilities::Block;
                break;
            default: throw(Err::invalidAbility() + ", hence " + std::to_string(code) + " is invalid.");
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
        case Abilities::Omit: return "Omit";
        case Abilities::Block: return "Block";
    }
    return "";
}

bool Ability::getIsActivated() { return isActivated; }

void Ability::useAbility(){
	if (isActivated) throw runtime_error(Err::abilityAlreadyUsed(getAbilityName(), getAbilityID()));
	else isActivated = !isActivated;
}

std::string Ability::getValidAbilities() {
    return ValidAbilities;
}

std::vector<std::string> Ability::getExpectedParams(const string &abilityName) {
    std::vector<std::string> expectedParams;

    // abilities applied to a coordinate
    if (abilityName == "Firewall" || abilityName == "Imprison" || abilityName == "Omit" || abilityName == "Block") {
        expectedParams = {"int", "int"};
    }
    // abilities applied to 2 coordinates
    else if (abilityName == "Warp") {
        expectedParams = {"int", "int", "int", "int"};
    }
    // abilities applied directly on a link
    else {
        expectedParams = {"char"};
    }

    return expectedParams;
}

