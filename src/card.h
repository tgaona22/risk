#ifndef CARD_H
#define CARD_H

#include <string>

struct Card
{
    int id;
    std::string territory;
    std::string type;
};

typedef std::tuple<Card, Card, Card> CardSet;

std::string cardstr(const Card &c);

#endif CARD_H