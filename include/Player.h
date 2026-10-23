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
    bool matchingPair;
    int specialCards;
    std::vector<ShiftToken> shiftTokens;
    int chips;
    int usedChips;

    Player();

    void printCurrentHand();

    void SpendChips();

    void RefundChips();

    void TaxChips();

    void EvaluateMatchingPair();
};

enum class ActionType {
    STAND,
    DRAW_BLOOD_DECK,
    DRAW_BLOOD_DISCARD,
    DRAW_SAND_DECK,
    DRAW_SAND_DISCARD,
    USE_TOKEN,
};

struct PlayerAction {
    int playerId;
    ActionType type;
    NumberCard card;
};

#endif //KESSEL_SABACC_PLAYER_H
