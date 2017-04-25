#ifndef PLANNING_AGENT_H
#define PLANNING_AGENT_H

#include "i_agent.h"
#include "map.h"
#include "territory.h"

// For testing purposes...
#include "random_agent.h"

#include <map>
#include <vector>
#include <queue>
#include <algorithm>

class PlanningAgent : public IAgent {
private:
  // For testing purposes...
  RandomAgent randy;

  std::queue<std::tuple<const Territory*, int>> reinforcement_plan;
public:
  PlanningAgent(const Map& map, int id, sf::Color color);
  ~PlanningAgent();

  const Territory* selectUnoccupiedTerritory(const std::map<std::string, Territory*>& unoccupied_territories);
  std::tuple<const Territory*, int> reinforce(int total_reinforcements);
  std::tuple<const Territory*, const Territory*, int> attack();
  int defend(const Territory *attacker, const Territory *defender, int attacking_units);
  int capture(const Territory *from, const Territory *to_capture, int attacking_units);
  std::tuple<const Territory*, const Territory*, int> fortify();

private:
  void allocateReinforcements(int total_reinforcements);
  
  int neighborCount(const Territory *territory);
  double evaluateThreat(const Territory *territory);

  bool compareTerritories(std::tuple<const Territory*, double> first, std::tuple<const Territory*, double> second);
  std::vector<std::tuple<const Territory*, double>> sortTerritories(std::vector<std::tuple<const Territory*, double>>);
};

#endif
