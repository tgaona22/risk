#ifndef HUMAN_AGENT_H
#define HUMAN_AGENT_H

#include <map>
#include <string>

#include "i_agent.h"
#include "territory.h"
#include "map.h"
#include "console.h"

class HumanAgent : public IAgent {
private:
  Console& console;
public:
  HumanAgent(const Map& map, Console& console, int id, std::string name, sf::Color color);
  ~HumanAgent();

  const Territory* selectUnoccupiedTerritory(const std::map<std::string, Territory*>& unoccupied_territories);
  std::tuple<const Territory*, int> reinforce(int total_reinforcements);
  std::tuple<const Territory*, const Territory*, int> attack();
  int defend(const Territory *attacker, const Territory *defender, int attacking_units);
  int capture(const Territory *from, const Territory *to_capture, int attacking_units);
  std::tuple<const Territory*, const Territory*, int> fortify();

};

#endif
