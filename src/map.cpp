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

    continents_bonus[cont_name] = cont_bonus;

    std::vector<Territory *> c_ts;

    for (auto &t : cont["territories"])
    {
      Territory *territory = new Territory(t["name"], cont, t["x"], t["y"]);
      named_territories.emplace(t["name"], territory);
      territories.push_back(territory);
      c_ts.push_back(territory);
    }
    continents[cont_name] = c_ts;
  }

  // Initialize each territory's list of neighbors.
  for (auto &cont : map_data)
  {
    for (auto &t_json : cont["territories"])
    {
      for (auto &n_name : t_json["neighbors"])
      {
        Territory *t = named_territories.at(t_json["name"]);
        Territory *n = named_territories.at(n_name);
        t->addNeighbor(n);

        // Draw the lines between territories. Alaska/Kamchatka is a special case.
        if (t->getName().compare("Alaska") == 0 && n->getName().compare("Kamchatka") == 0)
        {
          connecting_lines.push_back(sf::Vertex(t->getPosition(), sf::Color(105, 105, 105)));
          connecting_lines.push_back(sf::Vertex(sf::Vector2f(0, 100), sf::Color(105, 105, 105)));
        }
        else if (t->getName().compare("Kamchatka") == 0 && n->getName().compare("Alaska") == 0)
        {
          connecting_lines.push_back(sf::Vertex(t->getPosition(), sf::Color(255, 0, 0)));
          connecting_lines.push_back(sf::Vertex(sf::Vector2f(size.x, 110), sf::Color(255, 0, 0)));
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
}

Map::~Map()
{
  for (auto iter = begin(named_territories); iter != end(named_territories); iter++)
  {
    delete iter->second;
  }
}

void Map::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  // Draw the lines connecting the neighboring territories.
  target.draw(&connecting_lines[0], connecting_lines.size(), sf::Lines);
  // Draw each territory.
  for (auto iter = begin(named_territories); iter != end(named_territories); iter++)
  {
    Territory *territory = iter->second;
    target.draw(*territory);
  }
}

const Territory *Map::getTerritory(const std::string &name) const
{
  auto iter = named_territories.find(name);
  if (iter == end(named_territories))
  {
    return nullptr;
  }
  return iter->second;
}

const std::vector<Territory *> &Map::getTerritories() const
{
  return territories;
}

const std::vector<Territory *> Map::getTerritories(int player_id) const
{
  std::vector<Territory *> ts;
  for (auto &t : territories)
  {
    if (t->getOccupierId() == player_id)
    {
      ts.push_back(t);
    }
  }
  return ts;
}

const std::map<std::string, Territory *> &Map::getNamedTerritories() const
{
  return named_territories;
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
  for (auto iter = begin(named_territories); iter != end(named_territories); iter++)
  {
    total = total + iter->second->getUnits();
  }
  return total / named_territories.size();
}

std::map<std::string, int> Map::continentOwners() const
{
  std::map<std::string, int> result;
  for (const auto &c : continents)
  {
    std::vector<Territory *> c_ts = c.second;
    int pid = c_ts.at(0)->getOccupierId();
    bool owns_all = true;
    for (const auto t : c_ts)
    {
      if (t->getOccupierId() != pid)
      {
        owns_all = false;
        break;
      }
    }

    result[c.first] = (owns_all) ? pid : -1;
  }
  return result;
}

std::vector<std::string> Map::getContinents(int player_id) const
{
  std::vector<std::string> p_cs;
  for (const auto &c : continents)
  {
    std::vector<Territory *> ts = c.second;
    bool owns_c = true;
    for (const auto t : ts)
    {
      if (t->getOccupierId() != player_id)
      {
        owns_c = false;
        break;
      }
    }
    if (owns_c)
    {
      p_cs.push_back(c.first);
    }
  }
  return p_cs;
}