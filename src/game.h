#ifndef GAME_H
#define GAME_H

#include "map.h"
#include "territory.h"
#include "console.h"
#include "i_agent.h"
#include "human_agent.h"

#include <vector>

class Game {
private:
  std::vector<IAgent*> players;
  Console console;
  Map map;
  
public:
  Game(sf::Vector2<int> screen_size, const std::string& map_file);
  ~Game();

  bool start();

  void claimTerritories();
  //void takeTurn(IAgent *player);

  Console& getConsole() { return console; }
  Map& getMap() { return map; }
private:
  Territory* askAgentToChooseTerritory(IAgent *agent);
};

#endif
