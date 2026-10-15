#include "Game.h"
#include <array>
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
void Game::EvaluateRound(Dealer &dealer) {
    std::vector<std::array<NumberCard, 2>> playerHands;

    playerHands.reserve(dealer.players.size());
    for (const auto& player : dealer.players) {
        if ()
        playerHands.push_back({
            player.bloodCard,
            player.sandCard
        });
    }

    playerHands.shrink_to_fit();
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
