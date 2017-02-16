#ifndef TERRITORY_H
#define TERRITORY_H

#include <SFML/Graphics.hpp>

class Territory : public sf::Drawable {
private:
  // Game related data members.
  const std::string name;
  const int territory_id; // A unique identifier for the territory.
  int player_id; // The id # of the player occupying the territory.
  int units; // The # of units in the territory.
  std::vector<Territory*> neighbors // A list of all neighboring territories.
  
  // Display/UI related data members.
  sf::Sprite sprite;
  const sf::Vector2f position;
  sf::Text text;
  const sf::Vector2f text_position;
  
  static sf::Texture texture;
public:
  // Initialization related functions
  Territory(const std::string& name, int id, int pos_x, int pos_y);
  void addNeighbor(Territory *territory);

private:
};

#endif
