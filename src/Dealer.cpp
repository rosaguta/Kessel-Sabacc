#include "Dealer.h"

// #include <iostream>
#include <random>
#include <algorithm>
#include <array>

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
        cards.push_back({
            family,
            IMPOSTOR
        });
    }
    cards.push_back({
        family,
        SYLOP
    });
}

void Dealer::HandleShiftFreeDrawToken(int playerIndex) {
    players[playerIndex].CanDrawForFree = true;
}

void Dealer::HandleShiftRefundToken(int playerIndex) {
    players[playerIndex].chips += 2;
}

void Dealer::HandleShiftExtraRefundToken(int playerIndex) {
    players[playerIndex].chips += 3;
}

void Dealer::HandleShiftGeneralTariffToken(int playerIndex) {
    for (int i = 0; i < static_cast<int>(players.size()); i++)
    {
        if (i != playerIndex && !players[i].immune)
        {
            players[i].chips = std::max(0, players[i].chips - 1);
        }
    }
}

void Dealer::HandleShiftTargetTariffToken(int playerIndex, int targetPlayerIndex) {
    if (targetPlayerIndex != playerIndex && !players[targetPlayerIndex].immune)
    {
        players[targetPlayerIndex].chips = std::max(0, players[targetPlayerIndex].chips - 2);
    }
}

bool Dealer::HandleShiftEmbargoToken(int playerIndex, int targetPlayerIndex) {
    if (targetPlayerIndex != playerIndex && !players[targetPlayerIndex].immune)
    {
        players[targetPlayerIndex].mustStand = true;
    }
    return true;
}

bool Dealer::HandleShiftMarkdownToken(int playerIndex) {
    (void)playerIndex;
    markdownActive = true;
    return true;
}

bool Dealer::HandleShiftImmunityToken(int playerIndex) {
    players[playerIndex].immune = true;
    return true;
}

bool Dealer::HandleShiftGeneralAuditToken(int playerIndex) {
    for (int i = 0; i < static_cast<int>(players.size()); ++i)
    {
        if (i != playerIndex && players[i].stoodThisTurn && !players[i].immune)
        {
            players[i].chips = std::max(0, players[i].chips - 2);
        }
    }
    return true;
}

bool Dealer::HandleShiftTargetAuditToken(int playerIndex, int targetPlayerIndex) {
    Player& target = players[targetPlayerIndex];
    if (targetPlayerIndex == playerIndex || !target.stoodThisTurn)
    {
        return false;
    }
    if (!target.immune)
    {
        target.chips = std::max(0, target.chips - 3);
    }
    return true;
}

bool Dealer::HandleShiftMajorFraudToken() {
    majorFraudActive = true;
    return true;
}

void Dealer::HandleShiftEmbezzlementToken(int playerIndex) {
    for (int i = 0; i < static_cast<int>(players.size()); i++)
    {
        if (i != playerIndex && !players[i].immune && players[i].chips > 0) {
            players[i].chips--;
            players[playerIndex].chips++;
        }
    }
}

bool Dealer::HandleShiftCookTheBooksToken() {
    cookTheBooksActive = true;
    sabacc = 6;
    return true;
}
//TODO: make the player choose which card to discard
bool Dealer::HandleShiftExhaustionToken(int playerIndex, int targetPlayerIndex) {
    (void)playerIndex;
    Player& target = players[targetPlayerIndex];
    if (target.immune)
    {
        return true;
    }

    bloodNumberCards.push_back(target.bloodCard);
    sandNumberCards.push_back(target.sandCard);

    std::random_device rd;
    std::mt19937 generator(rd());
    std::shuffle(bloodNumberCards.begin(), bloodNumberCards.end(), generator);
    std::shuffle(sandNumberCards.begin(), sandNumberCards.end(), generator);

    target.bloodCard = bloodNumberCards.back();
    bloodNumberCards.pop_back();
    target.sandCard = sandNumberCards.back();
    sandNumberCards.pop_back();
    target.EvaluateMatchingPair();
    return true;
}

