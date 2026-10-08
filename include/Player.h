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
    NumberCard bloodCard;
    NumberCard sandCard;

    int specialCards;
    std::vector<ShiftToken> shiftTokens;
    int chips;

    Player();

    void printCurrentHand();
};


#endif //KESSEL_SABACC_PLAYER_H
