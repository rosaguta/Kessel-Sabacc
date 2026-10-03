#include <iostream>
#include <random>
using namespace std;

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
    int specialCards;
    int shiftTokens;
    int chips;
    Player()
    {
        generateNumberCards();
        specialCards = 0;
        shiftTokens = 0;
        chips = 0;
    }
    void generateNumberCards()
    {
        random_device rd;
        mt19937 gen(rd());

        uniform_int_distribution<int> familyDist(0, 1);
        uniform_int_distribution<int> numberDist(1, 6);
        for (int i = 0; i < 2; i++)
        {
            cards[i].cardFamily = CardFamily(familyDist(gen));
            cards[i].number = Number(numberDist(gen));
        }
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
class Dealer{
    public:
        int currentPlayerTurn;
        NumberCard numberCards[36]
};
int main()
{
    Player player1;
    Player player2;
    cout << "====HAND ONE====\r\n";
    player1.printCurrentHand();
    cout << "====HAND ONE====\r\n";
    cout << "====HAND TWO====\r\n";
    player2.printCurrentHand();
    cout << "====HAND TWO====\r\n";
    
    return 0;
}