bool Dealer::HandleShiftDirectTransactionToken(int playerIndex, int targetPlayerIndex) {
    if (targetPlayerIndex == playerIndex || players[targetPlayerIndex].immune)
    {
        return true;
    }
    std::swap(players[playerIndex].bloodCard, players[targetPlayerIndex].bloodCard);
    std::swap(players[playerIndex].sandCard, players[targetPlayerIndex].sandCard);
    players[playerIndex].EvaluateMatchingPair();
    players[targetPlayerIndex].EvaluateMatchingPair();
    return true;
}
// TODO: player must choose the new sabacc number
bool Dealer::HandleShiftPrimeSabaccToken(int playerIndex) {
    (void)playerIndex;
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> dice(1, 6);
    const std::array<int, 2> rolls = {dice(generator), dice(generator)};
    sabacc = rolls[0];
    primeSabaccActive = true;
    return true;
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
    sabacc = 1;
    primeSabaccActive = false;
    markdownActive = false;
    majorFraudActive = false;
    cookTheBooksActive = false;
    for (Player& player : players)
    {
        player.resetState();
    }
    this->DealCards();
    return true;
}

//TODO: add return false when the cards couldnt be shuffled/dealt
bool Dealer::DealCards() {
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
        player.EvaluateMatchingPair();
    }
    this->faceUpBloodCard = bloodNumberCards.back();
    bloodNumberCards.pop_back();
    this->faceUpSandCard = sandNumberCards.back();
    sandNumberCards.pop_back();

    return true;
}
// returns false if the player doesnt have enough chips
//TODO: account for the FREE_DRAW shifttoken during gameplay
bool Dealer::DrawBloodCard(int playerIndex) {
    if (playerIndex < 0 || playerIndex >= static_cast<int>(players.size())
        || bloodNumberCards.empty()
        || (players[playerIndex].chips <= 0 && !players[playerIndex].CanDrawForFree)) {
        return false;
    }
    const bool freeDraw = players[playerIndex].CanDrawForFree;
    players[playerIndex].bloodCard = bloodNumberCards.back();
    bloodNumberCards.pop_back();
    players[playerIndex].CanDrawForFree = false;
    if (!freeDraw)
    {
        players[playerIndex].SpendChips();
    }
    players[playerIndex].EvaluateMatchingPair();
    return true;
}
// returns false if the player doesnt have enough chips
//TODO: account for the FREE_DRAW shifttoken during gameplay
bool Dealer::DrawSandCard(int playerIndex) {
    if (playerIndex < 0 || playerIndex >= static_cast<int>(players.size())
        || sandNumberCards.empty()
        || (players[playerIndex].chips <= 0 && !players[playerIndex].CanDrawForFree)) {
        return false;
    }
    const bool freeDraw = players[playerIndex].CanDrawForFree;
    players[playerIndex].sandCard = sandNumberCards.back();
    sandNumberCards.pop_back();
    players[playerIndex].CanDrawForFree = false;
    if (!freeDraw)
    {
        players[playerIndex].SpendChips();
    }
    players[playerIndex].EvaluateMatchingPair();
    return true;
}
// returns false if the player doesnt have enough chips
//TODO: account for the FREE_DRAW shifttoken during gameplay
bool Dealer::DrawDiscardBloodCard(int playerIndex, const NumberCard cardFromPlayer) {
    if (playerIndex < 0 || playerIndex >= static_cast<int>(players.size())
        || (players[playerIndex].chips <= 0 && !players[playerIndex].CanDrawForFree)) {
        return false;
    }
    if (players[playerIndex].CanDrawForFree){
        players[playerIndex].CanDrawForFree = false;
        players[playerIndex].bloodCard = faceUpBloodCard;
        faceUpBloodCard = cardFromPlayer;
        players[playerIndex].EvaluateMatchingPair();
        return true;
    }
    players[playerIndex].bloodCard = faceUpBloodCard;
    faceUpBloodCard = cardFromPlayer;
    players[playerIndex].SpendChips();
    players[playerIndex].EvaluateMatchingPair();
    return true;
}
// returns false if the player doesnt have enough chips
//TODO: account for the FREE_DRAW shifttoken during gameplay
bool Dealer::DrawDiscardSandCard(int playerIndex, const NumberCard cardFromPlayer) {
    if (playerIndex < 0 || playerIndex >= static_cast<int>(players.size())
        || (players[playerIndex].chips <= 0 && !players[playerIndex].CanDrawForFree)) {
        return false;
    }
    if (players[playerIndex].CanDrawForFree){
        players[playerIndex].CanDrawForFree = false;
        players[playerIndex].sandCard = faceUpSandCard;
        faceUpSandCard = cardFromPlayer;
        players[playerIndex].EvaluateMatchingPair();
        return true;
    }

    players[playerIndex].sandCard = faceUpSandCard;
    faceUpSandCard = cardFromPlayer;
    players[playerIndex].SpendChips();
    players[playerIndex].EvaluateMatchingPair();
    return true;
}

