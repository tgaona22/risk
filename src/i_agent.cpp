#include "i_agent.h"

int IAgent::getId() const {
  return id;
}

const std::string& IAgent::getName() const {
  return name;
}

int IAgent::getNumberOfTerritories() const {
  return territories.size();
}

bool IAgent::hasTerritory(const Territory *territory) const {
  return territory->getOccupierId() == id;
}

bool IAgent::canAttack(const Territory *territory) const {
  for (auto iter = begin(territories); iter != end(territories); iter++) {
    const Territory *current_territory = *iter;
    if (current_territory->hasNeighbor(territory)) {
      return true;
    }
  }
  return false;
}

void IAgent::addTerritory(const Territory *territory) {
  territories.push_back(territory);
}

void IAgent::removeTerritory(const Territory *territory) {
  auto iter = begin(territories);
  while ((*iter) != territory) {
    iter++;
  }
  territories.erase(iter);
}

const sf::Color& IAgent::getColor() const { 
  return color;
}
