#ifndef CONSOLE_H
#define CONSOLE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Console : public sf::Drawable {
private:
  std::vector<sf::Text*> log;
  int log_offset, log_length, log_position;
  sf::Text cmdline;
  std::string cmdstring;
  int max_cmd_length, cmd_position;

  sf::Font font;
  int font_size;
  sf::Vector2<int> position;
  sf::Vector2<int> size;

public:
  // Constructor and destructor
  Console(int log_length, const sf::Vector2<int>& window_size);
  ~Console();

  // Interface for use by the main event-handling loop.
  void readText(int character);
  void deleteText();
  bool enterCommand();
  void scroll(bool up);

private:
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
  void updateLogPositions();
  void initCmdLine(const sf::Vector2<int>& window_size);
  void initLog(int length, const sf::Vector2<int>& window_size);    
};

#endif
