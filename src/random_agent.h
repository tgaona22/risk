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
  RandomAgent(const Map& map, int id);
  ~RandomAgent();

  const Territory* selectUnoccupiedTerritory(const std::map<std::string, Territory*>& unoccupied_territories) const;
  std::tuple<const Territory*, int> reinforce(int total_reinforcements) const;
private:
  const Territory* getRandomTerritory() const;
  int getRandomInt(int from, int to) const;
};

#endif
