#include "map.h"
#include <fstream>
#include <sstream>

#include <iostream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

Map::Map(const std::string &mapfile, sf::Vector2<int> pos, sf::Vector2<int> sz) : position(pos),
                                                                                  size(sz)
{
  std::ifstream f(mapfile);
  json map_data = json::parse(f);

  // Construct the territories and add them to the map.
  for (auto &cont : map_data)
  {
    std::string cont_name = cont["name"];
    int cont_bonus = cont["bonus"];
    for (auto &t : cont["territories"])
    {
      Territory *territory = new Territory(t["name"], cont, t["x"], t["y"]);
      territories.emplace(t["name"], territory);
    }
  }

  // Initialize each territory's list of neighbors.
  for (auto &cont : map_data)
  {
    for (auto &t_json : cont["territories"])
    {
      for (auto &n_name : t_json["neighbors"])
      {
        Territory *t = getTerritory(t_json["name"]);
        Territory *n = getTerritory(n_name);
        t->addNeighbor(n);

        // Draw the lines between territories. Alaska/Kamchatka is a special case.
        if (t->getName().compare("Alaska") == 0 && n->getName().compare("Kamchatka") == 0)
        {
          connecting_lines.push_back(sf::Vertex(t->getPosition(), sf::Color(105, 105, 105)));
          connecting_lines.push_back(sf::Vertex(sf::Vector2f(0, 100), sf::Color(105, 105, 105)));
        }
        else if (t->getName().compare("Kamchatka") == 0 && n->getName().compare("Alaska") == 0)
        {
          connecting_lines.push_back(sf::Vertex(t->getPosition(), sf::Color(105, 105, 105)));
          connecting_lines.push_back(sf::Vertex(sf::Vector2f(size.x, 110), sf::Color(105, 105, 105)));
        }
        else
        {
          // Add a line connecting the two territories.
          connecting_lines.push_back(sf::Vertex(t->getPosition(), sf::Color(105, 105, 105)));
          connecting_lines.push_back(sf::Vertex(n->getPosition(), sf::Color(105, 105, 105)));
        }
        // If I want to highlight actions between territories, I'll need a more intelligent
        // way to store the lines.
      }
    }
  }

  // initTerritories(mapfile);
  // initNeighbors(mapfile);
}

Map::~Map()
{
  for (auto iter = begin(territories); iter != end(territories); iter++)
  {
    delete iter->second;
  }
}

/*
void Map::initTerritories(const std::string &mapfile)
{
  std::ifstream in(mapfile);
  std::string line;

  while (std::getline(in, line))
  {
    std::istringstream line_stream(line);
    // Read the territory name.
    std::string territory_name;
    line_stream >> territory_name;
    // Read in the x and y coordinates of the territory's position.
    std::string coordinate;
    line_stream >> coordinate;
    int x = std::stoi(coordinate);
    line_stream >> coordinate;
    int y = std::stoi(coordinate);
    // Create the territory and add it with its name as the key to the territories map.
    Territory *territory = new Territory(territory_name, x + position.x, y + position.y);
    territories.insert(std::pair<std::string, Territory *>(territory_name, territory));
    // Note: we can only add the neighbors once all territories have been created.
  }
  // Close the input stream.
  in.close();
}

void Map::initNeighbors(const std::string &mapfile)
{
  // This function assumes that initTerritories has been called first.
  std::ifstream in(mapfile);
  std::string line;

  while (std::getline(in, line))
  {
    std::istringstream line_stream(line);
    std::string name;
    // Read the territory name and get the territory.
    line_stream >> name;
    Territory *current_territory = getTerritory(name);
    // We can skip the two coordinates.
    line_stream >> name >> name;
    // Now read the neighbors and add them to the current territory's neighbor list.
    std::string neighbor;
    while (line_stream >> neighbor)
    {
      Territory *neighbor_territory = getTerritory(neighbor);
      current_territory->addNeighbor(neighbor_territory);

      // Special case for the Kamchatka Alaska connection...
      // std::cout << current_territory->getName();
      if (current_territory->getName().compare("Alaska") == 0 && neighbor_territory->getName().compare("Kamchatka") == 0)
      {
        connecting_lines.push_back(sf::Vertex(current_territory->getPosition(), sf::Color(105, 105, 105)));
        connecting_lines.push_back(sf::Vertex(sf::Vector2f(0, 100), sf::Color(105, 105, 105)));
      }
      else if (current_territory->getName().compare("Kamchatka") == 0 && neighbor_territory->getName().compare("Alaska") == 0)
      {
        connecting_lines.push_back(sf::Vertex(current_territory->getPosition(), sf::Color(105, 105, 105)));
        connecting_lines.push_back(sf::Vertex(sf::Vector2f(size.x, 110), sf::Color(105, 105, 105)));
      }
      else
      {
        // Add a line connecting the two territories.
        connecting_lines.push_back(sf::Vertex(current_territory->getPosition(), sf::Color(105, 105, 105)));
        connecting_lines.push_back(sf::Vertex(neighbor_territory->getPosition(), sf::Color(105, 105, 105)));
      }
    }
  }
  // Close the input stream.
  in.close();
}
*/

void Map::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  // Draw the lines connecting the neighboring territories.
  target.draw(&connecting_lines[0], connecting_lines.size(), sf::Lines);
  // Draw each territory.
  for (auto iter = begin(territories); iter != end(territories); iter++)
  {
    Territory *territory = iter->second;
    target.draw(*territory);
  }
}

// what the fuck?
Territory *Map::getTerritory(const std::string &name)
{
  return const_cast<Territory *>(static_cast<const Map &>(*this).getTerritory(name));
}

const Territory *Map::getTerritory(const std::string &name) const
{
  auto iter = territories.find(name);
  if (iter == end(territories))
  {
    return nullptr;
  }
  return iter->second;
}

std::map<std::string, Territory *> Map::getTerritories() const
{
  return territories;
}

/* Does a breadth-first-search to determine if there is a path of territories
   owned by the player who controls src and dest between src and dest. */
bool Map::areConnected(const Territory *src, const Territory *dest) const
{
  if (!src->isOccupied() || !dest->isOccupied())
  {
    return false;
  }

  std::queue<const Territory *> search;
  std::vector<const Territory *> searched;
  search.push(src);
  while (!search.empty())
  {
    const Territory *current = search.front();
    search.pop();
    if (current == dest)
    {
      return true;
    }

    const std::vector<Territory *> &neighbors = current->getNeighbors();
    for (auto iter = begin(neighbors); iter != end(neighbors); iter++)
    {
      if ((*iter)->getOccupierId() == src->getOccupierId() && std::find(begin(searched), end(searched), *iter) == end(searched))
      {
        search.push(*iter);
      }
    }
    searched.push_back(current);
  }
  return false;
}

double Map::getUnitAverage() const
{
  double total = 0;
  for (auto iter = begin(territories); iter != end(territories); iter++)
  {
    total = total + iter->second->getUnits();
  }
  return total / territories.size();
}
