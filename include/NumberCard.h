#ifndef KESSEL_SABACC_CARDS_H
#define KESSEL_SABACC_CARDS_H

#pragma once

enum Number
{
    ONE = 1,
    TWO = 2,
    THREE = 3,
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

#endif //KESSEL_SABACC_CARDS_H
