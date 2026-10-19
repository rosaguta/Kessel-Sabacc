#ifndef KESSEL_SABACC_DEALER_H
#define KESSEL_SABACC_DEALER_H


#pragma once

#include <vector>
#include "Player.h"

#define MAX_PLAYERCOUNT 4

class Dealer {
private:
    static void GenerateCards(
        std::vector<NumberCard> &cards,
        CardFamily family
    );

    bool HandleShiftFreeDrawToken(int playerIndex); // Avoid the draw fee this turn
    bool HandleShiftRefundToken(int playerIndex); // Retrieve 2 chips in play
    bool HandleShiftExtraRefundToken(int playerIndex); // Retrieve 3 chips in play
    bool HandleShiftGeneralTariffToken(int playerIndex); // Other players are taxed one chip
    bool HandleShiftTargetTariffToken(int playerIndex, int targetPlayerIndex); // Target player is taxed 2 chips
    bool HandleShiftEmbargoToken(int playerIndex, int targetPlayerIndex); // Next player must stand
    bool HandleShiftMarkdownToken(int playerIndex); // Set Sylop value to 0 until next reveal
    bool HandleShiftImmunityToken(int playerIndex); // Prevent Shift Token effects against you until next reveal
    bool HandleShiftGeneralAuditToken(int playerIndex); // Other players in stand are taxed 2 chips
    bool HandleShiftTargetAuditToken(int playerIndex, int targetPlayerIndex); // Choose a player in Stand. They are taxed three chips.
    bool HandleShiftMajorFraudToken(); // Set Impostor value to 6 until next reveal
    bool HandleShiftEmbezzlementToken(int playerIndex); // Take one chip from each player’s pot to your pot
    bool HandleShiftCookTheBooksToken(); // Inverts Sabacc ranks until the next reveal (6/6 Sabacc is the best until next reveal)
    bool HandleShiftExhaustionToken(int playerIndex); // Choose a player. They must discard and draw a new hand
    bool HandleShiftDirectTransactionToken(int playerIndex, int targetPlayerIndex); // Choose a player. Trade hands with them.
    bool HandleShiftPrimeSabaccToken(int playerIndex); // Roll two dice. Pick one value as the new best sabacc

public:
    int currentPlayerTurn{};

    std::vector<NumberCard> bloodNumberCards;
    std::vector<NumberCard> sandNumberCards;
    NumberCard faceUpBloodCard;
    NumberCard faceUpSandCard;

    std::vector<Player> players;
    std::vector<Player> defeatedPlayers;

    Dealer();

    bool AddParticipant(const Player &player);

    bool Reset();

    bool DealCards();

    bool PlayerCanSpendChips(int playerIndex, bool drawFee = true);

    bool DrawBloodCard(int playerIndex, bool drawFee = true);

    bool DrawSandCard(int playerIndex, bool drawFee = true);

    bool DrawDiscardBloodCard(int playerIndex, NumberCard cardFromPlayer, bool drawFee = true);

    bool DrawDiscardSandCard(int playerIndex, NumberCard cardFromPlayer, bool drawFee = true);

    bool RemovePlayer(int playerIndex);

    bool HandleShiftToken(int playerIndex, ShiftToken shift_token, int targetPlayerIndex);
};

#endif //KESSEL_SABACC_DEALER_H
