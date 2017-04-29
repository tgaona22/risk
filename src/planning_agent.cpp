#include "planning_agent.h"

#include <cmath>
#include <climits>
// Testing purposes...
#include <iostream>

PlanningAgent::PlanningAgent(const Map& map, int id, sf::Color color) :
  IAgent(map, id, color), randy(map, id, color), unit_budget(0), has_attack_plan(false)
{}

PlanningAgent::~PlanningAgent() {}

const Territory* PlanningAgent::selectUnoccupiedTerritory(const std::map<std::string, Territory*>& unoccupied_territories) {
  // Prefer to choose territories that are adjacent to currently owned territories.
  const Territory *to_select;
  int max_adjacent = -1;
  for (auto iter = begin(unoccupied_territories); iter != end(unoccupied_territories); iter++) {
    const Territory *current_territory = iter->second;
    int count = countFriendlyNeighbors(current_territory);
    if (max_adjacent < count) {
      to_select = current_territory;
      max_adjacent = count;
    }
  }
  return to_select;
}

std::tuple<const Territory*, int> PlanningAgent::reinforce(int total_reinforcements) {
  if (reinforcement_plan.empty()) {
    allocateReinforcements(total_reinforcements);
  }
  std::tuple<const Territory*, int> to_return = reinforcement_plan.front();
  reinforcement_plan.pop();
  return to_return;
}

std::tuple<const Territory*, const Territory*, int> PlanningAgent::attack() {
  // If we have spent our allotted units, pass.
  if (unit_budget == 0 && has_attack_plan) {
    has_attack_plan = false;
    return std::make_tuple(nullptr, nullptr, 0);
  }

  if (!attack_target || !has_attack_plan || hasTerritory(attack_target)) {
    has_attack_plan = true;
    selectTarget();
  }

  // Choose where we will attack from.
  const std::vector<Territory*>& neighbors = attack_target->getNeighbors();
  const Territory *attacker = nullptr;
  double least_threat = INT_MAX;
  for (auto iter = begin(neighbors); iter != end(neighbors); iter++) {
    if (hasTerritory(*iter) && (*iter)->getUnits() > 1) {
      double threat = evaluateThreat(*iter); 
      if (threat < least_threat) {
	attacker = *iter;
	least_threat = threat;
      }
    }
  }
  
  int attacking_units = std::min(unit_budget, std::min(3, attacker->getUnits() - 1));
  // In the informOfBattleOutcome method we update the unit budget with the number of lost troops.  
  std::cout << "Planner tries to attack " << attack_target->getName() << " from " 
	    << attacker->getName() << " with " << attacking_units << " units.\n";
  return std::make_tuple(attack_target, attacker, attacking_units);
}

int PlanningAgent::defend(const Territory *attacker, const Territory *defender, int attacking_units) {
  return getRandomInt(1, std::min(2, defender->getUnits()));
}

int PlanningAgent::capture(const Territory *from, const Territory *to_capture, int attacking_units) {
  return randy.capture(from, to_capture, attacking_units);
}

std::tuple<const Territory*, const Territory*, int> PlanningAgent::fortify() {
  return randy.fortify();
}

void PlanningAgent::informOfBattleOutcome(int attacker_id, int defender_id, int attacker_lost, int defender_lost, bool captured) {
  if (attacker_id == id) {
    unit_budget = unit_budget - attacker_lost;
    std::cout << "Informed of battle outcome: we lost " << attacker_lost << " units. unit_budget = " << unit_budget << std::endl;
  }
}

void PlanningAgent::selectTarget() {
  // Determine how many units we are willing to spend in this round of attacks.
  if (unit_budget == 0) {
    double available_units = 0;
    for (auto iter = begin(territories); iter != end(territories); iter++) {
      available_units = available_units + ((*iter)->getUnits() - 1);
    }
    double fraction = .5;
    unit_budget = static_cast<int>(ceil(available_units * fraction));
  }
  // Get a list of eligible targets.
  std::vector<const Territory*> targets = getTargets();
  std::vector<std::tuple<const Territory*, double>> average_wins;
  std::vector<std::tuple<const Territory*, double>> average_unit_losses;
  std::vector<std::tuple<const Territory*, double>> attack_values;
  // For each possible target run an attack simulation.
  for (auto iter = begin(targets); iter != end(targets); iter++) {
    double avg_wins, avg_units_lost;
    std::tie(avg_wins, avg_units_lost) = simulateAttack(*iter, unit_budget, 500);
    average_wins.push_back(std::make_tuple(*iter, avg_wins));
    average_unit_losses.push_back(std::make_tuple(*iter, avg_units_lost));
    attack_values.push_back(std::make_tuple(*iter, evaluateAttackValue(*iter)));

    std::cout << (*iter)->getName() << ": " << avg_wins << "; " << avg_units_lost << "; "
	      << evaluateAttackValue(*iter) << std::endl;
  }
  // Sort each list. Also, we value losing few units, so reverse that list.
  std::sort(begin(average_wins), end(average_wins), PlanningAgent::compareTerritories);
  std::sort(begin(average_unit_losses), end(average_unit_losses), PlanningAgent::compareTerritories);
  std::reverse(begin(average_unit_losses), end(average_unit_losses));
  std::sort(begin(attack_values), end(attack_values), PlanningAgent::compareTerritories);
  
  int best_rank = INT_MAX;
  for (auto iter = begin(targets); iter != end(targets); iter++) {
    // A lower rank means a better choice.
    int rank = getRank(*iter, average_wins) + getRank(*iter, average_unit_losses) + getRank(*iter, attack_values);
    std::cout << (*iter)->getName() << " has rank " << rank << std::endl;
    if (rank < best_rank) {
      attack_target = *iter;
      best_rank = rank;
    }
  }
  std::cout << "The target is " << attack_target->getName() << std::endl;
}

