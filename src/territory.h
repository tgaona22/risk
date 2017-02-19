#ifndef TERRITORY_H
#define TERRITORY_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Territory : public sf::Drawable {
private:
  // Game related data members.
  const std::string name;
  const int territory_id; // A unique identifier for the territory.
  int player_id; // The id # of the player occupying the territory.
  int units; // The # of units in the territory.
  std::vector<Territory*> neighbors; // A list of all neighboring territories.
  
  // Display/UI related data members.
  sf::CircleShape sprite;
  int radius;
  const sf::Vector2f position;
  sf::Font font;
  sf::Text text;
  const sf::Vector2f text_position;
  
public:
  // Initialization related functions
  Territory(const std::string& name, int id, int pos_x, int pos_y);
  void addNeighbor(Territory *territory);

  sf::Vector2f getPosition();
  const std::string& getName();

private:
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

#endif
