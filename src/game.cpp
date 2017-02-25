#include "game.h"

Game::Game(sf::Vector2<int> screen_size, const std::string& map_file) :
  console(6, screen_size),
  map(map_file, sf::Vector2<int>(0,0), sf::Vector2<int>(screen_size.y - console.getSize().y, screen_size.x))
{
  players.push_back(new HumanAgent(map, console, 1));
  //players.push_back(new RandomAgent(map, 2));
}

Game::~Game() {
  for (auto player : players) {
    delete player;
  }
}

bool Game::start() {
  claimTerritories();
  return true;
}

void Game::claimTerritories() {
  // We're assuming that no funny business has gone on to assign players to territories before hand...
  std::map<std::string, Territory*> unoccupied_territories = map.getTerritories();

  auto iter = begin(players);
  // While there are still unoccupied territories...
  while (!unoccupied_territories.empty()) {
    IAgent *player = *iter;
    // Ask the player to choose a territory.
    Territory *territory = askAgentToChooseTerritory(player);
    // Assign that territory to the player.
    territory->setOccupier(player, 1);
    // Remove the territory from the unoccupied list.
    unoccupied_territories.erase(territory->getName());
    // Move on to the next player.
    iter++;
    if (iter == end(players)) {
      iter = begin(players);
    }
  }
}

Territory* Game::askAgentToChooseTerritory(IAgent *agent) {
  // Agent must return an unoccupied territory.
  Territory *territory = const_cast<Territory*>(agent->selectUnoccupiedTerritory());
  while (territory->isOccupied()) {
    territory = const_cast<Territory*>(agent->selectUnoccupiedTerritory());
  } 
  return territory;
}
