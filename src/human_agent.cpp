#include "human_agent.h"

HumanAgent::HumanAgent(const Map& map, Console& console, int id, std::string name, sf::Color color) :
  IAgent(map, id, name, color),
  console(console)
{}

HumanAgent::~HumanAgent() {}

const Territory* HumanAgent::selectUnoccupiedTerritory(const std::map<std::string, Territory*>& unoccupied_territories) {
  std::string territory_name;
  territory_name = console.prompt("Choose an unoccupied territory:");
  const Territory *territory = map.getTerritory(territory_name);
  while (territory == nullptr || territory->isOccupied()) {
    std::string msg = (territory == nullptr)
      ? territory_name + " is not a territory. Choose an unoccupied territory:" 
      : territory_name + " is already occupied. Choose an unoccupied territory:";
    territory_name = console.prompt(msg);
    territory = map.getTerritory(territory_name);
  }
  return territory;
}

std::tuple<const Territory*, int> HumanAgent::reinforce(int total_reinforcements) {
  std::string territory_name;
  std::string prompt_msg = "You have " + std::to_string(total_reinforcements) + " reinforcements available."
    + " What territory would you like to reinforce?";
  territory_name = console.prompt(prompt_msg);
  const Territory *territory = map.getTerritory(territory_name);
  while (territory == nullptr || !hasTerritory(territory)) {
    std::string err_msg = (territory == nullptr)
      ? territory_name + " is not a territory. What territory would you like to reinforce?" 
      : "You do not own " + territory_name + "! What territory would you like to reinforce?";
    territory_name = console.prompt(err_msg);
    territory = map.getTerritory(territory_name);
  }
  
  // NOTE: Eventually I should add exception handling here!
  prompt_msg = "You have " + std::to_string(total_reinforcements) + 
    " reinforcements available. How many would you like to place in " + territory->getName() + "?";
  int reinforcements = std::stoi(console.prompt(prompt_msg));
  while (reinforcements < 0 || reinforcements > total_reinforcements) {
    std::string err_msg = (reinforcements <= 0) 
      ? "Enter a positive number of reinforcements!"
      : "You don't have " + std::to_string(reinforcements) + " reinforcements!";
    reinforcements = std::stoi(console.prompt(err_msg + " " + prompt_msg));
  }

  return std::make_tuple(territory, reinforcements);
}

std::tuple<const Territory*, const Territory*, int> HumanAgent::attack() {
  std::string to_name, from_name;
  std::string prompt_msg = "Select a territory to attack. (Type 'pass' to stop attacking).";
  to_name = console.prompt(prompt_msg);

  if (to_name.compare("pass") == 0) {
    return std::make_tuple(nullptr, nullptr, 0);
  }

  const Territory *to = map.getTerritory(to_name);
  while (to == nullptr || hasTerritory(to) || !canAttack(to)) {
    std::string err_msg;
    if (to == nullptr) {
      err_msg = to_name + " is not a territory!";
    }
    else if (hasTerritory(to)) {
      err_msg = "You cannot attack your own territories!";
    }
    else if (!canAttack(to)) {
      err_msg = to_name + " is not adjacent to one of your territories!";
    }
    to_name = console.prompt(err_msg + " " + prompt_msg);
    to = map.getTerritory(to_name);
  }

  prompt_msg = "Where are you attacking from?";
  from_name = console.prompt(prompt_msg);
  const Territory *from = map.getTerritory(from_name);
  while (from == nullptr || !hasTerritory(from) || !from->hasNeighbor(to) || from->getUnits() == 1) {
    std::string err_msg;
    if (from == nullptr) {
      err_msg = from_name + " is not a territory!";
    }
    else if (!hasTerritory(from)) {
      err_msg = "You do not own " + from_name + "!";
    }
    else if (!from->hasNeighbor(to)) {
      err_msg = from_name + " is not adjacent to " + to_name + "!";
    }
    else if (from->getUnits() == 1) {
      err_msg = "You cannot attack from a territory that only has one unit!";
    }
    from_name = console.prompt(err_msg + " " + prompt_msg);
    from = map.getTerritory(from_name);
  }

  int attacking_units;
  prompt_msg = "How many units are you attacking with?";
  // NOTE: need exception handling here as well.
  attacking_units = std::stoi(console.prompt(prompt_msg));
  while (attacking_units < 1 || attacking_units > 3 || from->getUnits() <= attacking_units) {
    std::string err_msg;
    if (attacking_units < 1 || attacking_units > 3) {
      err_msg = "You can attack with 1, 2, or 3 units.";
    }
    else if (from->getUnits() <= attacking_units) {
      err_msg = "You need " + std::to_string(attacking_units + 1) + " units in " + from_name 
	+ " but there are only " + std::to_string(from->getUnits()) + "!";
    }
    attacking_units = std::stoi(console.prompt(err_msg + " " + prompt_msg));
  }

  return std::make_tuple(to, from, attacking_units);
}

