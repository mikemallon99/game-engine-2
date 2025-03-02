#pragma once

#include <iostream>
#include <map>
#include <array>

#include "../keyboard.h"
#include "../Gui/log_gui.h"
#include "../shader.h"

enum GagType {
    TRAP,
    LURE,
    SOUND,
    THROW,
    SQUIRT,
    DROP,
};

struct Gag {
    int damage;
    float accuracy;
};

struct GagTrack {
    GagType gagType;
    Gag allGags[6];
    int xpLevels[5];
    int curLevel;
    int curXP;
    int isActive;
};

std::map<GagType, GagTrack> initializeGagTracks() {
    std::map<GagType, GagTrack> gagTracks;

    // Example values for each GagTrack
    gagTracks[TRAP] = {
        TRAP,
        {{10, 0.95}, {20, 0.90}, {30, 0.85}, {40, 0.80}, {50, 0.75}, {60, 0.70}}, // allGags
        {20, 100, 500, 1000, 2500}, // xpLevels
        2, // curLevel
        150, // curXP
        false,
    };

    gagTracks[LURE] = {
        LURE,
        {{0, 0.95}, {0, 0.90}, {0, 0.85}, {0, 0.80}, {0, 0.75}, {0, 0.70}}, // allGags
        {20, 100, 500, 1000, 2500}, // xpLevels
        1, // curLevel
        80, // curXP
        false,
    };

    gagTracks[SOUND] = {
        SOUND,
        {{5, 0.95}, {10, 0.90}, {15, 0.85}, {20, 0.80}, {25, 0.75}, {30, 0.70}}, // allGags
        {20, 100, 500, 1000, 2500}, // xpLevels
        3, // curLevel
        600, // curXP
        false,
    };

    gagTracks[THROW] = {
        THROW,
        {{8, 0.95}, {16, 0.90}, {24, 0.85}, {32, 0.80}, {40, 0.75}, {48, 0.70}}, // allGags
        {20, 100, 500, 1000, 2500}, // xpLevels
        4, // curLevel
        900, // curXP
        true,
    };

    gagTracks[SQUIRT] = {
        SQUIRT,
        {{7, 0.95}, {14, 0.90}, {21, 0.85}, {28, 0.80}, {35, 0.75}, {42, 0.70}}, // allGags
        {20, 100, 500, 1000, 2500}, // xpLevels
        3, // curLevel
        550, // curXP
        true,
    };

    gagTracks[DROP] = {
        DROP,
        {{20, 0.80}, {40, 0.75}, {60, 0.70}, {80, 0.65}, {100, 0.60}, {120, 0.55}}, // allGags
        {20, 100, 500, 1000, 2500}, // xpLevels
        2, // curLevel
        300, // curXP
        false,
    };

    return gagTracks;
}

std::map<GagType, std::array<int, 6>> initializeGagInventory() {
    std::map<GagType, std::array<int, 6>> gagInventory;

    gagInventory[TRAP].fill(0);
    gagInventory[LURE].fill(0);
    gagInventory[SOUND].fill(0);
    gagInventory[THROW].fill(0);
    gagInventory[THROW][0] = 5;
    gagInventory[SQUIRT].fill(0);
    gagInventory[SQUIRT][0] = 5;
    gagInventory[DROP].fill(0);

    return gagInventory;
}

enum BattleState {
    PLAYER_PICKING,
    PLAYER_ATTACK,
    ENEMY_ATTACK
};

class Battle {
public:
    bool battleActive;
    BattleState curState;
    KeyboardDebouncer kbd;
    
    // Player battle system data
    std::map<GagType, GagTrack> playerGagTracks;
    std::map<GagType, std::array<int, 6>> playerGagInventory;
    int playerMaxHealth;
    int playerCurHealth;
    GagType gagTypeSel;
    int trackIdxSel;

    // Enemy battle data
    int enemyMaxHealth;
    int enemyCurHealth;

    Battle() {
        playerGagTracks = initializeGagTracks();
        playerGagInventory = initializeGagInventory();
        playerMaxHealth = 15;
        playerCurHealth = 11;

        enemyMaxHealth = 15;
        enemyCurHealth = 10;

        curState = PLAYER_PICKING;

        kbdMgr.registerKeyboard(&kbd);
    }

    void ProcessKeyboard() {
        if (curState == PLAYER_PICKING) {
            if (kbd.checkKey(GLFW_KEY_P)) {
                // Just select a lvl 1 throw gag
                selectGagCallback(THROW, 0);
            }
        }
    }

    void selectGagCallback(GagType gagType, int trackIdx) {
        gagTypeSel = gagType;
        trackIdxSel = trackIdx;
        curState = PLAYER_ATTACK;
    }

    void Update() {
        if (curState == PLAYER_PICKING) {
            // Prompt player to select gag
            // How to handle drawing to the screen? Player gui + gag selection?
            // The gag selection will occur once the player left clicks on a gag, so the call will happen in the controller
            // Depending on the cur state, the draw call will be different. So I suppose the draw should happen here
            // Also for now, we can put gag selection into the debug menu. Debug menu will need a callback function
            /*logGui.Write("Player picking gag");*/
            ProcessKeyboard();
        }
        else if (curState == PLAYER_ATTACK) {
            logGui.Write("Player attacking");
            // Do gag logic here

            // Sutract one from gag inventory
            playerGagInventory[gagTypeSel][trackIdxSel]--;
            // Accumulate XP value of the gag + multiplier
            // Do accuracy roll to see if gag misses
            // Execute gag function (just damage for throw/squirt)
            int dmg = playerGagTracks[gagTypeSel].allGags[trackIdxSel].damage;
            std::ostringstream oss;
            enemyCurHealth -= dmg;
            oss << "Enemy takes " << dmg << " damage: " << enemyCurHealth << "/" << enemyMaxHealth << " HP.";
            logGui.Write(oss.str());
            // Update GUI based on what happened
            // Change battle state to enemy turn

            curState = ENEMY_ATTACK;
        }
        else if (curState == ENEMY_ATTACK) {
            // Enemy selects attack based off AI logic
            // Once player selects an attack and it hits the enemy, there should be a short pause so it feels like the enemy is doing an attack
            // How to have a time.wait() type thing, without stalling the game?

            logGui.Write("Enemy attacking");

            int dmg = 3;
            playerCurHealth -= dmg;
            std::ostringstream oss;
            oss << "Player takes " << dmg << " damage: " << playerCurHealth << "/" << playerMaxHealth << " HP.";
            logGui.Write(oss.str());
            curState = PLAYER_PICKING;
        }
    }
};
