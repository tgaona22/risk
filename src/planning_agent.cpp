#include "planning_agent.h"

PlanningAgent::PlanningAgent(const Map& map, int id, sf::Color color) :
  IAgent(map, id, color), randy(map, id, color)
{}

PlanningAgent::~PlanningAgent() {}

const Territory* PlanningAgent::selectUnoccupiedTerritory(const std::map<std::string, Territory*>& unoccupied_territories) {
  // Prefer to choose territories that are adjacent to currently owned territories.
  const Territory *to_select;
  int max_adjacent = -1;
  for (auto iter = begin(unoccupied_territories); iter != end(unoccupied_territories); iter++) {
    const Territory *current_territory = iter->second;
    int count = neighborCount(current_territory);
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
  return randy.attack();
}

int PlanningAgent::defend(const Territory *attacker, const Territory *defender, int attacking_units) {
  return randy.defend(attacker, defender, attacking_units);
}

int PlanningAgent::capture(const Territory *from, const Territory *to_capture, int attacking_units) {
  return randy.capture(from, to_capture, attacking_units);
}

std::tuple<const Territory*, const Territory*, int> PlanningAgent::fortify() {
  return randy.fortify();
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

// Returns the number of friendly neighbors of the territory.
int PlanningAgent::neighborCount(const Territory *territory) {
  int count = 0;
  for (auto iter = begin(territories); iter != end(territories); iter++) {
    if (territory->hasNeighbor(*iter)) {
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
  return adjacent_enemies / ((double)territory->getUnits());
}

bool PlanningAgent::compareTerritories(std::tuple<const Territory*, double> first, std::tuple<const Territory*, double> second) {
  double first_value, second_value;
  std::tie(std::ignore, first_value) = first;
  std::tie(std::ignore, second_value) = second;
  return first_value > second_value;
}


  