int HumanAgent::defend(const Territory *attacker, const Territory *defender, int attacking_units) {
  std::string prompt_msg = defender->getName() + " is being attacked from " + attacker->getName() + " by "
    + std::to_string(attacking_units) + " units! How many units will defend?";
  int defending_units = std::stoi(console.prompt(prompt_msg));
  while (defending_units < 1 || defending_units > 2 || defender->getUnits() < defending_units) {
    std::string err_msg;
    if (defending_units < 1 || defending_units > 2) {
      err_msg = "You can defend with 1 or 2 units.";
    }
    else if (defender->getUnits() < defending_units) {
      err_msg = "You cannot defend with more units than there in the territory!";
    }
    defending_units = std::stoi(console.prompt(err_msg + " " + prompt_msg));
  }
  return defending_units;
}

int HumanAgent::capture(const Territory *from, const Territory *to_capture, int attacking_units) {
  std::string prompt_msg = "You have captured " + to_capture->getName() + "! How many units will you move in?";
  int capturing_units = std::stoi(console.prompt(prompt_msg));
  while (capturing_units < attacking_units || capturing_units >= from->getUnits()) {
    std::string err_msg;
    if (capturing_units < attacking_units) {
      err_msg = "You must move at least " + std::to_string(attacking_units) + " units!";
    }
    else {
      err_msg = "You cannot move more units than there are in " + from->getName() + "!";
    }
    capturing_units = std::stoi(console.prompt(err_msg + " " + prompt_msg));
  }
  return capturing_units;
}

std::tuple<const Territory*, const Territory*, int> HumanAgent::fortify() {
  std::string prompt_msg = "What territory do you want to fortify? (Type 'pass' to skip this phase)";

  std::string to_name = console.prompt(prompt_msg);

  if (to_name.compare("pass") == 0) {
    return std::make_tuple(nullptr, nullptr, 0);
  }

  const Territory *to = map.getTerritory(to_name);
  while (to == nullptr || !hasTerritory(to)) {
    std::string err_msg;
    if (to == nullptr) {
      err_msg = to_name + " is not a territory!";
    }
    else {
      err_msg = "You don't own " + to_name + "!";
    }
    to_name = console.prompt(err_msg + " " + prompt_msg);
    to = map.getTerritory(to_name);
  }

  prompt_msg = "What territory are you fortifying from?";
  std::string from_name = console.prompt(prompt_msg);
  const Territory *from = map.getTerritory(from_name);
  while (from == nullptr || !hasTerritory(from) || from->getUnits() == 1) {
    std::string err_msg;
    if (from == nullptr) {
      err_msg = from_name + " is not a territory!";
    }
    else if (!hasTerritory(from)) {
      err_msg = "You don't own " + from_name + "!";
    }
    else {
      err_msg = "You can't move units from a territory that only has one unit!";
    }
    from_name = console.prompt(err_msg + " " + prompt_msg);
    from = map.getTerritory(from_name);
  }

  prompt_msg = "How many units will you move?";
  int fortifying_units = std::stoi(console.prompt(prompt_msg));
  while (fortifying_units < 1 || fortifying_units >= from->getUnits()) {
    std::string err_msg;
    if (fortifying_units < 1) {
      err_msg = "You must move at least one unit!";
    }
    else {
      err_msg = from_name + " only has " + std::to_string(from->getUnits()) + " units!";
    }
    fortifying_units = std::stoi(console.prompt(err_msg + " " + prompt_msg));
  }

  return std::make_tuple(to, from, fortifying_units);
}
