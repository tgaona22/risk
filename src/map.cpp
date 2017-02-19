#include "map.h"
#include <fstream>
#include <sstream>

#include <iostream>

Map::Map(const std::string& mapfile) {
  initTerritories(mapfile);
  initNeighbors(mapfile);
}

Map::~Map() {
  for (auto territory : territories) {
    delete territory;
  }
}

void Map::initTerritories(const std::string& mapfile) {
  std::ifstream in(mapfile);
  std::string line;
  // The id corresponds to the territory's location in the territories vector.
  int id = 0;
  while (std::getline(in, line)) {
    std::istringstream line_stream(line);
    // Read the territory name, assign it an id, and add the (name, id) pair to name_map.
    std::string territory_name;
    line_stream >> territory_name;
    name_map.insert(std::pair<std::string, int>(territory_name, id++));
    // Read in the x and y coordinates of the territory's position.
    std::string coordinate;
    line_stream >> coordinate;
    int x = std::stoi(coordinate);
    line_stream >> coordinate;
    int y = std::stoi(coordinate);
    // Create the territory and add it to the territories vector.
    Territory *territory = new Territory(territory_name, id, x, y);
    territories.push_back(territory);
    // Note: we can only add the neighbors once all territories have been created.
  }
  // Close the input stream.
  in.close();
}

void Map::initNeighbors(const std::string& mapfile) {
  // This function assumes that initTerritories has been called first.
  std::ifstream in(mapfile);
  std::string line;
  
  int territory_index = 0;
  while (std::getline(in, line)) {
    std::istringstream line_stream(line);
    std::string neighbor;
    // We can skip the name and the coordinates.
    line_stream >> neighbor >> neighbor >> neighbor;
    // Now read the neighbors and add them to the current territory's neighbor list.
    Territory *current_territory = territories.at(territory_index);
    while (line_stream >> neighbor) {
      Territory *neighbor_territory = getTerritory(neighbor);
      current_territory->addNeighbor(neighbor_territory);

      // Add a line connecting the two territories.
      connecting_lines.push_back(sf::Vertex(current_territory->getPosition(), sf::Color(105,105,105)));
      connecting_lines.push_back(sf::Vertex(neighbor_territory->getPosition(), sf::Color(105,105,105)));
    }
    // Move on to the next territory.
    territory_index = territory_index + 1;
  }
  // Close the input stream.
  in.close();
}

void Map::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  // Draw the lines connecting the neighboring territories.
  target.draw(&connecting_lines[0], connecting_lines.size(), sf::Lines);
  // Draw each territory.
  for (auto iter = begin(territories); iter != end(territories); iter++) {
    target.draw(**iter);
  }
}

Territory* Map::getTerritory(const std::string& name) {
  auto iter = name_map.find(name);
  if (iter == end(name_map)) {
    return nullptr;
  }
  return territories.at(iter->second);
}
    
    
  
