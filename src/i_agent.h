#ifndef IAGENT_H
#define IAGENT_H

#include "map.h"
#include "territory.h"

class Map;
class Territory;

class IAgent {
protected:
  const Map& map;
  int id;
  sf::Color color;
public:
  IAgent(const Map& map, int id, sf::Color color) : map(map), id(id), color(color) {}
  virtual ~IAgent() {}

  int getId() const { return id; }
  const sf::Color& getColor() const { return color; }

  virtual const Territory* selectUnoccupiedTerritory(const std::map<std::string, Territory*>& unoccupied_territories) const = 0;
  //virtual int reinforce(Territory *from, Territory *to) = 0;
  //virtual int attack(Territory *from, Territory *to) = 0;
  //virtual int maneuver(Territory *from, Territory *to) = 0;
  
};

#endif
