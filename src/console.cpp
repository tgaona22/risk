#include "console.h"

#include <iostream>

Console::Console(int log_length, sf::Vector2<int> window_size) {
  font_size = 16;
  font.loadFromFile("/usr/share/fonts/truetype/DroidSans.ttf");

  max_cmd_length = 50;
  this->log_length = log_length;

  size.x = window_size.x;
  size.y = font_size * (log_length + 1);
  position.x = 0;
  position.y = window_size.y - size.y;
  command_position = window_size.y - font_size;
  log_position = window_size.y - 2*font_size;
  log_offset = 0;

  cmdline.setFont(font);
  cmdline.setColor(sf::Color::Green);
  cmdline.setCharacterSize(font_size);
  cmdline.setPosition(0, command_position);
  cmdstring = "> ";
  cmdline.setString(cmdstring);
}

Console::~Console() {
  std::vector<sf::Text*>::iterator iter;
  for (iter = log.begin(); iter != log.end(); iter++) {
    delete *iter;
  }
}

void Console::readText(int character) {
  if (cmdstring.length() < max_cmd_length) {
    cmdstring += character;
    cmdline.setString(cmdstring);
  }
}

void Console::deleteText() {
  if (cmdstring.length() > 2) {
    cmdstring.erase(cmdstring.length() - 1, 1);
    cmdline.setString(cmdstring);
  }
}

bool Console::enterCommand() {
  std::string input = cmdstring.substr(2, std::string::npos);
  // Here would be a good place to parse the input.
  // For now, we'll just move it into the log and wipe the command line.
  // On entering a command, we want the log to go back to the most recent commands.
  log_offset = 0;
  log.push_back(new sf::Text(input, font, font_size));
  updateLogPositions();
  
  cmdstring = "> ";
  cmdline.setString(cmdstring);
  return true;
}

void Console::scroll(bool up) {
  if (up) {
    // log.size() must be cast to int because it returns an unsigned type.
    if (log_offset < (int)log.size() - log_length) {
      log_offset++;
    }
  }
  else {
    if (log_offset > 0) {
      log_offset--;
    }
  }
  updateLogPositions();
}

void Console::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  // Display the command line.
  target.draw(cmdline);
  // Display the most recent "log_length" log entries
  std::vector<sf::Text*>::const_iterator iter = (log.end()-1) - log_offset;
  for (int i = 0; i < log_length && i < log.size(); i++) {
    sf::Text *msg = *iter;
    target.draw(*msg);
    iter--;
  }
}

void Console::updateLogPositions() {
  std::vector<sf::Text*>::iterator iter = (log.end()-1) - log_offset;
  for (int i = 0; i < log_length && i < log.size(); i++) {
    (*iter)->setPosition(0, log_position - i*font_size);
    iter--;
  }
}

