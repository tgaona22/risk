#ifndef CONSOLE_H
#define CONSOLE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <deque>
#include <string>

class Console : public sf::Drawable
{
private:
  // std::vector<sf::Text*> log;
  std::deque<sf::Text *> log;
  // std::deque<sf::Text*>::iterator log_offset;
  int log_offset; // the index of the first element to print in the log.
  // std::queue<std::string> commands;
  int log_length, log_position;
  int log_max;
  sf::Text cmdline;
  std::string cmdstring;
  int max_cmd_length, cmd_position;

  bool listen_to_input;
  bool command_entered;

  sf::Font font;
  int font_size;
  sf::Vector2<int> position; // the upper left hand corner of the console.
  sf::Vector2<int> size;     // the length and width of the console.

public:
  // Constructor and destructor
  Console(int log_length, const sf::Vector2<int> &window_size);
  ~Console();

  std::string prompt(const std::string &msg);
  void inform(const std::string &msg);

  // Interface for use by the main event-handling loop.
  void readText(int character);
  void deleteText();
  bool enterCommand();
  void scroll(bool up);

  const sf::Vector2<int> &getSize() const { return size; }

private:
  virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
  void updateLogPositions();
  void addToLog(const std::string &msg, sf::Color color);
  void initCmdLine(const sf::Vector2<int> &window_size);
  void initLog(int length, const sf::Vector2<int> &window_size);
};

#endif
