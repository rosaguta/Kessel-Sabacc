#include <iostream>
#include "Dealer.h"
#include "Player.h"
#include "Game.h"
void RenderTerminal(const Game& game, const Dealer& dealer) {
    std::cout << "\n========================================\n";
    std::cout << "ROUND " << game.currentRound << " | TURN " << game.currentTurn << "\n";
    std::cout << "\n================Dealer===================\n";

    std::cout << "Face-up Blood: " << dealer.faceUpBloodCard.ToString()
              << " | Face-up Sand: " << dealer.faceUpSandCard.ToString() << "\n";
    std::cout << "\n================Player===================\n";
    const Player& current = dealer.players[game.activePlayerIndex];
    std::cout << "Active Player: " << game.activePlayerIndex << "\n";
    std::cout << "Hand: Blood[" << current.bloodCard.ToString() << "] Sand["
              << current.sandCard.ToString() << "]\n";
    std::cout << "Chips: " << current.chips << "\n";
    std::cout << "========================================\n";
}

PlayerAction GetTerminalInput(int activePlayerId) {
    std::cout << "Choose Action:\n";
    std::cout << "1. Stand\n2. Draw Blood Deck\n3. Draw Blood Discard\n"
              << "4. Draw Sand Deck\n5. Draw Sand Discard\n> ";

    int choice;
    std::cin >> choice;

    PlayerAction action;
    action.playerId = activePlayerId;
    switch (choice) {
        case 1: action.type = ActionType::STAND; break;
        case 2: action.type = ActionType::DRAW_BLOOD_DECK; break;
        case 3: action.type = ActionType::DRAW_BLOOD_DISCARD; break;
        case 4: action.type = ActionType::DRAW_SAND_DECK; break;
        case 5: action.type = ActionType::DRAW_SAND_DISCARD; break;
        default: action.type = ActionType::STAND; break;
    }
    return action;
}


int main()
{
    Player player1;
    Player player2;
    Player player3;
    Player player4;

    Dealer dealer;

    dealer.AddParticipant(player1);
    dealer.AddParticipant(player2);
    dealer.AddParticipant(player3);
    dealer.AddParticipant(player4);

    Game game;
    bool prepped = game.Prepare(&dealer);
    // for (Player& player : dealer.players)
    // {
    //     player.printCurrentHand();
    // }
    if(prepped == false) {
        return 1;
    }
    while (!game.IsGameOver()) {
        RenderTerminal(game, dealer);

        if (game.currentPhase == GamePhase::AWAITING_PLAYER_ACTION) {
            PlayerAction action = GetTerminalInput(game.activePlayerIndex);
            game.ApplyAction(action, dealer);
        } else if (game.currentPhase == GamePhase::ROUND_END) {
            game.ResolveEndOfRound(dealer);
        }
    }
    return 0;
}