#include "Game.h"
#include "Dealer.h"

bool Game::Preparation(Dealer* dealer) {
    if (dealer->players.empty()) {
        return false;
    }
    if (dealer->DealCards()) {
        return true;
    }
    return false;
}
bool Game::Start() {
    return false;
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

