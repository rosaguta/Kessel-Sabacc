# KESSEL SABACC

This repository around the game of **Kessel Sabacc** from the video game **Star Wars Outlaws**. During my playthrough of this game I got introduced to the game of Kessel Sabacc. 
I felt much enjoyment playing this game and I felt inclined on challenging myself on building this game in a language I have not yet touched. The language in this case is c++.
I do have some experience with C but it is still rather new to me and I want to see what challenges I come across when I get off my main programming language (C#)

## RULES
The game will be defined by [this pdf](https://hyperspaceprops.com/wp-content/uploads/2024/09/Kessel-Sabacc-Rules-v4.pdf) of HyperSpaceProps

## FEATURES

| Category                | Feature / Rule                                    | Status | Implementation Details & Code Reference                                                                               |
|:------------------------|:--------------------------------------------------|:------:|:----------------------------------------------------------------------------------------------------------------------|
| **Setup & Components**  | **Player Capacity (2–4 Players)**                 |  [x]   | Defined via `MAX_PLAYERCOUNT 4` in `Dealer.h`; enforced in `Dealer::AddParticipant` and validated in `Game::Prepare`. |
|                         | **Credit Pot & Buy-in System**                    |  [ ]   | Rules require buy-in credits (50–200 credits) placed into a central pot.                                              |
|                         | **Chip Tracking per Player**                      |  [x]   | `Player::chips` tracks chip counts in `Player.h`                                                                      |
|                         | **Standard Number Cards (1–6)**                   |  [x]   | Generated with 3 sets per family (18 Blood, 18 Sand) in `Dealer::GenerateCards`.                                      |
|                         | **Specialty Cards: Impostors (6)**                |  [ ]   | Counter exists (`Dealer::imposterCards`), but cards are not defined in `NumberCard.h` or added to decks.              |
|                         | **Specialty Cards: Sylops (2)**                   |  [ ]   | Counter exists (`Dealer::sylopCards`), but card types and deck integration are absent.                                |
|                         | **Face-up Discard Piles**                         |  [x]   | Initialized during deal with top cards (`faceUpBloodCard`, `faceUpSandCard`) in `Dealer::DealCards`.                  |
|                         | **Initial Deal (1 Blood, 1 Sand)**                |  [x]   | Shuffled and dealt to each player at round start in `Dealer::DealCards`.                                              |
| **Turn Actions**        | **3-Turn Round Limit**                            |  [x]   | Round progression and turn limit of 3 turns tracked and transitioned in `main.cpp` / `Game.h`.                        |
|                         | **Stand Action**                                  |  [x]   | Implemented in `Game::ApplyAction` (`ActionType::STAND`) requiring no chip fee.                                       |
|                         | **Draw from Facedown Deck (Blood / Sand)**        |  [x]   | Handled by `Dealer::DrawBloodCard` and `Dealer::DrawSandCard`.                                                        |
|                         | **Draw from Face-up Discard Pile (Blood / Sand)** |  [x]   | Handled by `Dealer::DrawDiscardBloodCard` and `Dealer::DrawDiscardSandCard`.                                          |
|                         | **Chip Fee on Draw (1 Chip)**                     |  [x]   | Deducted when drawing cards in `Dealer::Draw*` when `drawFee` is enabled.                                             |
|                         | **Choice to Discard Drawn Card vs. Hand Card**    |  [ ]   | Drawing currently replaces hand card directly without offering choice to discard the drawn card instead.              |
| **Shift Tokens**        | **Shift Token Enum & Registry (16 Tokens)**       |  [x]   | All 16 tokens declared in `ShiftToken.h` and dispatched in `Dealer::HandleShiftToken`.                                |
|                         | **Shift Token Effect Execution**                  |  [ ]   | Handler methods (`HandleShiftFreeDrawToken`, `HandleShiftRefundToken`, etc.) are empty stubs in `Dealer.cpp`.         |
|                         | **Shift Token Pre-Action Play Window**            |  [ ]   | Game loop does not provide an integrated prompt to play a token prior to Stand/Draw.                                  |
| **Reveal & Evaluation** | **Clockwise Reveal Phase**                        |  [ ]   | Round transitions directly to `ResolveEndOfRound` without a reveal sequence.                                          |
|                         | **Sabacc Dice Rolling for Impostors**             |  [ ]   | No dice rolling mechanism or Impostor value assignment implemented.                                                   |
|                         | **Sylop Value Matching & Pure Sabacc**            |  [ ]   | No evaluation logic to match Sylop to adjacent cards or resolve Pure Sabacc (Sylop pair).                             |
|                         | **Hand Comparison & Ranking Hierarchy**           |  [ ]   | Missing Sabacc rank hierarchy (Pure Sabacc > Sylop Sabacc > 1/1 ... 6/6 > Non-Sabacc differences).                    |
|                         | **Round Winner Determination**                    |  [ ]   | No winner evaluation logic in `Game::ResolveEndOfRound`.                                                              |
|                         | **Tie Resolution**                                |  [ ]   | No handling for matching winning hands.                                                                               |
| **Scoring & End Game**  | **Chip Recovery for Round Winner**                |  [ ]   | Winning player invested chip refund is not calculated or applied.                                                     |
|                         | **End-of-Round Chip Penalties / Taxation**        |  [ ]   | Taxing players based on card difference or 1 chip for losing Sabacc is not implemented.                               |
|                         | **Player Elimination on Zero Chips**              |  [ ]   | `Dealer::RemovePlayer` exists, but is not connected to active player elimination checks during gameplay.              |
|                         | **Game Over & Victory Condition**                 |  [ ]   | `Game::IsGameOver` returns `false` unconditionally; victory by last remaining player with chips is unimplemented.     |
| **GUI**                 | **need to think about this one**                  |  [ ]   | Still need to think about this one. first complete the base game                                                      |

