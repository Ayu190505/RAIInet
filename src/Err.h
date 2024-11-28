#ifndef ERR_H
#define ERR_H

#include <string>

using namespace std;

class Err {
    public:
    static string insufficientArgs;
    static string invalidArg;
    static string invalidCommandLineArg;
    static string invalidRuntimeCommand;
    static string invalidPlayerNumber;
    static string invalidAbilityIndex;
    static string expectedCoordinatesForFireWall;
    static string expectedCoordinatesForWarp;
    static string expectedLinkIdentity;
    static string invalidCoordinates;
    static string invalidLinks;
    static string invalidLink;
    static string expectedLinkAndDir;
    static string invalidDirection;
    static string invalidFile;
    static string abilityUsedThisTurn;
    static string reenterCommand;
    static string isAlreadyDownloaded;
    static string isAlreadyVisible;
    static string isAlreadyBoosted;
    static string isAlreadyTrojan;
    static string cannotPlaceFirewallOnSP;
    static string cannotPlaceFirewallOnFw;
    static string cannotPlaceFirewallDirectlyOnOpp;
    static string cannotMoveOpponentsLink;
    static string cannotMoveDownloadedLink;
    static string cannotMoveImprisonedLink;
    static string cannotBattleImprisonedLink;
    static string notInBounds;
    static string cannotMoveOntoOwnLink;

    static string cannotDownloadOwnLink(bool SP); // self server port and self side bound
    static string cannotUseAbilityOnOwnLink(const string &ability);
    static string cannotUseAbilityOnOtherLink(const string &ability);
    static string abilityAlreadyUsed(const string &name, int index);
    static string cannotUseAbilityonOtherAbility(const string &ability, const string &otherAbility);
    static std::string invalidAbilities();
    static std::string invalidAbility();
};

#endif
