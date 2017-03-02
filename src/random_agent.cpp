#include "random_agent.h"

RandomAgent::RandomAgent(const Map& map, int id) :
  IAgent(map, id, sf::Color::Red)
{
  // Seed the random number generator.
  std::srand((unsigned)std::time(0));
}

RandomAgent::~RandomAgent() {}

const Territory* RandomAgent::selectUnoccupiedTerritory(const std::map<std::string, Territory*>& unoccupied_territories) const {
  auto iter = begin(unoccupied_territories);
  return (*iter).second;
}

std::tuple<const Territory*, int> RandomAgent::reinforce(int total_reinforcements) const {
  const Territory *to_reinforce = getRandomTerritory();
  int reinforcements = getRandomInt(1, total_reinforcements);
  return std::make_tuple(to_reinforce, reinforcements);
}

std::tuple<const Territory*, const Territory*, int> RandomAgent::attack() const {
  if ((rand() % 100) <= 20) {
    return std::make_tuple(nullptr, nullptr, 0);
  }
  const Territory *from = getRandomTerritory();
  const std::vector<Territory*>& neighbors = from->getNeighbors();
  const Territory *to = neighbors.at(getRandomInt(0, neighbors.size() - 1));
  int max_units = ((from->getUnits() - 1) < 3) ? from->getUnits() - 1 : 3;
  int units = getRandomInt(1, max_units);
  return std::make_tuple(to, from, units);
}

int RandomAgent::defend(const Territory *attacker, const Territory *defender, int attacking_units) const {
  int max_units = (defender->getUnits() < 2) ? defender->getUnits() : 2;
  return getRandomInt(1, max_units);
}

int RandomAgent::capture(const Territory *from, const Territory *to_capture, int attacking_units) const {
  int max_units = from->getUnits() - 1;
  return getRandomInt(attacking_units, max_units);
}

const Territory* RandomAgent::getRandomTerritory() const {
  std::vector<std::string> names;
  for (auto iter = begin(territories); iter != end(territories); iter++) {
    names.push_back(iter->first);
  }
  // Select a random territory. 
  int index = getRandomInt(0, territories.size() - 1);
  return (territories.find(names.at(index)))->second;
}

int RandomAgent::getRandomInt(int from, int to) const {
  return (rand() % (to - from + 1)) + from;
}

