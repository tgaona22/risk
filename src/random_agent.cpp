#include "random_agent.h"

#include <iostream>

RandomAgent::RandomAgent(const Map& map, int id, std::string name, sf::Color color) :
  IAgent(map, id, name, color)
{
  // Seed the random number generator.
  std::srand((unsigned)std::time(0));
}

RandomAgent::~RandomAgent() {}

const Territory* RandomAgent::selectUnoccupiedTerritory(const std::map<std::string, Territory*>& unoccupied_territories) {
  auto iter = begin(unoccupied_territories);
  return (*iter).second;
}

std::tuple<const Territory*, int> RandomAgent::reinforce(int total_reinforcements) {
  const Territory *to_reinforce = getRandomTerritory();
  int reinforcements = getRandomInt(1, total_reinforcements);
  return std::make_tuple(to_reinforce, reinforcements);
}

std::tuple<const Territory*, const Territory*, int> RandomAgent::attack() {
  // If there is no territory with more than one unit, agent must pass.
  bool pass = true;
  std::vector<const Territory*> possible_attackers;
  for (auto iter = begin(territories); iter != end(territories); iter++) {
    const Territory *current_territory = *iter;
    if (current_territory->getUnits() > 1) {
      possible_attackers.push_back(current_territory);
      pass = false;
    }
  }
  if (pass || (rand() % 100) <= 20) {
    return std::make_tuple(nullptr, nullptr, 0);
  }

  const Territory *from = possible_attackers.at(getRandomInt(0, possible_attackers.size() - 1));

  const std::vector<Territory*>& neighbors = from->getNeighbors();
  const Territory *to = neighbors.at(getRandomInt(0, neighbors.size() - 1));
  int max_units = ((from->getUnits() - 1) < 3) ? from->getUnits() - 1 : 3;
  int units = getRandomInt(1, max_units);
  return std::make_tuple(to, from, units);
}

int RandomAgent::defend(const Territory *attacker, const Territory *defender, int attacking_units) {
  int max_units = (defender->getUnits() < 2) ? defender->getUnits() : 2;
  return getRandomInt(1, max_units);
}

int RandomAgent::capture(const Territory *from, const Territory *to_capture, int attacking_units) {
  int max_units = from->getUnits() - 1;
  return getRandomInt(attacking_units, max_units);
}

std::tuple<const Territory*, const Territory*, int> RandomAgent::fortify() {
  std::vector<const Territory*> possible_fortifiers;
  for (auto iter = begin(territories); iter != end(territories); iter++) {
    const Territory *current_territory = *iter;
    if (current_territory->getUnits() > 1) {
      possible_fortifiers.push_back(current_territory);
    }
  }

  if (possible_fortifiers.size() == 0 || (rand() % 100) <= 20) {
    return std::make_tuple(nullptr, nullptr, 0);
  }

  const Territory *to, *from; 
  from = possible_fortifiers.at(getRandomInt(0, possible_fortifiers.size() - 1));
  to = possible_fortifiers.at(getRandomInt(0, possible_fortifiers.size() - 1));
  int fortifying_units = getRandomInt(1, from->getUnits() - 1);
  return std::make_tuple(to, from, fortifying_units);
}

const Territory* RandomAgent::getRandomTerritory() const {
  return territories.at(getRandomInt(0, territories.size() - 1));
}

int RandomAgent::getRandomInt(int from, int to) const {
  return (rand() % (to - from + 1)) + from;
}
