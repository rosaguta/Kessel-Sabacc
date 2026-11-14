// Game loop unit tests: round resolution, card handling and winner/loser evaluation.
// Failing checks describe behaviour that differs from the Kessel Sabacc rules v4.
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "Game.h"

static int passed = 0, failed = 0;
#define CHECK(cond, msg) do { if (cond) { ++passed; std::cout << "  PASS  " << msg << "\n"; } \
    else { ++failed; std::cout << "  FAIL  " << msg << "   [" #cond "]\n"; } } while (0)

static Dealer makeDealer(int n) {
    Dealer d;
    for (int i = 0; i < n; ++i) { Player p; d.AddParticipant(p); }
    d.DealCards();
    return d;
}
static void setHand(Dealer &d, int i, Number blood, Number sand) {
    d.players[i].bloodCard = {BLOOD, blood};
    d.players[i].sandCard = {SAND, sand};
    d.players[i].EvaluateMatchingPair();
}
static bool has(const std::vector<int> &v, int x) { return std::find(v.begin(), v.end(), x) != v.end(); }
// Every player must appear exactly once across winners and losers.
static bool eachPlayerOnce(const RoundResult &r, int playerCount) {
    if (static_cast<int>(r.winners.size() + r.losers.size()) != playerCount) return false;
    for (int i = 0; i < playerCount; ++i) {
        if (std::count(r.winners.begin(), r.winners.end(), i) + std::count(r.losers.begin(), r.losers.end(), i) != 1)
            return false;
    }
    return true;
}
static void printResult(const RoundResult &r) {
    std::cout << "        actual: winners={";
    for (size_t i = 0; i < r.winners.size(); ++i) std::cout << (i ? "," : "") << r.winners[i];
    std::cout << "} losers={";
    for (size_t i = 0; i < r.losers.size(); ++i) std::cout << (i ? "," : "") << r.losers[i];
    std::cout << "}\n";
}

int main() {
    std::cout << "== ROUND RESOLUTION (rules: losers lose their pot + penalty)\n";
    {
        Game g; Dealer d = makeDealer(2);
        setHand(d, 0, ONE, SIX); setHand(d, 1, TWO, THREE);
        RoundResult r = g.ResolveEndOfRound(d);
        CHECK(has(r.winners, 1) && has(r.losers, 0), "overtaken leader (1/6 vs 2/3) ends up in losers");
        std::cout << "        actual: winners=" << r.winners.size() << " losers=" << r.losers.size() << "\n";
    }
    {
        Game g; Dealer d = makeDealer(2);
        setHand(d, 0, TWO, TWO); setHand(d, 1, ONE, FOUR);
        d.players[1].SpendChips(); d.players[1].SpendChips(); // 6 chips, 2 in pot
        g.ResolveEndOfRound(d);
        CHECK(d.players[1].chips == 3, "loser loses 2-chip pot + 3 penalty (diff 1/4) -> 3 chips");
        std::cout << "        actual: chips=" << d.players[1].chips << "\n";
    }

    std::cout << "== MULTIPLE WINNERS (non-matching: everyone tied on lowest difference wins)\n";
    {
        // Differences: p0=3, p1=1, p2=1, p3=5
        Game g; Dealer d = makeDealer(4);
        setHand(d, 0, FIVE, TWO); setHand(d, 1, FOUR, THREE);
        setHand(d, 2, TWO, ONE);  setHand(d, 3, SIX, ONE);
        RoundResult r = g.ResolveEndOfRound(d);
        CHECK(r.winners.size() == 2 && has(r.winners, 1) && has(r.winners, 2), "p1 and p2 (diff 1) both win");
        CHECK(r.losers.size() == 2 && has(r.losers, 0) && has(r.losers, 3), "p0 (diff 3) and p3 (diff 5) lose");
        CHECK(eachPlayerOnce(r, 4), "every player appears exactly once");
        printResult(r);
    }
    {
        // Differences: p0=3, p1=1, p2=1 -> first leader is overtaken, then tied
        Game g; Dealer d = makeDealer(3);
        setHand(d, 0, ONE, FOUR); setHand(d, 1, TWO, THREE); setHand(d, 2, THREE, TWO);
        RoundResult r = g.ResolveEndOfRound(d);
        CHECK(r.winners.size() == 2 && has(r.winners, 1) && has(r.winners, 2), "overtaken then tied: p1 and p2 win");
        CHECK(r.losers.size() == 1 && has(r.losers, 0), "overtaken leader p0 loses");
        CHECK(eachPlayerOnce(r, 3), "every player appears exactly once");
        printResult(r);
    }
    {
        // All players tied
        Game g; Dealer d = makeDealer(3);
        setHand(d, 0, ONE, THREE); setHand(d, 1, FOUR, TWO); setHand(d, 2, SIX, FOUR);
        RoundResult r = g.ResolveEndOfRound(d);
        CHECK(r.winners.size() == 3 && r.losers.empty(), "all tied on diff 2 -> everyone wins");
        printResult(r);
    }
    {
        // Winners refunded, losers taxed
        Game g; Dealer d = makeDealer(3);
        // p2 must stay above 0 chips, otherwise it is eliminated and removed from d.players
        setHand(d, 0, FOUR, THREE); setHand(d, 1, TWO, ONE); setHand(d, 2, ONE, FIVE);
        for (auto &p : d.players) p.SpendChips(); // each 7 chips, 1 in pot
        g.ResolveEndOfRound(d);
        CHECK(d.players[0].chips == 8 && d.players[1].chips == 8, "both tied winners get their pot back");
        CHECK(d.players[2].chips < 7, "loser is taxed");
        std::cout << "        actual: chips=" << d.players[0].chips << "," << d.players[1].chips
                  << "," << d.players[2].chips << "\n";
    }

    std::cout << "== MULTIPLE WINNERS (matching pairs: everyone tied on lowest pair wins)\n";
    {
        Game g; Dealer d = makeDealer(3);
        setHand(d, 0, TWO, TWO); setHand(d, 1, TWO, TWO); setHand(d, 2, THREE, THREE);
        RoundResult r = g.ResolveEndOfRound(d);
        CHECK(r.winners.size() == 2 && has(r.winners, 0) && has(r.winners, 1), "two 2/2 pairs both win");
        CHECK(r.losers.size() == 1 && has(r.losers, 2), "3/3 loses");
        CHECK(eachPlayerOnce(r, 3), "every player appears exactly once");
        printResult(r);
    }
    {
        // Leader overtaken by a lower pair must end up in losers exactly once
        Game g; Dealer d = makeDealer(3);
        setHand(d, 0, FIVE, FIVE); setHand(d, 1, ONE, ONE); setHand(d, 2, ONE, FOUR);
        RoundResult r = g.ResolveEndOfRound(d);
        CHECK(r.winners.size() == 1 && has(r.winners, 1), "1/1 beats 5/5");
        CHECK(eachPlayerOnce(r, 3), "every player appears exactly once");
        printResult(r);
    }

    std::cout << "== KEEP DRAWN CARD (old card goes to discard)\n";
    {
        Dealer d = makeDealer(2);
        d.players[0].bloodCard = {BLOOD, ONE}; d.faceUpBloodCard = {BLOOD, FIVE};
        d.players[0].cardInHand = {BLOOD, SIX};
        d.HandlePlayerInHandCardToMainHand(0, true);
        CHECK(d.players[0].bloodCard.number == SIX, "player keeps drawn card");
        CHECK(d.faceUpBloodCard.number == ONE, "old card becomes face-up discard");
        std::cout << "        actual: hand=" << d.players[0].bloodCard.ToString()
                  << " discard=" << d.faceUpBloodCard.ToString() << "\n";
    }

    std::cout << "== IMPOSTOR VALUES\n";
    {
        // Impostor rolls 1-6, so Impostor+3 has a difference of at most 3 (or pairs on a 3)
        // and must always beat 1/5 (difference 4).
        int wins = 0;
        for (int k = 0; k < 50; ++k) {
            Game g; Dealer d = makeDealer(2);
            setHand(d, 0, IMPOSTOR, THREE); setHand(d, 1, ONE, FIVE);
            RoundResult r = g.ResolveEndOfRound(d);
            if (has(r.winners, 0) && !has(r.winners, 1)) ++wins;
        }
        CHECK(wins == 50, "Impostor+3 always beats 1/5 (50/50 runs)");
        std::cout << "        actual: " << wins << "/50\n";
    }

    std::cout << "== GAME FINISHED (game ends when only one player has chips left)\n";
    {
        Game g;
        CHECK(!g.IsGameOver(), "fresh game is not over");
    }
    {
        // Nobody runs out of chips -> game continues into the next round
        Game g; Dealer d = makeDealer(3);
        setHand(d, 0, ONE, TWO); setHand(d, 1, ONE, FOUR); setHand(d, 2, TWO, SIX);
        g.ResolveEndOfRound(d);
        CHECK(!g.IsGameOver(), "no player at 0 chips -> game not over");
        CHECK(d.players.size() == 3, "all 3 players still in the game");
        CHECK(g.currentRound == 2, "game advances to round 2");
        std::cout << "        actual: players=" << d.players.size() << " round=" << g.currentRound << "\n";
    }
    {
        // One of three players is knocked out -> two remain, game continues
        Game g; Dealer d = makeDealer(3);
        setHand(d, 0, ONE, TWO); setHand(d, 1, TWO, THREE); setHand(d, 2, SIX, ONE);
        d.players[2].chips = 1; // penalty pushes p2 to 0 or below
        g.ResolveEndOfRound(d);
        CHECK(!g.IsGameOver(), "one of three players eliminated -> game not over");
        CHECK(d.players.size() == 2, "eliminated player is removed");
        std::cout << "        actual: players=" << d.players.size() << "\n";
    }
    {
        // Two players, loser runs out of chips -> game over
        Game g; Dealer d = makeDealer(2);
        setHand(d, 0, ONE, TWO); setHand(d, 1, SIX, ONE);
        d.players[1].chips = 1;
        g.ResolveEndOfRound(d);
        CHECK(g.IsGameOver(), "last opponent eliminated -> game over");
        CHECK(d.players.size() == 1, "only the winner remains");
        CHECK(g.currentRound == 1, "round does not advance once the game is over");
        std::cout << "        actual: players=" << d.players.size() << " round=" << g.currentRound << "\n";
    }
    {
        // Two adjacent players knocked out in the same round -> game over
        Game g; Dealer d = makeDealer(3);
        setHand(d, 0, ONE, TWO); setHand(d, 1, SIX, ONE); setHand(d, 2, SIX, TWO);
        d.players[1].chips = 1; d.players[2].chips = 1;
        g.ResolveEndOfRound(d);
        CHECK(d.players.size() == 1, "both broke players are removed in the same round");
        CHECK(g.IsGameOver(), "two players eliminated at once -> game over");
        std::cout << "        actual: players=" << d.players.size() << "\n";
    }

    std::cout << "\n" << passed << " passed, " << failed << " failed\n";
    return failed ? 1 : 0;
}
