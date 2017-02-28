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
  return std::make_pair(to_reinforce, reinforcements);
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

