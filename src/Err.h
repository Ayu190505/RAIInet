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
    static string invalidAbilities;
    static string invalidAbility;
    static string invalidAbilityIndex;
    static string expectedCoordinatesForFireWall;
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
    static string cannotPlaceFirewallOnSP;
    static string cannotPlaceFirewallOnFw;
    static string cannotPlaceFirewallDirectlyOnOpp;
    static string cannotMoveOpponentsLink;
    static string cannotMoveDownloadedLink;
    static string notInBounds;
    static string cannotMoveOntoOwnLink;
    static string cannotDownloadOwnLink(bool SP); // self server port and self side bound
    static string cannotUseAbilityOnOwnLink(const string &ability);
    static string cannotUseAbilityOnOtherLink(const string &ability);
    static string abilityAlreadyUsed(const string &name, int index);
    
};

#endif
