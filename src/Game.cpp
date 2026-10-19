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
    currentPhase = GamePhase::AWAITING_PLAYER_ACTION;
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
// returns playerIndex of the winner
int Game::ResolveMatchingPairWinner(Dealer &dealer) {
    int winningPlayerIndex = -1;

    for (int i = 0; i < dealer.players.size(); i++) {
        const auto& currentPlayer = dealer.players[i];
        if (!currentPlayer.matchingPair) {
            continue;
        }
        if (winningPlayerIndex == -1 || currentPlayer.bloodCard.number < dealer.players[winningPlayerIndex].bloodCard.number)
        {
            winningPlayerIndex = i;
        }
    }
    return winningPlayerIndex;
}
// returns playerIndex of the winner
int Game::ResolveNonMatchingPairWinner(Dealer &dealer) {
    int winningPlayerIndex = -1;
    int lowestDifference = INT_MAX;
    for (int i = 0; i < dealer.players.size(); i++) {
        const auto& currentPlayer = dealer.players[i];
        int difference = std::abs(static_cast<int>(currentPlayer.bloodCard.number) - static_cast<int>(currentPlayer.sandCard.number));

        if (difference < lowestDifference) {
            lowestDifference = difference;
            winningPlayerIndex = i;
        }
    }
    return winningPlayerIndex;
}
void Game::EvaluateRound(Dealer &dealer) {
    int winner;
    bool matchingPair = MatchingPairExist(dealer);
    if (matchingPair) {
        winner = ResolveMatchingPairWinner(dealer);
    }
    else {
        winner = ResolveNonMatchingPairWinner(dealer);
    }
    std::cout << "\nThe winner of this round is: " << winner << "\n";
    std::cout << "hand: " << dealer.players[winner].bloodCard.ToString() << " | " << dealer.players[winner].sandCard.ToString() << "\n";
}
int Game::RollDice() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(1 , 6);
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
