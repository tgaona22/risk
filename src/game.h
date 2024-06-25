#ifndef GAME_H
#define GAME_H

#include "map.h"
#include "territory.h"
#include "console.h"
#include "i_agent.h"
#include "human_agent.h"
#include "random_agent.h"
#include "planning_agent.h"

#include <vector>
#include <tuple>
#include <thread>
#include <chrono>
#include <algorithm>

class Game
{
private:
  std::vector<IAgent *> players;
  Console console;
  Map map;

  bool first_turn;
  const static int initial_army_size[];

public:
  Game(sf::Vector2<int> screen_size, const std::string &map_file);
  ~Game();

  bool run();
  bool isOver();

  Console &getConsole() { return console; }
  Map &getMap() { return map; }

private:
  void claimTerritories();
  void takeTurn(IAgent *player);

  Territory *askAgentToChooseTerritory(IAgent *agent,
                                       const std::vector<Territory *> &valid_territories,
                                       bool choosing);
  std::tuple<Territory *, int> askAgentToReinforce(IAgent *agent, int total_reinforcements);
  std::tuple<Territory *, Territory *, int> askAgentToAttack(IAgent *agent);
  int askAgentToDefend(IAgent *agent, Territory *defender, Territory *attacker, int attacking_units);
  int askAgentToCapture(IAgent *agent, Territory *from, Territory *to, int attacking_units);
  std::tuple<Territory *, Territory *, int> askAgentToFortify(IAgent *agent);

  void resolveBattle(Territory *attacker, Territory *defender, int attacking_units, int defending_units);
  int rollDie();
  int findMax(int *arr, int size);

  void assignTerritoryToAgent(Territory *territory, IAgent *agent, int units);
  int getNumberOfReinforcements(IAgent *player);
  void assignReinforcements(IAgent *player);
};

#endif
