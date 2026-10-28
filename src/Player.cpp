#include "Player.h"
#include <iostream>

std::string Player::getCardFamilyName(CardFamily family) {
    switch (family) {
        case SAND:
            return "Sand";

        case BLOOD:
            return "Blood";

        default:
            return "Unknown";
    }
}

std::string Player::getNumberName(Number number) {
    switch (number) {
        case ONE:
            return "One";

        case TWO:
            return "Two";

        case THREE:
            return "Three";

        case FOUR:
            return "Four";

        case FIVE:
            return "Five";

        case SIX:
            return "Six";

        case IMPOSTOR:
            return "Impostor";

        case SYLOP:
            return "Sylop";

        default:
            return "Unknown";
    }
}

Player::Player() {
    specialCards = 0;
    shiftTokens = {
        FREE_DRAW,
        GENERAL_AUDIT,
        MARKDOWN
    };
    chips = 8;
    usedChips = 0;
    matchingPair = false;
    CanDrawForFree = false;
    CanUseToken = true;
    mustStand = false;
    immune = false;
    stoodThisRound = false;
}

void Player::SpendChips() {
    chips--;
    usedChips++;
}
void Player::RefundChips() {
    int refundAmount = usedChips;
    chips += refundAmount;
    usedChips = 0;
}
void Player::TaxChips() {
    usedChips = 0;
}

void Player::EvaluateMatchingPair() {
    if (bloodCard.number == sandCard.number) {
        matchingPair = true;
    }else {
        matchingPair = false;
    }
}

void Player::resetState() {
    immune = false;
    CanDrawForFree = false;
    CanUseToken = true;
    mustStand = false;
    stoodThisRound = false;
}
