#ifndef KESSEL_SABACC_DEALER_H
#define KESSEL_SABACC_DEALER_H


#pragma once

#include <vector>
#include "Player.h"

#define MAX_PLAYERCOUNT 4

class Dealer
{
private:
    static void GenerateCards(
        std::vector<NumberCard>& cards,
        CardFamily family
    );
public:
    int currentPlayerTurn{};

    std::vector<NumberCard> bloodNumberCards;
    std::vector<NumberCard> sandNumberCards;
    NumberCard faceUpBloodCard;
    NumberCard faceUpSandCard;

    std::vector<Player> players;

    int imposterCards = 6;
    int sylopCards = 2;

    Dealer();

    bool AddParticipant(const Player& player);
    bool Reset();
    bool DealCards();
    bool DrawBloodCard(int playerIndex);
    bool DrawSandCard(int playerIndex);
    bool DrawDiscardBloodCard(int playerIndex, NumberCard cardFromPlayer);
    bool DrawDiscardSandCard(int playerIndex, NumberCard cardFromPlayer);
};

#endif //KESSEL_SABACC_DEALER_H
