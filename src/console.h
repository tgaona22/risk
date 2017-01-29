#ifndef CONSOLE_H
#define CONSOLE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Console : public sf::Drawable {
private:
  std::vector<sf::Text*> log;
  int log_offset;
  sf::Text cmdline;
  std::string cmdstring;
  int max_cmd_length;
  int log_length;

  sf::Font font;
  int font_size;
  int command_position;
  int log_position;
  sf::Vector2<int> position;
  sf::Vector2<int> size;
public:
  // Constructor and destructor
  Console(int log_length, sf::Vector2<int> window_size);
  ~Console();

  // Interface for use by the main event-handling loop.
  void readText(int character);
  void deleteText();
  bool enterCommand();
  void scroll(bool up);
private:
  /* Draw must be implemented for the class to be Drawable. Will allow Console
   * instances to be drawn by the render window. */
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
  void updateLogPositions();
  
  
};

#endif
