#ifndef KESSEL_SABACC_GAME_H
#define KESSEL_SABACC_GAME_H
#include "Dealer.h"


class Game {
    int RoundNumber = 1;
    int TurnNumber = 1;
    bool NextRound();
    bool NextTurn();
public:
    bool Preparation(Dealer* dealer);
    bool Start();
    bool End();
};


#endif //KESSEL_SABACC_GAME_H
