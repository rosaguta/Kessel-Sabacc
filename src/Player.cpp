#include "Player.h"
#include <iostream>

std::string Player::getCardFamilyName(CardFamily family)
{
    switch (family)
    {
        case SAND:
            return "Sand";

        case BLOOD:
            return "Blood";

        default:
            return "Unknown";
    }
}

std::string Player::getNumberName(Number number)
{
    switch (number)
    {
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

        default:
            return "Unknown";
    }
}

Player::Player()
{
    specialCards = 0;
    shiftTokens = {
        FREE_DRAW,
        GENERAL_AUDIT,
        MARKDOWN
    };
    chips = 8;
}

void Player::printCurrentHand()
{
    std::cout << "==The current hand is==\r\n";
    std::cout << "Cards:\r\n";

    std::cout << "  "
              << getCardFamilyName(bloodCard.cardFamily)
              << "\t"
              << getNumberName(bloodCard.number)
              << "\n";

    std::cout << "  "
              << getCardFamilyName(sandCard.cardFamily)
              << "\t"
              << getNumberName(sandCard.number)
              << "\n";

    std::cout << "  Shift tokens: ";

    for (const auto& token : shiftTokens)
    {
        std::cout << token << " ";
    }

    std::cout << "\n";
}