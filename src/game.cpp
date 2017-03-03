#include "game.h"

//const int Game::initial_army_size[] = {0, 0, 40, 35, 30, 25, 20};
const int Game::initial_army_size[] = {0, 0, 10, 10, 10, 10};

Game::Game(sf::Vector2<int> screen_size, const std::string& map_file) :
  console(12, screen_size),
  map(map_file, sf::Vector2<int>(0,0), sf::Vector2<int>(screen_size.y - console.getSize().y, screen_size.x)),
  first_turn(true)
{  
  //players.push_back(new HumanAgent(map, console, 0, sf::Color::Blue));
  players.push_back(new RandomAgent(map, 0, sf::Color::Cyan));
  players.push_back(new RandomAgent(map, 1, sf::Color::Magenta));
  players.push_back(new RandomAgent(map, 2, sf::Color::Red));
}

Game::~Game() {
  for (auto player : players) {
    delete player;
  }
}

bool Game::run() {
  // The game starts with players claiming territories until all are taken.
  claimTerritories();
  // Each player distributes their remaining units throughout their owned territories.
  for (auto iter = begin(players); iter != end(players); iter++) {
    assignReinforcements(*iter);
  }
  first_turn = false;
  // Now each player takes their 3 step turn until the game is over.
  auto iter = begin(players);
  while (!isOver()) {
    // Player can only take their turn if they haven't lost - ie they still control territories.
    IAgent *player = *iter;
    if (player->getNumberOfTerritories() > 0) {
      takeTurn(*iter);
    }
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
    std::this_thread::sleep_for(std::chrono::seconds(1));
    iter++;
    if (iter == end(players)) {
      iter = begin(players);
    }
  }
}

void Game::takeTurn(IAgent *player) {
  // Player assigns reinforcements.
  assignReinforcements(player);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Player attacks.
  Territory *to, *from;
  int attacking_units;
  do {
    // When the player passes, they return nullptr for to and from.
    std::tie(to, from, attacking_units) = askAgentToAttack(player);
    if (to != nullptr) {
      IAgent *defender = players.at(to->getOccupierId());
      int defending_units = askAgentToDefend(defender, to, from, attacking_units);
      console.inform("Player " + std::to_string(player->getId()) + " attacks " + to->getName() 
		     + " from " + from->getName() + " with " + std::to_string(attacking_units) 
		     + " units. Player " + std::to_string(defender->getId()) + " defends with "
		     + std::to_string(defending_units) + " units.");
      resolveBattle(from, to, attacking_units, defending_units);
    }
  } while (to != nullptr); 

  std::this_thread::sleep_for(std::chrono::seconds(1));
  
  // Player fortifies.
  int fortifying_units;
  std::tie(to, from, fortifying_units) = askAgentToFortify(player);
  if (to != nullptr) {
    from->reinforce(-fortifying_units);
    to->reinforce(fortifying_units);
    console.inform("Player " + std::to_string(player->getId()) + " moves " + std::to_string(fortifying_units) 
		   + " units from " + from->getName() + " to " + to->getName() + ".");
  }

  std::this_thread::sleep_for(std::chrono::seconds(1));
}

void Game::resolveBattle(Territory *attacker, Territory *defender, int attacking_units, int defending_units) {
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
    console.inform("Player " + std::to_string(attacker->getOccupierId()) + "'s best dice roll is "
		   + std::to_string(attacker_dice[attacker_index]) + " and Player " + std::to_string(defender->getOccupierId()) 
		   + "'s best roll is " + std::to_string(defender_dice[defender_index]));
    // The defender wins in case of tie.
    if (attacker_dice[attacker_index] > defender_dice[defender_index]) {
      console.inform("Player " + std::to_string(attacker->getOccupierId()) + " wins the battle.");
      defender_loss += 1;
    }
    else { 
      console.inform("Player " + std::to_string(defender->getOccupierId()) + " wins the battle.");
      attacker_loss += 1;
    }
    // Discard the two highest dice for the next comparison.
    attacker_dice[attacker_index] = 0;
    defender_dice[defender_index] = 0;
    count += 1;
  }

  delete[] defender_dice;
  delete[] attacker_dice;
  
  // Update the units in the territories.
  defender->reinforce(-defender_loss);
  attacker->reinforce(-attacker_loss);

  // If the defender lost all their units, the attacker captures the territory.
  if (defender->getUnits() == 0) {
    console.inform("Player " + std::to_string(attacker->getOccupierId()) + " captures " + defender->getName() 
		   + " from Player " + std::to_string(defender->getOccupierId()) + "!");
    IAgent *attacking_agent = players.at(attacker->getOccupierId());
    int capturing_units = askAgentToCapture(attacking_agent, attacker, defender, attacking_units - attacker_loss);
    assignTerritoryToAgent(defender, attacking_agent, capturing_units);
    attacker->reinforce(-capturing_units);
  }
}

