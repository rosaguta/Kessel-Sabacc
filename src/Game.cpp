#include "Game.h"
#include <random>
#include <algorithm>
#include <cmath>
#include <limits>
#include "Dealer.h"
#include "Player.h"



bool Game::ApplyAction(const PlayerAction &action, Dealer &dealer) {
    if (action.playerId < 0 || action.playerId >= static_cast<int>(dealer.players.size()))
    {
        return false;
    }
    Player& player = dealer.players[action.playerId];
    bool succeeded;
    switch (action.type) {
        case ActionType::DRAW_BLOOD_DECK:
            if (player.stoodThisTurn || player.mustStand) return false;
            succeeded = dealer.DrawBloodCard(action.playerId);
            return succeeded;
        case ActionType::DRAW_BLOOD_DISCARD:
            if (player.stoodThisTurn || player.mustStand) return false;
            return dealer.DrawDiscardBloodCard(action.playerId, action.card);
        case ActionType::DRAW_SAND_DECK:
            if (player.stoodThisTurn || player.mustStand) return false;
            succeeded = dealer.DrawSandCard(action.playerId);
            return succeeded;
        case ActionType::DRAW_SAND_DISCARD:
            if (player.stoodThisTurn || player.mustStand) return false;
            return dealer.DrawDiscardSandCard(action.playerId, action.card);
        case ActionType::USE_TOKEN:
        {
            if (player.stoodThisTurn || player.mustStand || !player.CanUseToken)
            {
                return false;
            }
            const auto token = std::find(player.shiftTokens.begin(), player.shiftTokens.end(), action.token);
            if (token == player.shiftTokens.end()
                || !dealer.HandleShiftToken(action.playerId, action.token, action.targetPlayerIndex))
            {
                return false;
            }
            player.usedShiftTokens.push_back(action.token);
            player.shiftTokens.erase(token);
            player.CanUseToken = false;
            return true;
        }
        case ActionType::STAND:
            player.stoodThisTurn = true;
            player.mustStand = false;
            return true;
        default:
            return false;
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
RoundResult Game::ResolveMatchingPairWinner(Dealer &dealer)
{
    RoundResult result;
    bool primeSabaccExists = false;
    for (const auto& player : dealer.players)
    {
        if (dealer.primeSabaccActive && !dealer.cookTheBooksActive
            && player.matchingPair
            && std::abs(player.bloodCard.number) == dealer.sabacc)
        {
            primeSabaccExists = true;
            break;
        }
    }

    int bestPairValue = dealer.cookTheBooksActive ? 1 : 6;

    for (int i = 0; i < static_cast<int>(dealer.players.size()); ++i)
    {
        const auto& player = dealer.players[i];
        if (!player.matchingPair){
            result.losers.push_back(i);
            continue;
        }

        int bloodValue = player.bloodCard.number;
        int pairValue = std::abs(bloodValue);

        if (primeSabaccExists)
        {
            if (pairValue == dealer.sabacc)
            {
                result.winners.push_back(i);
            }
            else
            {
                result.losers.push_back(i);
            }
        }
        else if ((!dealer.cookTheBooksActive && pairValue < bestPairValue)
            || (dealer.cookTheBooksActive && pairValue > bestPairValue))
        {
            bestPairValue = pairValue;
            result.winners.clear();
            result.winners.push_back(i);
        }
        else if (pairValue == bestPairValue)
        {
            result.winners.push_back(i);
        }
    }

    if (!primeSabaccExists)
    {
        for (int i = 0; i < static_cast<int>(dealer.players.size()); ++i)
        {
            const auto& player = dealer.players[i];
            if (player.matchingPair && std::abs(player.bloodCard.number) != bestPairValue)
            {
                result.losers.push_back(i);
            }
        }
    }

    return result;
}

// If nobody has a matching pair, the lowest difference wins.
// Every player tied for that difference wins.
RoundResult Game::ResolveNonMatchingPairWinner(Dealer &dealer)
{
    RoundResult result;
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
            result.winners.clear();
            result.winners.push_back(i);
        }
        else if (difference == lowestDifference)
        {
            result.winners.push_back(i);
        }else if (difference > lowestDifference){
            result.losers.push_back(i);
        }
    }

    return result;
}
void Game::ResolveImpostorCards(Dealer& dealer)
{
    for (auto& player : dealer.players)
    {
        if (player.bloodCard.number == IMPOSTOR)
        {
            player.bloodCard.number = dealer.majorFraudActive && !player.immune
                ? SIX
                : static_cast<Number>(RollDice());
        }

        if (player.sandCard.number == IMPOSTOR)
        {
            player.sandCard.number = dealer.majorFraudActive && !player.immune
                ? SIX
                : static_cast<Number>(RollDice());
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
            player.bloodCard.number = dealer.markdownActive && !player.immune
                ? SYLOP
                : player.sandCard.number;
        }
        else if (sandSylop)
        {
            player.sandCard.number = dealer.markdownActive && !player.immune
                ? SYLOP
                : player.bloodCard.number;
        }
        player.EvaluateMatchingPair();
    }

    return pureSabaccWinner;
}

RoundResult Game::EvaluateRound(Dealer &dealer) {
    ResolveImpostorCards(dealer);
    int pureSabaccWinner = ResolveSylopCards(dealer);
    RoundResult result;
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
            result = ResolveMatchingPairWinner(dealer);
        }
        else
        {
            result = ResolveNonMatchingPairWinner(dealer);
        }
    }else {
        result.winners.push_back(pureSabaccWinner);
        for (int i = 0; i < static_cast<int>(dealer.players.size()); ++i) {
            if (i != pureSabaccWinner)
            {
                result.losers.push_back(i);
            }
        }
    }
    for (int winnerIndex : result.winners)
    {
        auto& player = dealer.players[winnerIndex];
        player.RefundChips();
    }
    for (int loserIndex : result.losers)
    {
        auto& player = dealer.players[loserIndex];
        player.TaxChips();
    }
    return result;
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
