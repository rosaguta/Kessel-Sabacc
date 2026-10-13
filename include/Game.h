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
    int RoundNumber = 1;
    int TurnNumber = 1;
    bool NextRound();
    bool NextTurn();
    bool GameFinished = false;
    bool RoundFinished = false;
    void ProcessGame(Dealer* dealer);
    void ProcessRound(Dealer* dealer);
    void ProcessTurn(Dealer* dealer);
    bool End();
public:
    GamePhase currentPhase = GamePhase::AWAITING_PLAYER_ACTION;
    int currentRound = 1;
    int currentTurn = 1;
    int activePlayerIndex = 0;

    bool ApplyAction(const PlayerAction& action, Dealer& dealer);

    void ResolveEndOfRound(Dealer& dealer);

    bool IsGameOver() const;
    bool Prepare(Dealer* dealer);
    bool Start(Dealer* dealer);
};



#endif //KESSEL_SABACC_GAME_H
