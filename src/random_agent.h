#ifndef RANDOM_AGENT_H
#define RANDOM_AGENT_H

#include "i_agent.h"
#include "map.h"
#include "territory.h"

class RandomAgent : public IAgent {
private:
public:
  RandomAgent(const Map& map, int id);
  ~RandomAgent();

  const Territory* selectUnoccupiedTerritory(const std::map<std::string, Territory*>& unoccupied_territories) const;
};

#endif
