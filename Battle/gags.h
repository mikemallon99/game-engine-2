#pragma once

#include <iostream>
#include <map>
#include <array>

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

class PlayerBattleData {
public:
    std::map<GagType, GagTrack> gagTracks;
    std::map<GagType, std::array<int, 6>> gagInventory;
    int maxHealth;
    int curHealth;

    PlayerBattleData() {
        gagTracks = initializeGagTracks();
        gagInventory = initializeGagInventory();

        maxHealth = 15;
        curHealth = 11;
    }
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