#include <iostream>
#include <random>
#include <array>
#include <algorithm>
using namespace std;
/* This is a hard max count because kessel sabac in Star Wars Outlaws only shows a game with max 4 people.
This game is based on the gameplay shown in the video game*/
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
    NumberCard bloodCard;
    NumberCard sandCard;
    int specialCards;
    // TODO: the player must define the shift token before the game starts.
    std::vector<ShiftToken> shiftTokens;
    int chips;
    Player()
    {
        specialCards = 0;
        shiftTokens = std::vector<ShiftToken>{FREE_DRAW, GENERAL_AUDIT, MARKDOWN};
        chips = 8;
    }

    void printCurrentHand()
    {
        cout << "==The current hand is== \r\n";
        cout << "Cards:\r\n";
        cout << "  " << getCardFamilyName(bloodCard.cardFamily)
             << "\t" << getNumberName(bloodCard.number)
             << "\n";
        cout << "  " << getCardFamilyName(sandCard.cardFamily)
             << "\t" << getNumberName(sandCard.number)
             << "\n";
        cout << "  Shift tokens:  ";
        for (const auto &token : shiftTokens)
        {
            cout << token << " ";
        }
        cout << "\n";

        // cout << "Other unimplemented values:\r\n";
        // cout << "  Special cards: " << specialCards << "\n";
        // cout << "  Chips:         " << chips << "\n";
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
            player.bloodCard = bloodNumberCards.at(bloodIndex++);
            player.sandCard = sandNumberCards.at(sandIndex++);
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
    for (Player &player : dealer.players)
    {
        player.printCurrentHand();
    }

    return 0;
};
