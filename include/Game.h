#ifndef KESSEL_SABACC_GAME_H
#define KESSEL_SABACC_GAME_H
#include "Dealer.h"

enum class GamePhase {
    AWAITING_PLAYER_ACTION,
    PROCESSING_DICE_ROLL,
    ROUND_END,
    GAME_OVER
};
class Game {
    bool GameFinished = false;
    bool RoundFinished = false;
    void EvaluateRound(Dealer &dealer);

    int RollDice();

public:
    GamePhase currentPhase = GamePhase::AWAITING_PLAYER_ACTION;
    int currentRound = 1;
    int currentTurn = 1;
    int activePlayerIndex = 0;
    bool ApplyAction(const PlayerAction& action, Dealer& dealer);
    void ResolveEndOfRound(Dealer& dealer);
    bool IsGameOver() const;
    bool Prepare(Dealer* dealer);
};



#endif //KESSEL_SABACC_GAME_H
