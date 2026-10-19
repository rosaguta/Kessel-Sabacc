#ifndef KESSEL_SABACC_CARDS_H
#define KESSEL_SABACC_CARDS_H

#pragma once
#include <string>
enum Number
{
    ONE = 1,
    TWO = 2,
    THREE = 3,
    FOUR = 4,
    FIVE = 5,
    SIX = 6,
    IMPOSTOR = 7,
    IMPOSTOR_1 = -1, // can use std::abs() to get the positive value of this number
    IMPOSTOR_2 = -2,
    IMPOSTOR_3 = -3,
    IMPOSTOR_4 = -4,
    IMPOSTOR_5 = -5,
    IMPOSTOR_6 = -6,
    SYLOP = 8,

};

enum CardFamily
{
    SAND,
    BLOOD
};

struct NumberCard
{
    CardFamily cardFamily;
    mutable Number number;
    std::string ToString() const {
        std::string familyName;
        std::string nmr;
        switch (cardFamily) {
            case SAND:
                familyName = "SAND";
                break;
            case BLOOD:
                familyName = "BLOOD";
                break;
            default:
                familyName = "Unknown";
                break;
        }
        switch (number) {
            case ONE:
                nmr = "1";
                break;

            case TWO:
                nmr = "2";
                break;

            case THREE:
                nmr = "3";
                break;

            case FOUR:
                nmr = "4";
                break;

            case FIVE:
                nmr = "5";
                break;

            case SIX:
                nmr = "6";
                break;

            case IMPOSTOR:
                nmr = "Impostor";
                break;

            case IMPOSTOR_1:
                nmr = "Impostor(1)";
                break;

            case IMPOSTOR_2:
                nmr = "Impostor(2)";
                break;

            case IMPOSTOR_3:
                nmr = "Impostor(3)";
                break;

            case IMPOSTOR_4:
                nmr = "Impostor(4)";
                break;

            case IMPOSTOR_5:
                nmr = "Impostor(5)";
                break;

            case IMPOSTOR_6:
                nmr = "Impostor(6)";
                break;

            case SYLOP:
                nmr = "Sylop";
                break;

            default:
                nmr = "Unknown";
        }
        return familyName + "[" + nmr + "]";
    }
};

#endif //KESSEL_SABACC_CARDS_H
