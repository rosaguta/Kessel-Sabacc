#include <iostream>
#include "Dealer.h"
#include "Player.h"
#include "Game.h"

void RenderTerminal(const Game &game, const Dealer &dealer) {
    std::cout << "\n========================================\n";
    std::cout << "ROUND " << game.currentRound << " | TURN " << game.currentTurn << "\n";

    std::cout << "\n================Dealer===================\n";
    std::cout << "Remaining blood cards: " << dealer.bloodNumberCards.size() << " | Remaining sand cards: " << dealer.
            sandNumberCards.size() << "\n";

    std::cout << "Face-up: " << dealer.faceUpBloodCard.ToString()
            << " | Face-up: " << dealer.faceUpSandCard.ToString();
    std::cout << "\n================Player===================\n";
    const Player &current = dealer.players[game.activePlayerIndex];
    std::cout << "Active Player: " << game.activePlayerIndex << "\n";
    std::cout << "Hand: " << current.bloodCard.ToString() << " | "
            << current.sandCard.ToString() << "\n";
    std::cout << "Chips: " << current.chips << "\n";
    std::cout << "========================================\n";
}

PlayerAction GetTerminalInput(int activePlayerId, Dealer &dealer) {
    if (dealer.players[activePlayerId].chips <= 0) {
        std::cout << "Choose Action:\n";
        std::cout << "1. Stand\n";
    } else {
        std::cout << "Choose Action:\n";
        std::cout << "1. Stand\n2. Draw Blood Deck\n3. Draw Blood Discard\n"
                << "4. Draw Sand Deck\n5. Draw Sand Discard\n> ";
    }
    int choice;
    std::cin >> choice;

    PlayerAction action;
    action.playerId = activePlayerId;
    if (dealer.players[activePlayerId].chips <= 0) {
        action.type = ActionType::STAND;
    } else {
        switch (choice) {
            case 1: action.type = ActionType::STAND;
                break;
            case 2: action.type = ActionType::DRAW_BLOOD_DECK;
                break;
            case 3: action.type = ActionType::DRAW_BLOOD_DISCARD;
                action.card = dealer.players[activePlayerId].bloodCard;
                break;
            case 4: action.type = ActionType::DRAW_SAND_DECK;
                break;
            case 5: action.type = ActionType::DRAW_SAND_DISCARD;
                action.card = dealer.players[activePlayerId].sandCard;
                break;
            default: action.type = ActionType::STAND;
                break;
        }
    }
    return action;
}


int main() {
    Player player1;
    Player player2;
    Player player3;
    Player player4;

    Dealer dealer;

    dealer.AddParticipant(player1);
    dealer.AddParticipant(player2);
    // dealer.AddParticipant(player3);
    // dealer.AddParticipant(player4);

    Game game;
    bool prepped = game.Prepare(&dealer);
    if (prepped == false) {
        return 1;
    }
    //TODO: update the game loop
    while (!game.IsGameOver()) {
        if (game.currentTurn > 3) {
            game.currentPhase = GamePhase::ROUND_END;
        }
        if (game.currentPhase == GamePhase::ROUND_END) {
            game.ResolveEndOfRound(dealer);
        }
        if (game.activePlayerIndex > dealer.players.size() - 1) {
            game.currentTurn++;
            game.activePlayerIndex = 0;
        }
        RenderTerminal(game, dealer);
        if (game.currentPhase == GamePhase::AWAITING_PLAYER_ACTION) {
            PlayerAction action = GetTerminalInput(game.activePlayerIndex, dealer);
            game.ApplyAction(action, dealer);
            game.activePlayerIndex++;
        }

    }
    return 0;
}
