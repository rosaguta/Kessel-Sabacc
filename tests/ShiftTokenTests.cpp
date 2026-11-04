// Shift token unit tests. Failing checks describe behaviour that differs from the Kessel Sabacc rules v4.
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "Game.h"

static int passed = 0, failed = 0;
#define CHECK(cond, msg) do { if (cond) { ++passed; std::cout << "  PASS  " << msg << "\n"; } \
    else { ++failed; std::cout << "  FAIL  " << msg << "   [" #cond "]\n"; } } while (0)

static const std::vector<ShiftToken> ALL = {FREE_DRAW, REFUND, EXTRA_REFUND, GENERAL_TARIFF, TARGET_TARIFF,
    EMBARGO, MARKDOWN, IMMUNITY, GENERAL_AUDIT, TARGET_AUDIT, MAJOR_FRAUD, EMBEZZLEMENT, COOK_THE_BOOKS,
    EXHAUSTION, DIRECT_TRANSACTION, PRIME_SABACC};

static Dealer makeDealer(int n) {
    Dealer d;
    for (int i = 0; i < n; ++i) { Player p; p.shiftTokens = ALL; d.AddParticipant(p); }
    d.DealCards();
    return d;
}
static void setHand(Dealer &d, int i, Number blood, Number sand) {
    d.players[i].bloodCard = {BLOOD, blood};
    d.players[i].sandCard = {SAND, sand};
    d.players[i].EvaluateMatchingPair();
}
static bool use(Game &g, Dealer &d, int p, ShiftToken t, int target = -1, int sabacc = 1) {
    PlayerAction a{};
    a.playerId = p; a.type = ActionType::USE_TOKEN; a.token = t;
    a.targetPlayerIndex = target; a.sabaccNumber = sabacc;
    return g.ApplyAction(a, d);
}
static bool act(Game &g, Dealer &d, int p, ActionType t) {
    PlayerAction a{}; a.playerId = p; a.type = t;
    a.card = (t == ActionType::DRAW_SAND_DISCARD) ? d.players[p].sandCard : d.players[p].bloodCard;
    return g.ApplyAction(a, d);
}
static bool has(const std::vector<int> &v, int x) { return std::find(v.begin(), v.end(), x) != v.end(); }
static int countFamily(const Dealer &d, CardFamily f) {
    int n = static_cast<int>((f == BLOOD ? d.bloodNumberCards : d.sandNumberCards).size()) + 1; // + face-up
    return n + static_cast<int>(d.players.size()); // one card of each family per hand
}

int main() {
    std::cout << "== Generic token flow\n";
    {
        Game g; Dealer d = makeDealer(2);
        CHECK(use(g, d, 0, REFUND) == true, "owned token can be used");
        CHECK(std::count(d.players[0].shiftTokens.begin(), d.players[0].shiftTokens.end(), REFUND) == 0, "used token removed from hand");
        CHECK(d.players[0].usedShiftTokens.size() == 1, "used token recorded in usedShiftTokens");
        CHECK(use(g, d, 0, EXTRA_REFUND) == false, "second token in same turn rejected (CanUseToken)");
        d.players[0].CanUseToken = true;
        CHECK(use(g, d, 0, REFUND) == false, "same token cannot be used twice");
        Player p; // default player only owns FREE_DRAW, GENERAL_AUDIT, MARKDOWN
        d.players[1].shiftTokens = p.shiftTokens;
        CHECK(use(g, d, 1, PRIME_SABACC) == false, "token not owned is rejected");
        d.players[1].stoodThisTurn = true;
        CHECK(use(g, d, 1, FREE_DRAW) == false, "token rejected after player already stood");
    }

    std::cout << "== FREE_DRAW\n";
    {
        Game g; Dealer d = makeDealer(2);
        CHECK(use(g, d, 0, FREE_DRAW), "free draw accepted");
        CHECK(d.players[0].CanDrawForFree, "CanDrawForFree set");
        CHECK(act(g, d, 0, ActionType::DRAW_BLOOD_DECK), "draw after free draw succeeds");
        CHECK(d.players[0].chips == 8 && d.players[0].usedChips == 0, "draw cost nothing (chips 8, used 0)");
        d.players[0].chips = 0; d.players[0].CanDrawForFree = true;
        CHECK(act(g, d, 0, ActionType::DRAW_SAND_DISCARD), "free draw allows drawing with 0 chips");
        CHECK(d.players[0].chips == 0, "still 0 chips after free discard draw");
    }

    std::cout << "== REFUND / EXTRA_REFUND  (rules: return chips you paid this round; needs >=1 paid)\n";
    {
        Game g; Dealer d = makeDealer(2);
        CHECK(use(g, d, 0, REFUND) == false, "REFUND with 0 chips paid this round is rejected");
        CHECK(d.players[0].chips == 8, "REFUND with nothing paid does not create chips (chips == 8)");
    }
    {
        Game g; Dealer d = makeDealer(2);
        d.players[0].SpendChips(); d.players[0].SpendChips(); d.players[0].SpendChips(); // 5 chips, 3 in pot
        use(g, d, 0, REFUND);
        CHECK(d.players[0].chips == 7 && d.players[0].usedChips == 1, "REFUND moves 2 from pot to stock (7/1)");
        std::cout << "        actual: chips=" << d.players[0].chips << " used=" << d.players[0].usedChips << "\n";
        d.players[0].RefundChips();
        CHECK(d.players[0].chips <= 8, "after winning, total never exceeds the 8 chips owned");
        std::cout << "        actual after round win: chips=" << d.players[0].chips << "\n";
    }
    {
        Game g; Dealer d = makeDealer(2);
        d.players[0].SpendChips(); // 7 chips, 1 in pot
        use(g, d, 0, EXTRA_REFUND);
        CHECK(d.players[0].chips == 8 && d.players[0].usedChips == 0, "EXTRA_REFUND capped at chips in pot (8/0)");
        std::cout << "        actual: chips=" << d.players[0].chips << " used=" << d.players[0].usedChips << "\n";
    }

    std::cout << "== GENERAL_TARIFF (rules: others taxed 1 chip 'as if buying a card')\n";
    {
        Game g; Dealer d = makeDealer(3);
        d.players[2].immune = true;
        CHECK(use(g, d, 0, GENERAL_TARIFF), "accepted");
        CHECK(d.players[0].chips == 8, "user not taxed");
        CHECK(d.players[1].chips == 7, "other player loses 1 chip from stock");
        CHECK(d.players[1].usedChips == 1, "taxed chip goes to their pot, like buying a card");
        CHECK(d.players[2].chips == 8, "immune player untouched");
    }

    std::cout << "== TARGET_TARIFF\n";
    {
        Game g; Dealer d = makeDealer(3);
        CHECK(use(g, d, 0, TARGET_TARIFF, 0) == false, "self-target rejected");
        CHECK(use(g, d, 0, TARGET_TARIFF, 7) == false, "out-of-range target rejected");
        CHECK(use(g, d, 0, TARGET_TARIFF, 1), "valid target accepted");
        CHECK(d.players[1].chips == 6 && d.players[2].chips == 8, "only target loses 2 chips");
        d.players[0].CanUseToken = true; d.players[0].shiftTokens.push_back(TARGET_TARIFF);
        d.players[2].chips = 1;
        use(g, d, 0, TARGET_TARIFF, 2);
        CHECK(d.players[2].chips == 0, "tax clamps at 0 chips");
    }

    std::cout << "== EMBARGO (rules: NEXT player must stand)\n";
    {
        Game g; Dealer d = makeDealer(3);
        CHECK(use(g, d, 0, EMBARGO, 1), "accepted");
        CHECK(d.players[1].mustStand, "target mustStand set");
        CHECK(act(g, d, 1, ActionType::DRAW_BLOOD_DECK) == false, "embargoed player cannot draw");
        CHECK(use(g, d, 1, REFUND) == false, "embargoed player cannot use a token either");
        CHECK(act(g, d, 1, ActionType::STAND), "embargoed player can stand");
        Game g2; Dealer d2 = makeDealer(3);
        CHECK(use(g2, d2, 0, EMBARGO, 2), "can embargo a non-next player (rules say next player only)");
    }

    std::cout << "== MARKDOWN (Sylop = 0 until reveal)\n";
    {
        Game g; Dealer d = makeDealer(2);
        setHand(d, 0, SYLOP, THREE); setHand(d, 1, TWO, FOUR);
        RoundResult r = g.ResolveEndOfRound(d);
        CHECK(has(r.winners, 0), "baseline: Sylop+3 is a sabacc and beats 2/4");
    }
    {
        Game g; Dealer d = makeDealer(2);
        setHand(d, 0, SYLOP, THREE); setHand(d, 1, TWO, FOUR);
        CHECK(use(g, d, 1, MARKDOWN), "accepted");
        RoundResult r = g.ResolveEndOfRound(d);
        CHECK(has(r.winners, 1) && !has(r.winners, 0), "markdown: Sylop+3 (diff 3) loses to 2/4 (diff 2)");
        CHECK(!d.markdownActive, "markdown cleared after reveal");
    }
    {
        Game g; Dealer d = makeDealer(2);
        setHand(d, 0, SYLOP, SYLOP); setHand(d, 1, TWO, TWO);
        use(g, d, 1, MARKDOWN);
        RoundResult r = g.ResolveEndOfRound(d);
        CHECK(has(r.winners, 0), "markdown: Sylop pair still Pure Sabacc");
    }

    std::cout << "== IMMUNITY\n";
    {
        Game g; Dealer d = makeDealer(2);
        CHECK(use(g, d, 1, IMMUNITY), "accepted");
        CHECK(d.players[1].immune, "immune flag set");
        setHand(d, 0, ONE, TWO); setHand(d, 1, FIVE, FIVE);
        use(g, d, 0, DIRECT_TRANSACTION, 1);
        CHECK(d.players[1].bloodCard.number == FIVE, "immune player's hand not swapped");
        d.players[0].CanUseToken = true; use(g, d, 0, TARGET_TARIFF, 1);
        CHECK(d.players[1].chips == 8, "immune player not tariffed");
        d.players[0].CanUseToken = true; use(g, d, 0, EMBEZZLEMENT);
        CHECK(d.players[1].chips == 8, "immune player not embezzled");
        g.ResolveEndOfRound(d);
        CHECK(!d.players[1].immune, "immunity cleared after reveal");
    }
    {
        Game g; Dealer d = makeDealer(2);
        setHand(d, 0, SYLOP, THREE); setHand(d, 1, TWO, FOUR);
        use(g, d, 0, IMMUNITY);
        use(g, d, 1, MARKDOWN);
        RoundResult r = g.ResolveEndOfRound(d);
        CHECK(has(r.winners, 0), "immune player's Sylop not marked down");
    }

    std::cout << "== GENERAL_AUDIT (others in stand taxed 2)\n";
    {
        Game g; Dealer d = makeDealer(3);
        act(g, d, 1, ActionType::STAND);
        CHECK(use(g, d, 0, GENERAL_AUDIT), "accepted");
        CHECK(d.players[1].chips == 6, "standing player taxed 2");
        CHECK(d.players[2].chips == 8, "non-standing player untouched");
        CHECK(d.players[1].usedChips == 2, "audited chips are 'invested' into pot (rules wording)");
    }

    std::cout << "== TARGET_AUDIT (player in stand taxed 3)\n";
    {
        Game g; Dealer d = makeDealer(3);
        CHECK(use(g, d, 0, TARGET_AUDIT, 1) == false, "target not standing -> rejected");
        CHECK(std::count(d.players[0].shiftTokens.begin(), d.players[0].shiftTokens.end(), TARGET_AUDIT) == 1, "rejected token not consumed");
        act(g, d, 1, ActionType::STAND);
        CHECK(use(g, d, 0, TARGET_AUDIT, 1), "target standing -> accepted");
        CHECK(d.players[1].chips == 5, "target taxed 3");
    }

    std::cout << "== MAJOR_FRAUD (Impostor = 6)\n";
    {
        int wins = 0;
        for (int k = 0; k < 50; ++k) {
            Game g; Dealer d = makeDealer(2);
            setHand(d, 0, IMPOSTOR, SIX); setHand(d, 1, ONE, FOUR);
            use(g, d, 1, MAJOR_FRAUD);
            RoundResult r = g.ResolveEndOfRound(d);
            if (has(r.winners, 0) && !has(r.winners, 1)) ++wins;
        }
        CHECK(wins == 50, "Impostor+6 always becomes 6/6 sabacc and beats 1/4 (50/50 runs)");
        std::cout << "        actual: " << wins << "/50\n";
    }

    std::cout << "== EMBEZZLEMENT (rules: take 1 chip from each player's POT to your pot)\n";
    {
        Game g; Dealer d = makeDealer(3);
        d.players[1].SpendChips(); d.players[2].SpendChips(); // each has 1 in pot
        CHECK(use(g, d, 0, EMBEZZLEMENT), "accepted");
        std::cout << "        actual: p0 chips=" << d.players[0].chips << " used=" << d.players[0].usedChips
                  << " | p1 chips=" << d.players[1].chips << " used=" << d.players[1].usedChips << "\n";
        CHECK(d.players[0].usedChips == 2 && d.players[1].usedChips == 0, "pot-to-pot transfer");
        CHECK(d.players[0].chips + d.players[0].usedChips + d.players[1].chips + d.players[1].usedChips
              + d.players[2].chips + d.players[2].usedChips == 24, "chips conserved (24 total)");
    }

    std::cout << "== COOK_THE_BOOKS (6/6 best)\n";
    {
        Game g; Dealer d = makeDealer(2);
        setHand(d, 0, ONE, ONE); setHand(d, 1, SIX, SIX);
        use(g, d, 0, COOK_THE_BOOKS);
        RoundResult r = g.ResolveEndOfRound(d);
        CHECK(has(r.winners, 1) && has(r.losers, 0), "6/6 beats 1/1");
        CHECK(!d.cookTheBooksActive && d.sabacc == 1, "cook-the-books cleared after reveal");
    }
    {
        Game g; Dealer d = makeDealer(3);
        setHand(d, 0, TWO, TWO); setHand(d, 1, FOUR, FOUR); setHand(d, 2, THREE, THREE);
        use(g, d, 0, COOK_THE_BOOKS);
        RoundResult r = g.ResolveEndOfRound(d);
        CHECK(r.winners.size() == 1 && has(r.winners, 1), "4/4 beats 3/3 and 2/2");
        CHECK(has(r.losers, 0) && has(r.losers, 2), "2/2 and 3/3 are losers");
    }

    std::cout << "== EXHAUSTION (target discards and draws new hand)\n";
    {
        int changed = 0; bool conserved = true;
        for (int k = 0; k < 30; ++k) {
            Game g; Dealer d = makeDealer(2);
            NumberCard ob = d.players[1].bloodCard, os = d.players[1].sandCard;
            size_t bSize = d.bloodNumberCards.size();
            use(g, d, 0, EXHAUSTION, 1);
            if (d.players[1].bloodCard.number != ob.number || d.players[1].sandCard.number != os.number) ++changed;
            if (d.bloodNumberCards.size() != bSize || countFamily(d, BLOOD) != 22 || countFamily(d, SAND) != 22) conserved = false;
            if (d.players[1].bloodCard.cardFamily != BLOOD || d.players[1].sandCard.cardFamily != SAND) conserved = false;
        }
        CHECK(conserved, "deck sizes / families preserved");
        CHECK(changed > 15, "target's hand usually changes");
        std::cout << "        hand changed in " << changed << "/30 runs\n";
        Game g; Dealer d = makeDealer(2);
        d.players[1].immune = true; NumberCard ob = d.players[1].bloodCard;
        use(g, d, 0, EXHAUSTION, 1);
        CHECK(d.players[1].bloodCard.number == ob.number, "immune target unaffected");
    }

    std::cout << "== DIRECT_TRANSACTION (swap hands)\n";
    {
        Game g; Dealer d = makeDealer(2);
        setHand(d, 0, ONE, FOUR); setHand(d, 1, FIVE, FIVE);
        CHECK(use(g, d, 0, DIRECT_TRANSACTION, 1), "accepted");
        CHECK(d.players[0].bloodCard.number == FIVE && d.players[0].sandCard.number == FIVE, "user got 5/5");
        CHECK(d.players[1].bloodCard.number == ONE && d.players[1].sandCard.number == FOUR, "target got 1/4");
        CHECK(d.players[0].matchingPair && !d.players[1].matchingPair, "matchingPair re-evaluated");
    }

    std::cout << "== PRIME_SABACC (chosen value becomes best sabacc)\n";
    {
        Game g; Dealer d = makeDealer(2);
        setHand(d, 0, FOUR, FOUR); setHand(d, 1, ONE, ONE);
        CHECK(use(g, d, 0, PRIME_SABACC, -1, 4), "accepted");
        RoundResult r = g.ResolveEndOfRound(d);
        CHECK(has(r.winners, 0) && has(r.losers, 1), "4/4 prime beats 1/1");
        CHECK(!d.primeSabaccActive && d.sabacc == 1, "prime cleared after reveal");
    }
    {
        Game g; Dealer d = makeDealer(2);
        setHand(d, 0, FIVE, FIVE); setHand(d, 1, TWO, TWO);
        use(g, d, 0, PRIME_SABACC, -1, 4);
        RoundResult r = g.ResolveEndOfRound(d);
        CHECK(has(r.winners, 1) && has(r.losers, 0), "nobody holds prime -> normal ranking (2/2 beats 5/5)");
    }
    {
        Game g; Dealer d = makeDealer(2);
        CHECK(use(g, d, 0, PRIME_SABACC, -1, 9) == false, "out-of-range sabacc value (9) rejected");
        Game g2; Dealer d2 = makeDealer(2);
        CHECK(use(g2, d2, 0, PRIME_SABACC, -1, 0) == false, "out-of-range sabacc value (0) rejected");
    }

    std::cout << "\n" << passed << " passed, " << failed << " failed\n";
    return failed ? 1 : 0;
}
