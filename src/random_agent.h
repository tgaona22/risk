#ifndef RANDOM_AGENT_H
#define RANDOM_AGENT_H

#include "i_agent.h"
#include "map.h"
#include "territory.h"

#include <tuple>
#include <vector>
#include <cstdlib>
#include <ctime>

class RandomAgent : public IAgent {
private:
public:
  RandomAgent(const Map& map, int id, sf::Color color);
  ~RandomAgent();

  const Territory* selectUnoccupiedTerritory(const std::map<std::string, Territory*>& unoccupied_territories) const;
  std::tuple<const Territory*, int> reinforce(int total_reinforcements) const;
  std::tuple<const Territory*, const Territory*, int> attack() const;
  int defend(const Territory *attacker, const Territory *defender, int attacking_units) const;
  int capture(const Territory *from, const Territory *to_capture, int attacking_units) const;
  std::tuple<const Territory*, const Territory*, int> fortify() const;

private:
  const Territory* getRandomTerritory() const;
  int getRandomInt(int from, int to) const;
};

#endif
