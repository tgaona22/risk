#include "human_agent.h"

HumanAgent::HumanAgent(const Map& map, Console& console, int id) :
  IAgent(map, id, sf::Color::Blue),
  console(console)
{}

HumanAgent::~HumanAgent() {}

const Territory* HumanAgent::selectUnoccupiedTerritory(const std::map<std::string, Territory*>& unoccupied_territories) const {
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

std::tuple<const Territory*, int> HumanAgent::reinforce(int total_reinforcements) const {
  std::string territory_name;
  territory_name = console.prompt("What territory would you like to reinforce?");
  const Territory *territory = map.getTerritory(territory_name);
  while (territory == nullptr || !hasTerritory(territory)) {
    std::string msg = (territory == nullptr)
      ? territory_name + " is not a territory. What territory would you like to reinforce?" 
      : "You do not own " + territory_name + "! What territory would you like to reinforce?";
    territory_name = console.prompt(msg);
    territory = map.getTerritory(territory_name);
  }
  
  // NOTE: Eventually I should add exception handling here!
  std::string prompt_msg = "You have " + std::to_string(total_reinforcements) + 
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
