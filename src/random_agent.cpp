#include "random_agent.h"

RandomAgent::RandomAgent(const Map& map, int id) :
  IAgent(map, id, sf::Color::Red)
{}

RandomAgent::~RandomAgent() {}

const Territory* RandomAgent::selectUnoccupiedTerritory(const std::map<std::string, Territory*>& unoccupied_territories) const {
  auto iter = begin(unoccupied_territories);
  return (*iter).second;
}

