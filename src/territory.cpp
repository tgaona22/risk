#include "territory.h"
// #include <nlohmann/json.hpp>

Territory::Territory(int id,
                     const std::string &name,
                     const nlohmann::json &continent_json,
                     int pos_x,
                     int pos_y) : name(name),
                                  continent(continent_json["name"].get<std::string>()),
                                  player_id(-1),
                                  id(id),
                                  units(0),
                                  radius(15),
                                  position(pos_x, pos_y),
                                  name_position(pos_x - radius, pos_y + radius)
{
  std::string root_dir = "C:\\Users\\TylerGaona\\Documents\\Projects\\tag2\\risk\\";
  font.loadFromFile(root_dir + "data/DroidSans.ttf");
  name_text.setFont(font);
  name_text.setString(name);
  name_text.setCharacterSize(8);
  name_text.setPosition(name_position);
  std::vector<int> name_color = continent_json["color"].get<std::vector<int>>();
  name_text.setFillColor(sf::Color(name_color[0], name_color[1], name_color[2], name_color[3]));
  units_text.setFont(font);
  units_text.setString(std::to_string(units));
  units_text.setCharacterSize(8);
  units_text.setPosition(position.x - 2, position.y - 4);
  units_text.setFillColor(sf::Color::Black);
  sprite.setPosition(position.x - radius, position.y - radius);
  sprite.setRadius(radius);
  sprite.setPointCount(80);
}

void Territory::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  target.draw(sprite);
  target.draw(name_text);
  target.draw(units_text);
}

void Territory::addNeighbor(Territory *territory)
{
  neighbors.push_back(territory);
}

sf::Vector2f Territory::getPosition() const
{
  return position;
}

const std::string &Territory::getName() const
{
  return name;
}

const std::vector<Territory *> &Territory::getNeighbors() const
{
  return neighbors;
}

int Territory::getOccupierId() const
{
  return player_id;
}

int Territory::getUnits() const
{
  return units;
}

bool Territory::isOccupied() const
{
  return player_id != -1;
}

bool Territory::hasNeighbor(const Territory *territory) const
{
  for (auto iter = begin(neighbors); iter != end(neighbors); iter++)
  {
    if (territory == *iter)
    {
      return true;
    }
  }
  return false;
}

void Territory::setOccupier(IAgent *agent, int units)
{
  player_id = agent->getId();
  sprite.setFillColor(agent->getColor());
  updateUnits(units);
}

void Territory::reinforce(int reinforcements)
{
  updateUnits(units + reinforcements);
}

void Territory::updateUnits(int new_units)
{
  units = new_units;
  units_text.setString(std::to_string(units));
}