void PlanningAgent::allocateReinforcements(int total_reinforcements) {
  // Determine the threat rating of each of our territories.
  std::vector<std::tuple<const Territory*, double>> rated_territories;
  double total_threat = 0;
  for (auto iter = begin(territories); iter != end(territories); iter++) {
    double threat = evaluateThreat(*iter);
    total_threat = total_threat + threat;
    rated_territories.push_back(std::make_tuple(*iter, threat));
  }
  // Determine how to divide up the reinforcements.
  // Precisely, allocate a percentage of units commensurate with the percentage of total threat
  // that the territory faces.
  // In the future, may want to incorporate offensive strategy here...
  int allocated = 0;
  std::vector<int> reinforcements;
  for (auto iter = begin(rated_territories); iter != end(rated_territories); iter++) {
    double threat = std::get<1>(*iter);
    double threat_percentage = threat / total_threat;
    int to_allocate = static_cast<int>(std::floor(threat_percentage * total_reinforcements));

    const Territory *t = std::get<0>(*iter);
    std::cout << t->getName() << " has threat = " << threat << " and threat percentage = " << threat_percentage << std::endl;
    std::cout << "Allocating " << to_allocate << " units to " << t->getName() << std::endl;

    allocated = allocated + to_allocate;
    reinforcements.push_back(to_allocate);
  }
  // If there are any leftover units, distribute them 'evenly'.
  auto iter = begin(reinforcements);
  while (total_reinforcements - allocated > 0) {
    if (iter == end(reinforcements)) {
      iter = begin(reinforcements);
    }
    *iter = (*iter) + 1;
    allocated = allocated + 1;
  }
  // Add the territory, reinforcement pairs to the queue.
  for (int i = 0; i < rated_territories.size(); i++) {
    const Territory *territory = std::get<0>(rated_territories.at(i));
    reinforcement_plan.push(std::make_tuple(territory, reinforcements.at(i)));
  }
}

// Runs a simulated attack N times. Returns the percentage of successful captures and the average
// number of units lost.
std::tuple<double, double> PlanningAgent::simulateAttack(const Territory *target, int attackers_budget, int N) {
  // First, get an upper bound on how many units we can attack the target with.
  int total_attackers = 0;
  const std::vector<Territory*>& neighbors = target->getNeighbors();
  for (auto iter = begin(neighbors); iter != end(neighbors); iter++) {
    if (hasTerritory(*iter)) {
      // Remember at least one unit must be left in each territory.
      total_attackers = total_attackers + (*iter)->getUnits() - 1;
    }
  }
  total_attackers = std::min(total_attackers, attackers_budget);

  std::cout << "Total attackers of " << target->getName() << ": " << total_attackers 
	    << "; Defenders: " << target->getUnits() << std::endl;

  int total_wins = 0, total_units_lost = 0;
  // Run an attack simulation N times.
  for (int n = 0; n < N; n++) {
    int sim_attackers = total_attackers;
    int sim_defenders = target->getUnits();
    while (sim_defenders > 0 && sim_attackers > 0) {
      int lost_attackers, lost_defenders;
      std::tie(lost_attackers, lost_defenders) = resolveBattle(std::min(3, sim_attackers), std::min(getRandomInt(1,2), sim_defenders));
      sim_attackers = sim_attackers - lost_attackers;
      sim_defenders = sim_defenders - lost_defenders;
      total_units_lost = total_units_lost + lost_attackers;
    }
    if (sim_defenders <= 0) {
      total_wins = total_wins + 1;
    }
  }
  
  return std::make_tuple(static_cast<double>(total_wins) / N, static_cast<double>(total_units_lost) / N);
}

