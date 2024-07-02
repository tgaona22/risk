#include "card.h"

std::string cardstr(const Card &c)
{
    return c.type + ":" + c.territory;
}