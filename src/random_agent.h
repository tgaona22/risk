#ifndef RANDOM_AGENT_H
#define RANDOM_AGENT_H

#include "i_agent.h"
#include "map.h"
#include "territory.h"

#include <tuple>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

class RandomAgent : public IAgent
{
private:
public:
  RandomAgent(const Map &map, int id, std::string name, sf::Color color);
  ~RandomAgent();

  const Territory *chooseTerritory(const std::vector<Territory *> &valid_territories, bool choose_unoccupied);
  std::tuple<const Territory *, int> reinforce(int total_reinforcements);
  std::tuple<const Territory *, const Territory *, int> attack();
  int defend(const Territory *attacker, const Territory *defender, int attacking_units);
  int capture(const Territory *from, const Territory *to_capture, int attacking_units);
  std::tuple<const Territory *, const Territory *, int> fortify();

private:
  const Territory *getRandomTerritory() const;
  int getRandomInt(int from, int to) const;
};

#endif
