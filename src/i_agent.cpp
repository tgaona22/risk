#include "i_agent.h"

int IAgent::getId() const {
  return id;
}

int IAgent::getNumberOfTerritories() const {
  return territories.size();
}

bool IAgent::hasTerritory(const Territory *territory) const {
  return territory->getOccupierId() == id;
}

bool IAgent::canAttack(const Territory *territory) const {
  for (auto iter = begin(territories); iter != end(territories); iter++) {
    const Territory *current_territory = iter->second;
    if (current_territory->hasNeighbor(territory)) {
      return true;
    }
  }
  return false;
}

void IAgent::addTerritory(const Territory *territory) {
  territories.insert(std::pair<std::string, const Territory*>(territory->getName(), territory));
}

void IAgent::removeTerritory(const Territory *territory) {
  territories.erase(territory->getName());
}

const sf::Color& IAgent::getColor() const { 
  return color;
}
