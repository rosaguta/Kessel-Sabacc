#include "Game.h"
#include <iostream>
#include <ostream>
#include <random>
#include "Dealer.h"
#include "Player.h"



bool Game::ApplyAction(const PlayerAction &action, Dealer &dealer) {
    switch (action.type) {
        case ActionType::DRAW_BLOOD_DECK:
            return dealer.DrawBloodCard(action.playerId);
            break;
        case ActionType::DRAW_BLOOD_DISCARD:
            return dealer.DrawDiscardBloodCard(action.playerId, action.card);
            break;
        case ActionType::DRAW_SAND_DECK:
            return dealer.DrawSandCard(action.playerId);
            break;
        case ActionType::DRAW_SAND_DISCARD:
            return dealer.DrawDiscardSandCard(action.playerId, action.card);
            break;
        case ActionType::USE_TOKEN:
            return true;
            break;
        case ActionType::STAND:
            // no action is being performed when a user decides to stand
            return true;
            break;
        default:
            return true;
            break;
    }
}

void Game::ResolveEndOfRound(Dealer &dealer) {
    EvaluateRound(dealer);
    currentRound++;
    currentTurn = 1;
    activePlayerIndex = 0;
    dealer.Reset();
}
bool Game::MatchingPairExist(Dealer &dealer) {
    bool pairFound = false;
    for (const auto& player : dealer.players) {
        if (player.matchingPair) {
            pairFound = true;
            break;
        }
    }
    return pairFound;
}
// Returns every player with the lowest matching pair.
// A pair of Sylops is handled separately as Pure Sabacc.
std::vector<int> Game::ResolveMatchingPairWinner(Dealer &dealer)
{
    std::vector<int> winners;
    int lowestPairValue = std::numeric_limits<int>::max();

    for (int i = 0; i < static_cast<int>(dealer.players.size()); ++i)
    {
        const auto& player = dealer.players[i];
        if (!player.matchingPair)
            continue;

        int bloodValue = player.bloodCard.number;
        int pairValue = std::abs(bloodValue);

        if (pairValue < lowestPairValue)
        {
            lowestPairValue = pairValue;
            winners.clear();
            winners.push_back(i);
        }
        else if (pairValue == lowestPairValue)
        {
            winners.push_back(i);
        }
    }

    return winners;
}

// If nobody has a matching pair, the lowest difference wins.
// Every player tied for that difference wins.
std::vector<int> Game::ResolveNonMatchingPairWinner(Dealer &dealer)
{
    std::vector<int> winners;
    int lowestDifference = std::numeric_limits<int>::max();

    for (int i = 0; i < static_cast<int>(dealer.players.size()); ++i)
    {
        const auto& player = dealer.players[i];

        int bloodValue = player.bloodCard.number;
        int sandValue = player.sandCard.number;

        int difference = std::abs(bloodValue - sandValue);

        if (difference < lowestDifference)
        {
            lowestDifference = difference;
            winners.clear();
            winners.push_back(i);
        }
        else if (difference == lowestDifference)
        {
            winners.push_back(i);
        }
    }

    return winners;
}
void Game::ResolveImpostorCards(Dealer& dealer)
{
    for (auto& player : dealer.players)
    {
        if (player.bloodCard.number == IMPOSTOR)
        {
            player.bloodCard.number =
                static_cast<Number>(RollDice());
        }

        if (player.sandCard.number == IMPOSTOR)
        {
            player.sandCard.number =
                static_cast<Number>(RollDice());
        }
    }
}

int Game::ResolveSylopCards(Dealer &dealer)
{
    int pureSabaccWinner = -1;

    for (int i = 0; i < static_cast<int>(dealer.players.size()); ++i)
    {
        auto& player = dealer.players[i];
        bool bloodSylop = player.bloodCard.number == SYLOP;
        bool sandSylop = player.sandCard.number == SYLOP;

        if (bloodSylop && sandSylop)
        {
            pureSabaccWinner = i;
            continue;
        }

        if (bloodSylop)
        {
            player.bloodCard.number = player.sandCard.number;
        }
        else if (sandSylop)
        {
            player.sandCard.number = player.bloodCard.number;
        }
    }

    return pureSabaccWinner;
}
//TODO: tax the losing players
void Game::EvaluateRound(Dealer& dealer)
{
    ResolveImpostorCards(dealer);
    int pureSabaccWinner = ResolveSylopCards(dealer);
    std::vector<int> winners;
    // If there is no Pure Sabacc, resolve normal hands.
    if (pureSabaccWinner == -1)
    {
        bool matchingPairExists = false;

        for (const auto& player : dealer.players)
        {
            if (player.matchingPair)
            {
                matchingPairExists = true;
                break;
            }
        }

        // Matching pairs always beat non-matching hands.
        if (matchingPairExists)
        {
            winners = ResolveMatchingPairWinner(dealer);
        }
        else
        {
            winners = ResolveNonMatchingPairWinner(dealer);
        }
    }else {
        winners.push_back(pureSabaccWinner);
    }

    //Display all winners.
    if (winners.empty())
    {
        std::cout << "No winner could be determined.\n";
        return;
    }

    if (winners.size() == 1)
    {
        std::cout << "\nThe winner of this round is Player "
                  << winners[0] << "!\n";
    }
    else
    {
        std::cout << "\nThe round is tied between these players:\n";

        for (int winnerIndex : winners)
        {
            std::cout << "Player " << winnerIndex << '\n';
        }
    }

    for (int winnerIndex : winners)
    {
        const auto& player = dealer.players[winnerIndex];

        std::cout << "Player " << winnerIndex << " hand: "
                  << player.bloodCard.ToString() << " | "
                  << player.sandCard.ToString() << '\n';
    }

    // TODO: Refund the winning players' invested chips
    // and tax the losing players according to their hand values.
}
int Game::RollDice() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(-6 , -1); // can change to min 1 and max 6, minuses are used for debugging
    return distr(gen);
}

bool Game::IsGameOver() const {
    return false;
}

bool Game::Prepare(Dealer* dealer) {
    if (dealer->players.empty() || dealer->players.size() == 1) {
        return false;
    }
    if (dealer->DealCards()) {
        return true;
    }
    return false;
}
