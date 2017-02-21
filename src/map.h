#ifndef MAP_H
#define MAP_H

#include <SFML/Graphics.hpp>
#include "territory.h"
#include <vector>
#include <map>
#include <string>

class Map : public sf::Drawable {
private:
  std::vector<Territory*> territories;
  std::map<std::string, int> name_map; // This maps territory names to their id #'s.
  
  // UI/Display related members.
  sf::Vector2<int> position; // The upper left hand corner of the map.
  sf::Vector2<int> size; // How long and wide the map is.
  std::vector<sf::Vertex> connecting_lines; // A vertex array of the lines between neighboring territories.
public:
  Map(const std::string& mapfile, sf::Vector2<int> pos, sf::Vector2<int> sz);
  ~Map();

  Territory* getTerritory(const std::string& name);

private:
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

  // Functions related to map initialization.
  void initTerritories(const std::string& mapfile);
  void initNeighbors(const std::string& mapfile);
};

#endif
