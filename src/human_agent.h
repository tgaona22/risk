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
  std::tuple<const Territory*, int> reinforce(int total_reinforcements) const;
  std::tuple<const Territory*, const Territory*, int> attack() const;
  int defend(const Territory *attacker, const Territory *defender, int attacking_units) const;
  int capture(const Territory *from, const Territory *to_capture, int attacking_units) const;

};

#endif
