#include "human_agent.h"

HumanAgent::HumanAgent(const Map& map, Console& console, int id) :
  IAgent(map, id, sf::Color::Blue),
  console(console)
{}

HumanAgent::~HumanAgent() {}

const Territory* HumanAgent::selectUnoccupiedTerritory() const {
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
    
  
  


