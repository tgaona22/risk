#include "game.h"

Game::Game(sf::Vector2<int> screen_size, const std::string& map_file) :
  console(6, screen_size),
  map(map_file, sf::Vector2<int>(0,0), sf::Vector2<int>(screen_size.y - console.getSize().y, screen_size.x))
{
  players.push_back(new HumanAgent(map, console, 1));
  players.push_back(new RandomAgent(map, 2));
}

Game::~Game() {
  for (auto player : players) {
    delete player;
  }
}

bool Game::run() {
  // The game starts with players claiming territories until all are taken.
  claimTerritories();
  auto iter = begin(players);
  while (!isOver()) {
    takeTurn(*iter);
    // Move on to the next player.
    iter++;
    if (iter == end(players)) {
      iter = begin(players);
    }
  }
  return true;
}

bool Game::isOver() {
  return players.size() == 1;
}

void Game::claimTerritories() {
  // We're assuming that no funny business has gone on to assign players to territories before hand...
  std::map<std::string, Territory*> unoccupied_territories = map.getTerritories();

  auto iter = begin(players);
  // While there are still unoccupied territories...
  while (!unoccupied_territories.empty()) {
    IAgent *player = *iter;
    // Ask the player to choose a territory.
    Territory *territory = askAgentToChooseTerritory(player, unoccupied_territories);
    // Assign that territory to the player.
    assignTerritoryToAgent(territory, player, 1);
    // Remove the territory from the unoccupied list.
    unoccupied_territories.erase(territory->getName());
    // Move on to the next player.
    iter++;
    if (iter == end(players)) {
      iter = begin(players);
    }
  }
}

void Game::takeTurn(IAgent *player) {
  // Player assigns reinforcements.
  int total_reinforcements = getNumberOfReinforcements(player);
  Territory *territory;
  int reinforcements;
  while (total_reinforcements > 0) {
    std::tie(territory, reinforcements) = askAgentToReinforce(player, total_reinforcements);
    territory->reinforce(reinforcements);
    total_reinforcements = total_reinforcements - reinforcements;
  }

  // Player attacks.

  // Player maneuvers units.
}

int Game::getNumberOfReinforcements(IAgent *player) {
  int reinforcements;
  int player_territories = player->getNumberOfTerritories();
  if (player_territories <= 9) { 
    reinforcements = 3;
  }
  else {
    reinforcements = player_territories / 3;
  }
  return reinforcements;
}

std::tuple<Territory*, int> Game::askAgentToReinforce(IAgent *agent, int total_reinforcements) {
  const Territory *territory;
  int reinforcements;
  std::tie(territory, reinforcements) = agent->reinforce(total_reinforcements);
  /* Agent must return a territory they occupy and a positive number of units
     which does not exceed the remaining amount of reinforcements. */
  while (!agent->hasTerritory(territory) || reinforcements <= 0 || reinforcements > total_reinforcements) {
    std::tie(territory, reinforcements) = agent->reinforce(total_reinforcements);
  }
  return std::make_tuple(const_cast<Territory*>(territory), reinforcements);
}

Territory* Game::askAgentToChooseTerritory(IAgent *agent, const std::map<std::string, Territory*>& unoccupied_territories) {
  // Agent must return an unoccupied territory.
  Territory *territory = const_cast<Territory*>(agent->selectUnoccupiedTerritory(unoccupied_territories));
  while (territory->isOccupied()) {
    territory = const_cast<Territory*>(agent->selectUnoccupiedTerritory(unoccupied_territories));
  } 
  return territory;
}

void Game::assignTerritoryToAgent(Territory *territory, IAgent *agent, int units) {
  if (territory->isOccupied()) {
    int old_id = territory->getOccupierId();
    players.at(old_id)->removeTerritory(territory);
  }
  territory->setOccupier(agent, units);
  agent->addTerritory(territory);
}
  
