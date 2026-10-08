#include "Dealer.h"

#include <iostream>
#include <random>
#include <algorithm>

void Dealer::GenerateCards(
    std::vector<NumberCard>& cards,
    CardFamily family)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 1; j < 7; j++)
        {
            cards.push_back({
                family,
                static_cast<Number>(j)
            });
        }
    }
}

Dealer::Dealer()
{
    GenerateCards(bloodNumberCards, CardFamily::BLOOD);
    GenerateCards(sandNumberCards, CardFamily::SAND);
}

bool Dealer::AddParticipant(const Player& player)
{
    if (players.size() >= MAX_PLAYERCOUNT)
    {
        std::cout
            << "A player could not be added because the table is full\r\n";

        return false;
    }

    players.push_back(player);
    return true;
}

void Dealer::DealCards()
{
    std::random_device rd;
    std::mt19937 generator(rd());

    std::shuffle(
        bloodNumberCards.begin(),
        bloodNumberCards.end(),
        generator
    );

    std::shuffle(
        sandNumberCards.begin(),
        sandNumberCards.end(),
        generator
    );

    for (Player& player : players)
    {
        player.bloodCard = bloodNumberCards.back();
        bloodNumberCards.pop_back();

        player.sandCard = sandNumberCards.back();
        sandNumberCards.pop_back();
    }
}
