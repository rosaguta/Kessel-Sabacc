#include "Game.h"
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
            break;
        default:
            break;
    }
}

void Game::ResolveEndOfRound(Dealer &dealer) {
    currentRound++;
    currentPhase = GamePhase::AWAITING_PLAYER_ACTION;
    currentTurn = 1;
    activePlayerIndex = 0;
    dealer.Reset();
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
