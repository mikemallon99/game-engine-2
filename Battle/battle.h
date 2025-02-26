#pragma once

#include "gags.h"

enum BattleState {
    PLAYER_TURN,
    ENEMY_TURN
}

class BattleData {
    bool battleActive;
    BattleState curState;
    PlayerBattleData playerBattleData;

    void selectGagCallback(GagType gagType, int trackIdx) {
        if (!battleActive || curState != PLAYER_TURN) {
            return;
        }

        if (playerBattleData.gagInventory[gagType][trackIdx] <= 0) {
            return;
        }

        // Do gag logic here

        // Sutract one from gag inventory
        // Accumulate XP value of the gag + multiplier
        // Do accuracy roll to see if gag misses
        // Execute gag function (just damage for throw/squirt)
        // Update GUI based on what happened
        // Change battle state to enemy turn
    }

    void Update() {
        if (curState == PLAYER_TURN) {
            // Prompt player to select gag
            // How to handle drawing to the screen? Player gui + gag selection?
            // The gag selection will occur once the player left clicks on a gag, so the call will happen in the controller
            // Depending on the cur state, the draw call will be different. So I suppose the draw should happen here
            // Also for now, we can put gag selection into the debug menu. Debug menu will need a callback function
        }
        if (curState == ENEMY_TURN) {
            // Enemy selects attack based off AI logic
            // Once player selects an attack and it hits the enemy, there should be a short pause so it feels like the enemy is doing an attack
            // How to have a time.wait() type thing, without stalling the game?
        }
    }
};
