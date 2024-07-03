#include "map.h"
#include <fstream>
#include <sstream>

#include <iostream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

Map::Map(const std::string &mapfile, sf::Vector2<int> pos, sf::Vector2<int> sz) : position(pos),
                                                                                  size(sz),
                                                                                  line_map_idx(-1)
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

    int t_id = 0;
    for (auto &t : cont["territories"])
    {
      Territory *territory = new Territory(t_id, t["name"], cont, t["x"], t["y"]);
      ++t_id;
      named_territories.emplace(t["name"], territory);
      territories.push_back(territory);
      c_ts.push_back(territory);
    }
    continents[cont_name] = c_ts;
  }

  // for purposes of higlighting, I need to keep a map from pairs of territories
  // to vertices. (or maybe just territories -> vertices?)

  // how about a function that takes two territories and returns the ordered pair.

  // Initialize each territory's list of neighbors.
  int vertex_counter = 0;
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
        Territory *first, *second;
        std::tie(first, second) = pair(t, n);
        // Note that id(Alaska) < id(Kamchatka).

        if (line_map.find(std::tie(first, second)) == end(line_map))
        {
          if (first->getName() == "Alaska" && second->getName() == "Kamchatka")
          {
            connecting_lines.push_back(sf::Vertex(first->getPosition(), sf::Color(105, 105, 105)));
            connecting_lines.push_back(sf::Vertex(sf::Vector2f(0, 100), sf::Color(105, 105, 105)));
            connecting_lines.push_back(sf::Vertex(second->getPosition(), sf::Color(105, 105, 105)));
            connecting_lines.push_back(sf::Vertex(sf::Vector2f(size.x, 110), sf::Color(105, 105, 105)));

            line_map[std::tie(first, second)] = vertex_counter;
            alaska_idx = vertex_counter;
            vertex_counter += 4;
          }
          else
          {
            connecting_lines.push_back(sf::Vertex(t->getPosition(), sf::Color(105, 105, 105)));
            connecting_lines.push_back(sf::Vertex(n->getPosition(), sf::Color(105, 105, 105)));

            line_map[std::tie(first, second)] = vertex_counter;
            vertex_counter += 2;
          }
        }
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

std::tuple<Territory *, Territory *> Map::pair(Territory *a, Territory *b) const
{
  Territory *first = (a->getId() < b->getId()) ? a : b;
  Territory *second = (a->getId() < b->getId()) ? b : a;
  return std::tie(first, second);
}

void Map::highlight_line(Territory *a, Territory *b)
{
  // unhighlight the previous line.
  if (line_map_idx != -1)
  {
    int lim = line_map_idx == alaska_idx ? 4 : 2;
    for (int j = 0; j < lim; ++j)
    {
      connecting_lines.at(line_map_idx + j).color = sf::Color(105, 105, 105);
    }
  }

  Territory *first, *second;
  std::tie(first, second) = pair(a, b);
  line_map_idx = line_map.at(std::tie(first, second));

  // Change the color of the appropriate lines if there is highlighting.
  int lim = line_map_idx == alaska_idx ? 4 : 2;
  for (int j = 0; j < lim; ++j)
  {
    connecting_lines.at(line_map_idx + j).color = sf::Color::Red;
  }
}