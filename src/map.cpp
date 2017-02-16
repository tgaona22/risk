#include "map.h"
#include <fstream>
#include <sstream>

Map::Map(const std::string& mapfile) {
  initTerritories(mapfile);
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
    Territory *territory = new Territory(name, id, x, y);
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
    line_stream << neighbor << neighbor << neighbor;
    // Now read the neighbors and add them to the current territory's neighbor list.
    while (line_stream << neighbor) {
      territories.at(territory_index)->addNeighbor(getTerritory(neighbor));
    }
  }
  // Close the input stream.
  in.close();
}

Territory* Map::getTerritory(const std::string& name) {
  auto iter = name_map.find(name);
  if (iter == end(map)) {
    return nullptr;
  }
  return territories.at(iter->second);
}
    
    
  
