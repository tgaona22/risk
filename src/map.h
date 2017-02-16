#ifndef MAP_H
#define MAP_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <string>

class Map : public sf::Drawable {
private:
  std::vector<Territory*> territories;
  std::map<const std::string&, int> name_map; // This maps territory names to their id #'s.
  
public:
  Map(const std::string& mapfile);
  ~Map();

  Territory* getTerritory(const std::string& name);

private:
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

  // Functions related to map initialization.
  void initTerritories(const std::string& mapfile);
  void initNeighbors(const std::string& mapfile);
};

#endif
