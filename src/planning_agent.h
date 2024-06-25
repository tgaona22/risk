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

class PlanningAgent : public IAgent
{
private:
  bool has_attack_plan;
  const Territory *attack_target;
  int unit_budget;
  std::queue<std::tuple<const Territory *, int>> reinforcement_plan;

public:
  PlanningAgent(const Map &map, int id, std::string name, sf::Color color);
  ~PlanningAgent();

  const Territory *chooseTerritory(const std::vector<Territory *> &valid_territories, bool choose_unoccupied);
  std::tuple<const Territory *, int> reinforce(int total_reinforcements);
  std::tuple<const Territory *, const Territory *, int> attack();
  int defend(const Territory *attacker, const Territory *defender, int attacking_units);
  int capture(const Territory *from, const Territory *to_capture, int attacking_units);
  std::tuple<const Territory *, const Territory *, int> fortify();

  void informOfBattleOutcome(int attacker_id, int defender_id, int attacker_lost, int defender_lost, bool captured);

private:
  void allocateReinforcements(int total_reinforcements);
  void selectTarget();

  int countFriendlyNeighbors(const Territory *territory);
  int countWeakEnemyNeighbors(const Territory *territory);
  double calculateAverageThreat();
  double evaluateThreat(const Territory *territory);
  double evaluateAttackValue(const Territory *territory);

  std::vector<const Territory *> getTargets();

  std::tuple<double, double> simulateAttack(const Territory *target, int attackers_budget, int N);
  std::tuple<int, int> resolveBattle(int attacking_units, int defending_units);
  int rollDie();
  int getRandomInt(int from, int to);
  int findMax(int *arr, int size);

  static bool compareTerritories(std::tuple<const Territory *, double> first, std::tuple<const Territory *, double> second);
  int getRank(const Territory *territory, const std::vector<std::tuple<const Territory *, double>> &values);
};

#endif
