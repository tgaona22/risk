#ifndef MAP_H
#define MAP_H

#include <SFML/Graphics.hpp>
#include "territory.h"
#include <vector>
#include <queue>
#include <map>
#include <string>

// Forward declare Territory.
class Territory;

class Map : public sf::Drawable
{
private:
  // std::vector<Territory*> territories;
  std::map<std::string, Territory *> territories; // This maps territory names to pointers to Territory objects.
  std::map<std::string, std::vector<Territory *>> continents;
  std::map<std::string, int> continents_bonus; // reinforcement bonus for controlling continent.

  // UI/Display related members.
  sf::Vector2<int> position;                // The upper left hand corner of the map.
  sf::Vector2<int> size;                    // How long and wide the map is.
  std::vector<sf::Vertex> connecting_lines; // A vertex array of the lines between neighboring territories.
public:
  Map(const std::string &mapfile, sf::Vector2<int> pos, sf::Vector2<int> sz);
  ~Map();

  Territory *getTerritory(const std::string &name);
  const Territory *getTerritory(const std::string &name) const;
  std::map<std::string, Territory *> getTerritories() const; // Note: this is copying the map...

  bool areConnected(const Territory *, const Territory *) const;

  double getUnitAverage() const;

private:
  virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};

#endif
