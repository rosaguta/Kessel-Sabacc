#include <iostream>
#include <random>
#include <array>
#include <algorithm>
using namespace std;
#define MAX_PLAYERCOUNT 4
enum Number
{
    ONE = 1,
    TWO = 2,
    TRHEE = 3,
    FOUR = 4,
    FIVE = 5,
    SIX = 6
};
enum CardFamily
{
    SAND,
    BLOOD
};
struct NumberCard
{
    CardFamily cardFamily;
    Number number;
};

enum ShiftToken
{
    FREE_DRAW,
    REFUND,
    GENERAL_TARIFF,
    TARGET_TARIFF,
    EMBARGO,
    MARKDOWN,
    IMMUNITY,
    GENERAL_AUDIT,
    MAJOR_FRAUD
};
class Player
{
private:
    string getCardFamilyName(CardFamily family)
    {
        switch (family)
        {
        case SAND:
            return "Sand";
        case BLOOD:
            return "Blood";
        default:
            return "Unknown";
        }
    }

    string getNumberName(Number number)
    {
        switch (number)
        {
        case ONE:
            return "One";
        case TWO:
            return "Two";
        case TRHEE:
            return "Three";
        case FOUR:
            return "Four";
        case FIVE:
            return "Five";
        case SIX:
            return "Six";
        default:
            return "Unknown";
        }
    }

public:
    NumberCard cards[2];
    // TODO: Refactor the array above to 2 seperate cards
    // because you always have 1 of each family in hand
    NumberCard bloodCard;
    NumberCard sandCard;
    int specialCards;
    int shiftTokens;
    int chips;
    Player()
    {
        specialCards = 0;
        shiftTokens = 0;
        chips = 0;
    }

    void printCurrentHand()
    {
        cout << "==The current hand is== \r\n";
        cout << "Cards:\r\n";
        for (int i = 0; i < 2; i++)
        {
            cout << "  " << getCardFamilyName(cards[i].cardFamily)
                 << "  " << getNumberName(cards[i].number)
                 << "\n";
        }
        cout << "Other unimplemented values:\r\n";
        cout << "  Special cards: " << specialCards << "\n";
        cout << "  Shift tokens:  " << shiftTokens << "\n";
        cout << "  Chips:         " << chips << "\n";
    }
};
class Dealer
{
private:
    void GenerateCards(std::array<NumberCard, 18> &cards, CardFamily family)
    {
        int count = 0;
        for (int i = 0; i < 3; i++)
        {
            for (int j = 1; j < 7; j++)
            {
                cards[count].cardFamily = family;
                cards[count].number = static_cast<Number>(j);
                count++;
            };
        }
    }

public:
    int currentPlayerTurn;
    std::array<NumberCard, 18> bloodNumberCards;
    std::array<NumberCard, 18> sandNumberCards;
    std::vector<Player> players;
    int imposterCards = 6;
    int sylopCards = 2;
    Dealer()
    {
        GenerateCards(bloodNumberCards, CardFamily::BLOOD);
        GenerateCards(sandNumberCards, CardFamily::SAND);
    }
    bool AddParticipant(const Player &player)
    {
        if (players.size() >= MAX_PLAYERCOUNT)
        {
            cout << "A player could not be added because the table is full\r\n";
            return false;
        }
        players.push_back(player);
        return true;
    }
    void DealCards()
    {
        std::random_device rd;
        std::mt19937 generator(rd());

        std::shuffle(bloodNumberCards.begin(), bloodNumberCards.end(), generator);
        std::shuffle(sandNumberCards.begin(), sandNumberCards.end(), generator);

        size_t bloodIndex = 0;
        size_t sandIndex = 0;

        for (Player &player : players)
        {
            player.cards[0] = bloodNumberCards.at(bloodIndex++);
            player.cards[1] = sandNumberCards.at(sandIndex++);
        }
    }
};
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
    dealer.DealCards();

    // Player player1;
    // Player player2;
    // cout << "====HAND ONE====\r\n";
    // player1.printCurrentHand();
    // cout << "====HAND ONE====\r\n";
    // cout << "====HAND TWO====\r\n";
    // player2.printCurrentHand();
    // cout << "====HAND TWO====\r\n";

    return 0;
};