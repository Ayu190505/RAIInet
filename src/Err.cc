#include "Err.h"
#include <string>
#include "Ability.h"
#include <sstream>
// Argument errors here

string Err::insufficientArgs = "Insufficent arguments provided!";
string Err::invalidArg = "Invalid arguments provided!";
string Err::invalidCommandLineArg = 
    "Arguments must be of one of the forms: [-ability<index> <abilities>] [-link<index> <placementFile>] [graphics] [multipleDisplay] [player-four]. Invalid argument is: ";
string Err::invalidRuntimeCommand = 
"Invalid rumtime command provided\n Commands must be of one of the forms: [move <linkID> <dir>] [abilities] [ability <N>] [sequence <sequenceFile>] [board] [quit]";
string Err::invalidFile = "File not found: ";
string Err::expectedLinkAndDir = "Expected a link ID and direction to move";
string Err::invalidDirection = "Invalid direction provided. Directions may only be of the type [up] [down] [left] [right]";
string Err::reenterCommand = "Re-enter your command";
string Err::invalidPlayerNumber = "Cannot have players 3 or 4 for a two player game! Change player number to 4 by specifying [player-four] first!";

// Ability Errors
string Err::invalidAbilityIndex = "Expected an ability index from [1, 5]";
string Err::expectedCoordinatesForFireWall = "Expected coordinates from firewall in the form <row> <col>";
string Err::expectedCoordinatesForWarp = "Expected coordinates from warp in the form <row1> <col1> <row2> <col2>";
string Err::expectedLinkIdentity = "Expected a specification for which link to use ability on";
string Err::abilityUsedThisTurn = "You may only use one ability per turn. You must move a link before you use any more abilites.";
string Err::isAlreadyBoosted = "This link has already been boosted!";
string Err::isAlreadyDownloaded = "This link has already been downloaded!";
string Err::isAlreadyVisible = "This link is already visible!";
string Err::isAlreadyTrojan = "This link is already a Trojan!";
string Err::cannotPlaceFirewallOnSP = "You cannot place a firewall on a server port!";
string Err::cannotPlaceFirewallOnFw = "You cannot place a firewall on another firewall!";
string Err::cannotPlaceFirewallDirectlyOnOpp = "You cannot place a firewall directly on your opponent's link!";
string Err::cannotMoveImprisonedLink = "You cannot move an imprisoned Link!";

// Coordinate Errors
string Err::invalidCoordinates = "Invalid coordinates provided: outside the board";

// Link errors
string Err::invalidLinks = "Links may only be of the form [D1] [D2] [D3] [D4] [V1] [V2] [V3] [V4], and there must be exactly one of each form";
string Err::invalidLink = "Link IDs are from a-h for player 1 and A-H from player 2";

// Move errors
string Err::cannotMoveOpponentsLink = "You cannot move your opponent's link!";
string Err::cannotMoveDownloadedLink = "You cannot move a downloaded link!";
string Err::notInBounds = "Cannot move out of bounds from own edge or edge not belonging to any player";
string Err::cannotMoveOntoOwnLink = "Cannot move onto your own link";
string Err::cannotBattleImprisonedLink = "You can't battle an imprisoned Link";

// Functions
string Err::cannotDownloadOwnLink(bool SP) {
    if (SP) return "Cannot move to your own server port!";
    else return "Cannot move out of bounds from your own edge!";
} 
string Err::cannotUseAbilityOnOwnLink(const string &ability) {
    return "Cannot use " + ability + " on your own link!";
}
string Err::cannotUseAbilityOnOtherLink(const string &ability) {
    return "Cannot use " + ability + " on your opponent's link!";
}
string Err::abilityAlreadyUsed(const string &name, int index) {
    return name + " with id: " + std::to_string(index) +  " is already activated!";
}

string Err::cannotUseAbilityonOtherAbility(const string &ability, const string &otherAbility) {
    return "Cannot use " + ability + " on a cell with a " + otherAbility + "!";
}

// ability format helper
std::string formatAbilities(const std::string& abilities) {
    std::stringstream formattedAbilities;
    for (char ability : abilities) {
        formattedAbilities << "[" << ability << "] ";
    }
    std::string res = formattedAbilities.str();
    if (!res.empty()) res.pop_back();
    return res;
}

std::string Err::invalidAbilities() {
    std::string validAbilities = Ability::getValidAbilities();
    return "Abilities can only be of the form " + formatAbilities(validAbilities) + ", and you may have a total of 5 and a maximum of 2 of each ability!";
}

std::string Err::invalidAbility() {
    std::string validAbilities = Ability::getValidAbilities();
    return "Abilities can only be of the form " + formatAbilities(validAbilities);
}

std::string Err::abilityExpectsInputOf(const std::string &ability, const int paramNum, const std::string &input) {
    return "The parameter at position " + std::to_string(paramNum) + " for " + ability + " is expected to be " + input + '!';
}

std::string Err::cannotMoveLinkWithStatus(const string &status) {
    return "You cannot move a link that is " + status + '!';
}
