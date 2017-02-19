#include "territory.h"

Territory::Territory(const std::string& name, int id, int pos_x, int pos_y) :
  name(name), 
  territory_id(id),
  player_id(0),
  units(0),
  radius(20),
  position(pos_x, pos_y),
  text_position(pos_x - radius + 2, pos_y)
{
  font.loadFromFile("/usr/share/fonts/truetype/DroidSans.ttf");
  text.setFont(font);
  text.setString(name);
  text.setCharacterSize(8);
  text.setPosition(text_position);
  text.setColor(sf::Color::Red);
  sprite.setPosition(position.x - radius, position.y - radius);
  sprite.setRadius(radius);
  sprite.setPointCount(80);
}

void Territory::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  target.draw(sprite);
  target.draw(text);
}

void Territory::addNeighbor(Territory *territory) {
  neighbors.push_back(territory);
}

sf::Vector2f Territory::getPosition() {
  return position;
}

const std::string& Territory::getName() {
  return name;
}
  
