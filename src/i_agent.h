#ifndef IAGENT_H
#define IAGENT_H

#include "map.h"
#include "territory.h"

#include <tuple>
#include <string>

class Map;
class Territory;

class IAgent {
protected:
  const Map& map;
  std::vector<const Territory*> territories;
  int id;
  std::string name;
  sf::Color color;
public:
  IAgent(const Map& map, int id, std::string name, sf::Color color) : map(map), id(id), name(name), color(color) {}
  virtual ~IAgent() {}

  int getId() const;
  const std::string& getName() const;
  int getNumberOfTerritories() const;
  bool hasTerritory(const Territory *territory) const;
  bool canAttack(const Territory *territory) const;
  void addTerritory(const Territory *territory);
  void removeTerritory(const Territory *territory);
  const sf::Color& getColor() const;

  virtual const Territory* selectUnoccupiedTerritory(const std::map<std::string, Territory*>& unoccupied_territories) = 0;
  virtual std::tuple<const Territory*, int> reinforce(int total_reinforcements) = 0;
  virtual std::tuple<const Territory*, const Territory*, int> attack() = 0;
  virtual int defend(const Territory *attacker, const Territory *defender, int attacking_units) = 0;
  virtual int capture(const Territory *from, const Territory *to_capture, int attacking_units) = 0;
  virtual std::tuple<const Territory*, const Territory*, int> fortify() = 0;

  virtual void informOfBattleOutcome(int attacker_id, int defender_id, int attacker_lost, int defender_lost, bool captured)
  {}
protected:

};

#endif
