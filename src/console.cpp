#include "console.h"

Console::Console(int log_length, const sf::Vector2<int>& window_size) {
  font_size = 16;
  font.loadFromFile("/usr/share/fonts/truetype/DroidSans.ttf");

  listen_to_input = false;

  // Size denotes the length and width of the console.
  size.x = window_size.x;
  size.y = font_size * (log_length + 1);
  // Position denotes the upper left hand corner of the console.
  position.x = 0;
  position.y = window_size.y - size.y;
  
  // Initialize the command line and the message log.
  initCmdLine(window_size);
  initLog(log_length, window_size);
}

// Initializes the command line.
void Console::initCmdLine(const sf::Vector2<int>& window_size) {
  max_cmd_length = 50;  
  cmd_position = window_size.y - font_size;
  cmdline.setFont(font);
  cmdline.setColor(sf::Color::Green);
  cmdline.setCharacterSize(font_size);
  cmdline.setPosition(0, cmd_position);
  cmdstring = "> ";
  cmdline.setString(cmdstring);
}

// Initializes the message log.
void Console::initLog(int length, const sf::Vector2<int>& window_size) {
  log_length = length;
  log_position = window_size.y - 2*font_size;
  log_offset = 0;
}

// Frees the memory allocated by log messages.
Console::~Console() {
  std::vector<sf::Text*>::iterator iter;
  for (iter = log.begin(); iter != log.end(); iter++) {
    delete *iter;
  }
}

std::string Console::prompt(const std::string& msg) {
  listen_to_input = true;
  addToLog(msg, sf::Color::Magenta);
  command_entered = false;
  // Block this thread until the user enters a command.
  while(!command_entered) { }
  listen_to_input = false;
  // Return the most recently entered command.
  std::string latest_command = log.at(log.size() - 1)->getString();
  return latest_command;
}

void Console::addToLog(const std::string& msg, sf::Color color = sf::Color::Green) {
  log_offset = 0;
  sf::Text *msg_text = new sf::Text(msg, font, font_size);
  msg_text->setColor(color);
  log.push_back(msg_text);
  updateLogPositions();
}

/* If the current command does not exceed the maximum length, adds
 * the character to the end of the command. */
void Console::readText(int character) {
  if (listen_to_input && cmdstring.length() < max_cmd_length) {
    cmdstring += character;
    cmdline.setString(cmdstring);
  }
}

// Deletes a character from the current command.
void Console::deleteText() {
  if (listen_to_input && cmdstring.length() > 2) {
    cmdstring.erase(cmdstring.length() - 1, 1);
    cmdline.setString(cmdstring);
  }
}

/* Accepts the current command, sends it to Game for parsing, and 
 * records it in the message log. */
bool Console::enterCommand() {
  if (listen_to_input) {
    std::string input = cmdstring.substr(2, std::string::npos);
    addToLog(input);
    command_entered = true;
  
    cmdstring = "> ";
    cmdline.setString(cmdstring);
  }
  return true;
}

/* Displays older messages in the log if *up* is true; displays newer
 * messages in the log if *up* is false. */
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

/* Allows the render window to draw the console. This method draws
 * the command line and a subset of the message log. */
void Console::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  // Display the command line.
  target.draw(cmdline);
  /* Display *log_length* number of messages, starting with the message 
   * that is *log_offset* number of messages from the end of the list. */
  std::vector<sf::Text*>::const_iterator iter = (log.end()-1) - log_offset;
  for (int i = 0; i < log_length && i < log.size(); i++) {
    sf::Text *msg = *iter;
    target.draw(*msg);
    iter--;
  }
}

/* Sets the positions of the *log_length* number of messages that are
 * to be drawn to the screen. */
void Console::updateLogPositions() {
  std::vector<sf::Text*>::iterator iter = (log.end()-1) - log_offset;
  for (int i = 0; i < log_length && i < log.size(); i++) {
    (*iter)->setPosition(0, log_position - i*font_size);
    iter--;
  }
}


