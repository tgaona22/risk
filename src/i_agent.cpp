#include "i_agent.h"

int IAgent::getId() const
{
  return id;
}

const std::string &IAgent::getName() const
{
  return name;
}

int IAgent::getNumberOfTerritories() const
{
  return territories.size();
}

bool IAgent::hasTerritory(const Territory *territory) const
{
  return territory->getOccupierId() == id;
}

bool IAgent::canAttack(const Territory *territory) const
{
  for (auto iter = begin(territories); iter != end(territories); iter++)
  {
    const Territory *current_territory = *iter;
    if (current_territory->hasNeighbor(territory))
    {
      return true;
    }
  }
  return false;
}

void IAgent::addTerritory(const Territory *territory)
{
  territories.push_back(territory);
}

void IAgent::removeTerritory(const Territory *territory)
{
  auto iter = begin(territories);
  while ((*iter) != territory)
  {
    iter++;
  }
  territories.erase(iter);
}

const sf::Color &IAgent::getColor() const
{
  return color;
}

bool IAgent::drawCard()
{
  return true;
}

CardSet IAgent::turnInCards(const std::vector<std::tuple<int, int, int>> &card_sets)
{
  // A default implementation will be to return the first card set.
  std::tuple<int, int, int> t = card_sets.at(0);
  int i = std::get<0>(t);
  int j = std::get<1>(t);
  int k = std::get<2>(t);
  CardSet cards_turned_in = std::tie(cards.at(i), cards.at(j), cards.at(k));
  return cards_turned_in;
}

void IAgent::setOpponents(const std::vector<IAgent *> &players)
{
  for (const IAgent *player : players)
  {
    if (player->getId() != id)
    {
      opponents.push_back(player);
    }
  }
}