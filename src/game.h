#ifndef GAME_H
#define GAME_H

#include "map.h"
#include "territory.h"
#include "console.h"
#include "i_agent.h"
#include "human_agent.h"
#include "random_agent.h"

#include <vector>
#include <tuple>

class Game {
private:
  std::vector<IAgent*> players;
  Console console;
  Map map;
  
public:
  Game(sf::Vector2<int> screen_size, const std::string& map_file);
  ~Game();

  bool run();
  bool isOver();

  Console& getConsole() { return console; }
  Map& getMap() { return map; }
private:
  void claimTerritories();
  void takeTurn(IAgent *player);
  Territory* askAgentToChooseTerritory(IAgent *agent, const std::map<std::string, Territory*>& unoccupied_territories);
  std::tuple<Territory*, int> askAgentToReinforce(IAgent *agent, int total_reinforcements);
  void assignTerritoryToAgent(Territory *territory, IAgent *agent, int units);
  int getNumberOfReinforcements(IAgent *player);
};

#endif