// Returns a list of territories that can be attacked by the agent.
std::vector<const Territory*> PlanningAgent::getTargets() {
  std::vector<const Territory*> targets;
  for (auto iter_t = begin(territories); iter_t != end(territories); iter_t++) {
    const std::vector<Territory*>& neighbors = (*iter_t)->getNeighbors();
    for (auto iter_n = begin(neighbors); iter_n != end(neighbors); iter_n++) {
      // Only add the neighbor if the agent does not control it and it isn't already in the list.
      if (!hasTerritory(*iter_n) && std::find(begin(targets), end(targets), *iter_n) == end(targets)) {
	targets.push_back(*iter_n);
      }
    }
  }
  return targets;
}

// Returns the number of friendly neighbors of the territory.
int PlanningAgent::countFriendlyNeighbors(const Territory *territory) {
  int count = 0;
  for (auto iter = begin(territories); iter != end(territories); iter++) {
    if (territory->hasNeighbor(*iter)) {
      count = count + 1;
    }
  }
  return count;
}

int PlanningAgent::countWeakEnemyNeighbors(const Territory *territory) {
  // Define a territory to be weak if it has less than some percentage of the average unit number.
  double unit_average = map.getUnitAverage();
  double percentage = .5;
  int count = 0;
  const std::vector<Territory*>& neighbors = territory->getNeighbors();
  for (auto iter = begin(neighbors); iter != end(neighbors); iter++) {
    // Only count territories not under the agent's control.
    if (!hasTerritory(*iter) && (*iter)->getUnits() < (unit_average * percentage)) {
      count = count + 1;
    }
  }
  return count;
}

double PlanningAgent::evaluateThreat(const Territory *territory) {
  // Define threat as the ratio of nearby enemies to units in the territory.
  int adjacent_enemies = 0;
  const std::vector<Territory*>& neighbors = territory->getNeighbors();
  for (auto iter = begin(neighbors); iter != end(neighbors); iter++) {
    Territory *neighbor = *iter;
    if (!hasTerritory(neighbor)) {
      adjacent_enemies = adjacent_enemies + neighbor->getUnits();
    }
  }
  return adjacent_enemies / static_cast<double>(territory->getUnits());
}

double PlanningAgent::evaluateAttackValue(const Territory *territory) {
  double value = 0;
  // Increase value if it is adjacent to weakly defended territories.
  value = value + countWeakEnemyNeighbors(territory);
  // Increase value if it has many adjacent friendly territories.
  value = value + countFriendlyNeighbors(territory);

  return value;
}

bool PlanningAgent::compareTerritories(std::tuple<const Territory*, double> first, std::tuple<const Territory*, double> second) {
  double first_value, second_value;
  std::tie(std::ignore, first_value) = first;
  std::tie(std::ignore, second_value) = second;
  return first_value > second_value;
}

int PlanningAgent::getRank(const Territory *territory, const std::vector<std::tuple<const Territory*, double>>& values) {
  auto pos = std::find_if(begin(values), end(values), [territory](std::tuple<const Territory*, double> pair)
			  { return std::get<0>(pair) == territory; });
  return pos - begin(values);
}

std::tuple<int, int> PlanningAgent::resolveBattle(int attacking_units, int defending_units) {
  int *attacker_dice = new int[attacking_units];
  int *defender_dice = new int[defending_units];
  // Roll the dice.
  for (int i = 0; i < attacking_units; i++) {
    attacker_dice[i] = rollDie();
  }
  for (int i = 0; i < defending_units; i++) {
    defender_dice[i] = rollDie();
  }

  int count = 0;
  int min_rolls = (attacking_units < defending_units) ? attacking_units : defending_units;
  int attacker_loss = 0, defender_loss = 0;
  while (count < min_rolls) {
    // Find the greatest dice roll for each player.
    int attacker_index = findMax(attacker_dice, attacking_units);
    int defender_index = findMax(defender_dice, defending_units);
    // The defender wins in case of tie.
    if (attacker_dice[attacker_index] > defender_dice[defender_index]) {
      defender_loss += 1;
    }
    else { 
      attacker_loss += 1;
    }
    // Discard the two highest dice for the next comparison.
    attacker_dice[attacker_index] = 0;
    defender_dice[defender_index] = 0;
    count += 1;
  }

  delete[] defender_dice;
  delete[] attacker_dice;

  return std::make_tuple(attacker_loss, defender_loss);
}

int PlanningAgent::rollDie() {
  return (rand() % 6) + 1;
}

int PlanningAgent::getRandomInt(int from, int to) {
  return (rand() % (to - from + 1)) + from;
}

int PlanningAgent::findMax(int *arr, int size) {
  int max = arr[0];
  int max_index = 0;
  for (int i = 1; i < size; i++) {
    if (arr[i] > max) {
      max = arr[i];
      max_index = i;
    }
  }
  return max_index;
}
