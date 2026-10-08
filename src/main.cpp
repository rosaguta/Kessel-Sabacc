#include "Dealer.h"
#include "Player.h"
#include "Game.h"
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
    game.Preparation(dealer&);

    for (Player& player : dealer.players)
    {
        player.printCurrentHand();
    }

    return 0;
}