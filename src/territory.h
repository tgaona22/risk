#ifndef TERRITORY_H
#define TERRITORY_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

#include "i_agent.h"

// Forward declare IAgent.
class IAgent;

class Territory : public sf::Drawable {
private:
  // Game related data members.
  const std::string name;
  int player_id; // The id # of the player occupying the territory.
  int units; // The # of units in the territory.
  std::vector<Territory*> neighbors; // A list of all neighboring territories.
  
  // Display/UI related data members.
  sf::CircleShape sprite;
  int radius;
  const sf::Vector2f position;
  sf::Font font;
  sf::Text name_text;
  sf::Text units_text;
  const sf::Vector2f name_position;
  
public:
  // Initialization related functions
  Territory(const std::string& name, int pos_x, int pos_y);
  void addNeighbor(Territory *territory);

  sf::Vector2f getPosition() const;
  const std::string& getName() const;
  const std::vector<Territory*>& getNeighbors() const;
  int getUnits() const;
  bool isOccupied() const;
  int getOccupierId() const;
  bool hasNeighbor(const Territory *territory) const;

  void setOccupier(IAgent *occupier, int units);
  void reinforce(int reinforcements);

private:
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

  void updateUnits(int new_units);
};

#endif
