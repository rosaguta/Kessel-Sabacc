#ifndef KESSEL_SABACC_PLAYER_H
#define KESSEL_SABACC_PLAYER_H


#pragma once

#include <string>
#include <vector>

#include "NumberCard.h"
#include "ShiftToken.h"

class Player
{
private:
    std::string getCardFamilyName(CardFamily family);
    std::string getNumberName(Number number);

public:
    NumberCard bloodCard{};
    NumberCard sandCard{};
    NumberCard CardInHand{};
    bool matchingPair;
    int specialCards;
    std::vector<ShiftToken> shiftTokens;
    int chips;
    int usedChips;
    bool CanDrawForFree;
    bool CanUseToken;
    bool mustStand; // is set when a shifttoken is used to force a stand on a player
    bool immune; // immune to shifttokens against the player
    bool stoodThisTurn;
    std::vector<ShiftToken> usedShiftTokens;

    Player();

    void printCurrentHand();

    void SpendChips();

    void RefundChips();

    void TaxChips();

    void EvaluateMatchingPair();

    void resetState();
};

enum class ActionType {
    STAND,
    DRAW_BLOOD_DECK,
    DRAW_BLOOD_DISCARD,
    DRAW_SAND_DECK,
    DRAW_SAND_DISCARD,
    USE_TOKEN,
};



#endif //KESSEL_SABACC_PLAYER_H
