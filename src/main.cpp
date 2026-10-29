#include <iostream>
#include <map>

#include "Dealer.h"
#include "Player.h"
#include "Game.h"
std::map<ShiftToken, std::string> ShiftTokenToString = {
    {FREE_DRAW, "Free Draw"},
    {REFUND, "Refund"},
    {EXTRA_REFUND, "Extra Refund"},
    {GENERAL_TARIFF, "General Tariff"},
    {TARGET_TARIFF, "Target Tariff"},
    {EMBARGO, "Embargo"},
    {MARKDOWN, "Markdown"},
    {IMMUNITY, "Immunity"},
    {GENERAL_AUDIT, "General Audit"},
    {TARGET_AUDIT, "Target Audit"},
    {MAJOR_FRAUD, "Major Fraud"},
    {EMBEZZLEMENT, "Embezzlement"},
    {COOK_THE_BOOKS, "Cook The Books"},
    {EXHAUSTION, "Exhaustion"},
    {DIRECT_TRANSACTION, "Direct Transaction"},
    {PRIME_SABACC, "Prime Sabacc"}
};
static std::string enumToString(ShiftToken token) {
    return ShiftTokenToString[token];
}

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

PlayerAction GetTerminalInput(Game game, Dealer &dealer) {
    Player &player = dealer.players[game.activePlayerIndex];
    const int tokenMenuStart = 6;

    if (player.mustStand)
    {
        PlayerAction action{};
        action.playerId = game.activePlayerIndex;
        action.type = ActionType::STAND;
        action.roundNumber = game.currentRound;
        action.turnNumber = game.currentTurn;
        return action;
    }

    std::cout << "Choose Action:\n";
    std::cout << "1. Stand\n";
    if (player.chips > 0 || player.CanDrawForFree) {
        std::cout << "2. Draw Blood Deck\n3. Draw Blood Discard\n"
                  << "4. Draw Sand Deck\n5. Draw Sand Discard\n";
    }

    if (player.CanUseToken && !player.shiftTokens.empty()) {
        for (size_t i = 0; i < player.shiftTokens.size(); ++i) {
            std::cout << (tokenMenuStart + static_cast<int>(i))
                      << ". Use token " << enumToString(player.shiftTokens[i]) << "\n";
        }
    }
    std::cout << "> ";

    int choice;
    std::cin >> choice;

    PlayerAction action{};
    action.playerId = game.activePlayerIndex;
    action.roundNumber = game.currentRound;
    action.turnNumber = game.currentTurn;
    if (choice >= tokenMenuStart
        && choice < tokenMenuStart + static_cast<int>(player.shiftTokens.size())
        && player.CanUseToken) {
        action.type = ActionType::USE_TOKEN;
        action.token = player.shiftTokens[choice - tokenMenuStart];

        switch (action.token) {
            case TARGET_TARIFF:
            case EMBARGO:
            case TARGET_AUDIT:
            case DIRECT_TRANSACTION:
            case EXHAUSTION:
                std::cout << "Target player index: ";
                std::cin >> action.targetPlayerIndex;
                break;
            default:
                break;
        }
        return action;
    }

    if (player.chips <= 0) {
        action.type = ActionType::STAND;
        return action;
    }

    switch (choice) {
        case 1: action.type = ActionType::STAND;
            break;
        case 2: action.type = ActionType::DRAW_BLOOD_DECK;
            break;
        case 3: action.type = ActionType::DRAW_BLOOD_DISCARD;
            action.card = player.bloodCard;
            break;
        case 4: action.type = ActionType::DRAW_SAND_DECK;
            break;
        case 5: action.type = ActionType::DRAW_SAND_DISCARD;
            action.card = player.sandCard;
            break;
        default: action.type = ActionType::STAND;
            break;
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

    while (!game.IsGameOver())
    {
        RenderTerminal(game, dealer);
        switch (game.currentPhase)
        {
            case GamePhase::AWAITING_PLAYER_ACTION:
            {
                if (dealer.players[game.activePlayerIndex].stoodThisTurn)
                {
                    game.activePlayerIndex++;
                    if (game.activePlayerIndex > static_cast<int>(dealer.players.size()) - 1)
                    {
                        game.activePlayerIndex = 0;
                        game.currentTurn++;
                        for (auto& player : dealer.players)
                        {
                            player.stoodThisTurn = false;
                        }
                    }
                    break;
                }

                PlayerAction action = GetTerminalInput(game, dealer);
                bool applied = game.ApplyAction(action, dealer);
                if (!applied) {
                    std::cout << "Action failed.\n";
                    break; // same player tries again
                }
                Player &active = dealer.players[game.activePlayerIndex];

                // Token does not end the turn, player still stands or draws. as seen in StarWars Outlaws
                if (action.type == ActionType::USE_TOKEN) {
                    active.CanUseToken = false;
                    break;
                }

                active.CanUseToken = true;
                game.activePlayerIndex++;
                if (game.activePlayerIndex > static_cast<int>(dealer.players.size()) - 1)
                {
                    if (game.currentTurn == 3){
                        game.currentPhase = GamePhase::ROUND_END;
                    }
                    game.activePlayerIndex = 0;
                    game.currentTurn++;
                    for (auto& player : dealer.players)
                    {
                        player.stoodThisTurn = false;
                        player.mustStand = false;
                    }
                }
                break;
            }
            case GamePhase::ROUND_END:
            {
                game.ResolveEndOfRound(dealer);
                game.currentPhase = GamePhase::AWAITING_PLAYER_ACTION;
                break;
            }
            default:
            {
                std::cerr << "Unhandled game phase.\n";
                return 1;
            }
        }
    }
    return 0;
}
