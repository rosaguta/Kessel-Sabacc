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
    //TODO: reset game state and advance to next round
    RoundNumber++;
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
bool Game::Start(Dealer* dealer) {
    while (!GameFinished) {
        ProcessGame(dealer);
    }

    return false;
}
void Game::ProcessGame(Dealer* dealer) {
    while (!GameFinished) {
        ProcessRound(dealer);
        RoundNumber++;
    }
}

void Game::ProcessRound(Dealer *dealer) {
    while (!RoundFinished) {
        ProcessTurn(dealer);
    }
}

void Game::ProcessTurn(Dealer *dealer) {
    while (TurnNumber < 4) {

    }
}

bool Game::NextRound() {
    return false;

}
bool Game::NextTurn() {
    return false;

}

bool Game::End() {
    return false;

}

