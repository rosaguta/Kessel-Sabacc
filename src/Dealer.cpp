#include "Dealer.h"

#include <iostream>
#include <random>
#include <algorithm>

void Dealer::GenerateCards(
    std::vector<NumberCard>& cards,
    CardFamily family)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 1; j < 7; j++)
        {
            cards.push_back({
                family,
                static_cast<Number>(j)
            });
        }
    }
}

bool Dealer::HandleShiftFreeDrawToken(int playerIndex) {
}

bool Dealer::HandleShiftRefundToken(int playerIndex) {
}

bool Dealer::HandleShiftExtraRefundToken(int playerIndex) {
}

bool Dealer::HandleShiftGeneralTariffToken(int playerIndex) {
}

bool Dealer::HandleShiftTargetTariffToken(int playerIndex, int targetPlayerIndex) {
}

bool Dealer::HandleShiftEmbargoToken(int playerIndex, int targetPlayerIndex) {
}

bool Dealer::HandleShiftMarkdownToken(int playerIndex) {
}

bool Dealer::HandleShiftImmunityToken(int playerIndex) {
}

bool Dealer::HandleShiftGeneralAuditToken(int playerIndex) {
}

bool Dealer::HandleShiftTargetAuditToken(int playerIndex, int targetPlayerIndex) {
}

bool Dealer::HandleShiftMajorFraudToken() {
}

bool Dealer::HandleShiftEmbezzlementToken(int playerIndex) {
}

bool Dealer::HandleShiftCookTheBooksToken() {
}

bool Dealer::HandleShiftExhaustionToken(int playerIndex) {
}

bool Dealer::HandleShiftDirectTransactionToken(int playerIndex, int targetPlayerIndex) {
}

bool Dealer::HandleShiftPrimeSabaccToken(int playerIndex) {
}

Dealer::Dealer()
{
    GenerateCards(bloodNumberCards, CardFamily::BLOOD);
    GenerateCards(sandNumberCards, CardFamily::SAND);
}

bool Dealer::AddParticipant(const Player& player)
{
    if (players.size() >= MAX_PLAYERCOUNT)
    {
        std::cout
            << "A player could not be added because the table is full\r\n";

        return false;
    }

    players.push_back(player);
    return true;
}

bool Dealer::Reset() {
    bloodNumberCards.clear();
    sandNumberCards.clear();
    GenerateCards(bloodNumberCards, CardFamily::BLOOD);
    GenerateCards(sandNumberCards, CardFamily::SAND);
    this->DealCards();
    return true;
}

//TODO: add return false when the cards couldnt be shuffled/dealt
bool Dealer::DealCards()
{
    std::random_device rd;
    std::mt19937 generator(rd());

    std::shuffle(bloodNumberCards.begin(), bloodNumberCards.end(),generator);
    std::shuffle(sandNumberCards.begin(), sandNumberCards.end(),generator);

    for (Player& player : players)
    {
        player.bloodCard = bloodNumberCards.back();
        bloodNumberCards.pop_back();

        player.sandCard = sandNumberCards.back();
        sandNumberCards.pop_back();
    }
    this->faceUpBloodCard = bloodNumberCards.back();
    bloodNumberCards.pop_back();
    this->faceUpSandCard = sandNumberCards.back();
    sandNumberCards.pop_back();

    return true;
}

bool Dealer::DrawBloodCard(int playerIndex) {
    players[playerIndex].bloodCard = bloodNumberCards.back();
    bloodNumberCards.pop_back();
    players[playerIndex].chips--;
    return true;
}

bool Dealer::DrawSandCard(int playerIndex) {
    players[playerIndex].sandCard = sandNumberCards.back();
    sandNumberCards.pop_back();
    players[playerIndex].chips--;
    return true;
}

bool Dealer::DrawDiscardBloodCard(int playerIndex, const NumberCard cardFromPlayer) {
    players[playerIndex].bloodCard = faceUpBloodCard;
    faceUpBloodCard = cardFromPlayer;
    players[playerIndex].chips--;
    return true;
}

bool Dealer::DrawDiscardSandCard(int playerIndex, const NumberCard cardFromPlayer) {
    players[playerIndex].sandCard = faceUpBloodCard;
    faceUpSandCard = cardFromPlayer;
    players[playerIndex].chips--;
    return true;
}

bool Dealer::RemovePlayer(int playerIndex) {
    defeatedPlayers.push_back(players[playerIndex]);
    players.erase(players.begin() + playerIndex);
    return true;
}

bool Dealer::HandleShiftToken(int playerIndex, ShiftToken shift_token, int targetPlayerIndex) {
    switch (shift_token) {
        case FREE_DRAW:
            return HandleShiftFreeDrawToken(playerIndex);
            break;
        case REFUND:
            return HandleShiftRefundToken(playerIndex);
            break;
        case EXTRA_REFUND:
            return HandleShiftExtraRefundToken(playerIndex);
            break;
        case GENERAL_TARIFF:
            return HandleShiftGeneralTariffToken(playerIndex);
            break;
        case TARGET_TARIFF:
            return HandleShiftTargetTariffToken(playerIndex, targetPlayerIndex);
            break;
        case EMBARGO:
            return HandleShiftEmbargoToken(playerIndex, targetPlayerIndex);
            break;
        case MARKDOWN:
            return HandleShiftMarkdownToken(playerIndex);
            break;
        case IMMUNITY:
            return HandleShiftImmunityToken(playerIndex);
            break;
        case GENERAL_AUDIT:
            return HandleShiftGeneralAuditToken(playerIndex);
            break;
        case TARGET_AUDIT:
            return HandleShiftTargetAuditToken(playerIndex, targetPlayerIndex);
            break;
        case MAJOR_FRAUD:
            return HandleShiftMajorFraudToken();
            break;
        case EMBEZZLEMENT:
            return HandleShiftEmbezzlementToken(playerIndex);
            break;
        case COOK_THE_BOOKS:
            return HandleShiftCookTheBooksToken();
            break;
        case EXHAUSTION:
            return HandleShiftExhaustionToken(playerIndex);
            break;
        case DIRECT_TRANSACTION:
            return HandleShiftDirectTransactionToken(playerIndex, targetPlayerIndex);
            break;
        case PRIME_SABACC:
            return HandleShiftPrimeSabaccToken(playerIndex);
            break;
        default:
            std::cout << "Invalid shift token" << std::endl;
            return false;
    }
}