bool Dealer::RemovePlayer(int playerIndex) {
    if (playerIndex < 0 || playerIndex >= static_cast<int>(players.size()))
    {
        return false;
    }
    defeatedPlayers.push_back(players[playerIndex]);
    players.erase(players.begin() + playerIndex);
    return true;
}

bool Dealer::HandleShiftToken(int playerIndex, ShiftToken shift_token, int targetPlayerIndex) {
    if (playerIndex < 0 || playerIndex >= static_cast<int>(players.size()))
    {
        return false;
    }
    const bool needsTarget = shift_token == TARGET_TARIFF
        || shift_token == EMBARGO
        || shift_token == TARGET_AUDIT
        || shift_token == EXHAUSTION
        || shift_token == DIRECT_TRANSACTION;
    if (needsTarget && (targetPlayerIndex < 0
        || targetPlayerIndex >= static_cast<int>(players.size())
        || targetPlayerIndex == playerIndex))
    {
        return false;
    }
    switch (shift_token) {
        case FREE_DRAW:
            HandleShiftFreeDrawToken(playerIndex);
            return true;
            break;
        case REFUND:
            HandleShiftRefundToken(playerIndex);
            return true;
        case EXTRA_REFUND:
            HandleShiftExtraRefundToken(playerIndex);
            return true;
        case GENERAL_TARIFF:
            HandleShiftGeneralTariffToken(playerIndex);
            return true;
        case TARGET_TARIFF:
            HandleShiftTargetTariffToken(playerIndex, targetPlayerIndex);
            return true;
        case EMBARGO:
            return HandleShiftEmbargoToken(playerIndex, targetPlayerIndex);
        case MARKDOWN:
            return HandleShiftMarkdownToken(playerIndex);
        case IMMUNITY:
            return HandleShiftImmunityToken(playerIndex);
        case GENERAL_AUDIT:
            return HandleShiftGeneralAuditToken(playerIndex);
        case TARGET_AUDIT:
            return HandleShiftTargetAuditToken(playerIndex, targetPlayerIndex);
        case MAJOR_FRAUD:
            return HandleShiftMajorFraudToken();
        case EMBEZZLEMENT:
            HandleShiftEmbezzlementToken(playerIndex);
            return true;
        case COOK_THE_BOOKS:
            return HandleShiftCookTheBooksToken();
        case EXHAUSTION:
            return HandleShiftExhaustionToken(playerIndex, targetPlayerIndex);
        case DIRECT_TRANSACTION:
            return HandleShiftDirectTransactionToken(playerIndex, targetPlayerIndex);
        case PRIME_SABACC:
            return HandleShiftPrimeSabaccToken(playerIndex);
        default:
            return false;
    }
}

