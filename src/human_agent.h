#ifndef HUMAN_AGENT_H
#define HUMAN_AGENT_H

#include <map>

#include "i_agent.h"
#include "territory.h"
#include "map.h"
#include "console.h"

class HumanAgent : public IAgent {
private:
  Console& console;
public:
  HumanAgent(const Map& map, Console& console, int id);
  ~HumanAgent();

  const Territory* selectUnoccupiedTerritory(const std::map<std::string, Territory*>& unoccupied_territories) const;
};

#endif