void Game::assignReinforcements(IAgent *player) {
  int total_reinforcements = getNumberOfReinforcements(player);
  Territory *territory;
  int reinforcements;

  while (total_reinforcements > 0) {
    std::tie(territory, reinforcements) = askAgentToReinforce(player, total_reinforcements);
    territory->reinforce(reinforcements);
    total_reinforcements = total_reinforcements - reinforcements;
    console.inform("Player " + std::to_string(player->getId()) + " reinforces " + territory->getName() 
		   + " with " + std::to_string(reinforcements) + " units.");
  }  
}

int Game::getNumberOfReinforcements(IAgent *player) {
  int reinforcements = 0;
  int player_territories = player->getNumberOfTerritories();
  // On the first turn, each player gets extra units.
  if (first_turn) {
    reinforcements = initial_army_size[players.size()] - player_territories;
  }
  else if (player_territories <= 9) { 
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

std::tuple<Territory*, Territory*, int> Game::askAgentToAttack(IAgent *agent) {
  const Territory *to, *from;
  int attacking_units;
  std::tie(to, from, attacking_units) = agent->attack();
  // If agent returns nullptr for to, then they are passing their turn.
  if (to == nullptr) {
    return std::make_tuple(nullptr, nullptr, 0);
  }
  /* Agent must return an enemy territory and one of their own territories adjacent to it.
     The attacker can roll 1, 2, or 3 dice but must have at least one more unit in the attacking
     territory than the number of dice rolled. */
  while (agent->hasTerritory(to) || !agent->hasTerritory(from) || !from->hasNeighbor(to) ||
	 attacking_units < 1 || attacking_units > 3 || from->getUnits() <= attacking_units || from->getUnits() <= 1) 
    {
      std::tie(to, from, attacking_units) = agent->attack();
      if (to == nullptr) { 
	return std::make_tuple(nullptr, nullptr, 0);
      }
    }
  return std::make_tuple(const_cast<Territory*>(to), const_cast<Territory*>(from), attacking_units);
}

int Game::askAgentToDefend(IAgent *agent, Territory *defender, Territory *attacker, int attacking_units) { 
  /* Agent may roll either 1 or 2 dice, but must have at least as many units 
     in the territory as the number of dice being rolled. */
  int defending_units = agent->defend(attacker, defender, attacking_units);
  while (defending_units < 1 || defending_units > 2 || defender->getUnits() < defending_units) {
    defending_units = agent->defend(attacker, defender, attacking_units);
  }
  return defending_units;
}

int Game::askAgentToCapture(IAgent *agent, Territory *from, Territory *to, int attacking_units) {
  /* Agent must return a number that is at least as big as the number of attacking units but
     strictly less than the number of units in the territory the units are being moved from. */
  int capturing_units = agent->capture(from, to, attacking_units);
  while (capturing_units < attacking_units || capturing_units >= from->getUnits()) {
    capturing_units = agent->capture(from, to, attacking_units);
  }
  return capturing_units;
}

std::tuple<Territory*, Territory*, int> Game::askAgentToFortify(IAgent *agent) {
  /* Agent must return a territory they own, another territory they own, and a number 
     that is strictly less than the number of units in the territory being moved from.
     If the agent does not wish to fortify, it should return nullptr for the territories. */
  // TODO: The rules state there should be a path between the two territories. Need to implement this functionality in Map class.
  const Territory *to, *from;
  int fortifying_units;
  std::tie(to, from, fortifying_units) = agent->fortify();
  if (to == nullptr) {
    return std::make_tuple(nullptr, nullptr, 0);
  }
  while (!agent->hasTerritory(to) || !agent->hasTerritory(from) || from->getUnits() <= fortifying_units) {
    std::tie(to, from, fortifying_units) = agent->fortify();
    if (to == nullptr) {
      return std::make_tuple(nullptr, nullptr, 0);
    }
  }
  return std::make_tuple(const_cast<Territory*>(to), const_cast<Territory*>(from), fortifying_units);
}

void Game::assignTerritoryToAgent(Territory *territory, IAgent *agent, int units) {
  if (territory->isOccupied()) {
    int old_id = territory->getOccupierId();
    players.at(old_id)->removeTerritory(territory);
  }
  territory->setOccupier(agent, units);
  agent->addTerritory(territory);
}
  
int Game::rollDie() {
  return (rand() % 6) + 1;
}

int Game::findMax(int *arr, int size) {
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
