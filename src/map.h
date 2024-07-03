#ifndef MAP_H
#define MAP_H

#include <SFML/Graphics.hpp>
#include "territory.h"
#include "i_agent.h"
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
  std::map<std::string, Territory *> named_territories; // This maps territory names to pointers to Territory objects.
  std::vector<Territory *> territories;
  std::map<std::string, std::vector<Territory *>> continents;
  std::map<std::string, int> continents_bonus; // reinforcement bonus for controlling continent.

  // UI/Display related members.
  sf::Vector2<int> position;                // The upper left hand corner of the map.
  sf::Vector2<int> size;                    // How long and wide the map is.
  std::vector<sf::Vertex> connecting_lines; // A vertex array of the lines between neighboring territories.
  std::map<std::tuple<Territory *, Territory *>, int> line_map;
  int line_map_idx;
  int alaska_idx;
  Territory *attacker, *defender;

public:
  Map(const std::string &mapfile, sf::Vector2<int> pos, sf::Vector2<int> sz);
  ~Map();

  const Territory *getTerritory(const std::string &name) const;

  const std::map<std::string, Territory *> &getNamedTerritories() const;
  const std::vector<Territory *> &getTerritories() const;
  const std::vector<Territory *> getTerritories(int player_id) const;

  std::map<std::string, int> continentOwners() const;
  std::vector<std::string> getContinents(int player_id) const;
  const std::map<std::string, int> &getContinentsBonus() const { return continents_bonus; }

  bool areConnected(const Territory *, const Territory *) const;

  double getUnitAverage() const;

  std::tuple<Territory *, Territory *> pair(Territory *a, Territory *b) const;
  void highlight_line(Territory *a, Territory *b, const sf::Color &color);

private:
  virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};

#endif
