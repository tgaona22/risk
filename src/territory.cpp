#include "territory.h"

Territory::Territory(const std::string& name, int pos_x, int pos_y) :
  name(name), 
  player_id(-1),
  units(0),
  radius(15),
  position(pos_x, pos_y),
  name_position(pos_x - radius, pos_y + radius)
{
  font.loadFromFile("/usr/share/fonts/truetype/DroidSans.ttf");
  name_text.setFont(font);
  name_text.setString(name);
  name_text.setCharacterSize(8);
  name_text.setPosition(name_position);
  name_text.setColor(sf::Color::Red);
  units_text.setFont(font);
  units_text.setString(std::to_string(units));
  units_text.setCharacterSize(8);
  units_text.setPosition(position.x - 2, position.y - 4);
  units_text.setColor(sf::Color::Black);
  sprite.setPosition(position.x - radius, position.y - radius);
  sprite.setRadius(radius);
  sprite.setPointCount(80);
}

void Territory::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  target.draw(sprite);
  target.draw(name_text);
  target.draw(units_text);
}

void Territory::addNeighbor(Territory *territory) {
  neighbors.push_back(territory);
}

sf::Vector2f Territory::getPosition() const {
  return position;
}

const std::string& Territory::getName() const {
  return name;
}

bool Territory::isOccupied() const {
  return player_id != -1;
}

void Territory::setOccupier(IAgent *agent, int units) {
  player_id = agent->getId();
  sprite.setFillColor(agent->getColor());
  updateUnits(units);
}

void Territory::updateUnits(int new_units) {
  units = new_units;
  units_text.setString(std::to_string(units));
}
  
