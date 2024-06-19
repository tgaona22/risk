#include "console.h"

#include <iostream>

Console::Console(int log_length, const sf::Vector2<int> &window_size)
{
  font_size = 16;
  std::string root_dir = "C:\\Users\\TylerGaona\\Documents\\Projects\\tag2\\risk\\";
  font.loadFromFile(root_dir + "data/DroidSans.ttf");

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
void Console::initCmdLine(const sf::Vector2<int> &window_size)
{
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
void Console::initLog(int length, const sf::Vector2<int> &window_size)
{
  log_length = length;
  log_max = 8;
  log_position = window_size.y - 2 * font_size;
  log_offset = 0;
}

// Frees the memory allocated by log messages.
Console::~Console()
{
  for (auto iter = begin(log); iter != end(log); iter++)
  {
    delete *iter;
  }
}

std::string Console::prompt(const std::string &msg)
{
  listen_to_input = true;
  addToLog(msg, sf::Color::Magenta);
  command_entered = false;
  // Block this thread until the user enters a command.
  while (!command_entered)
  {
  }
  listen_to_input = false;
  // Return the most recently entered command.
  // std::string latest_command = log.at(log.size() - 1)->getString();
  return log.front()->getString();
}

void Console::inform(const std::string &msg)
{
  addToLog(msg, sf::Color::White);
}

void Console::addToLog(const std::string &msg, sf::Color color = sf::Color::Green)
{
  sf::Text *msg_text = new sf::Text(msg, font, font_size);
  msg_text->setColor(color);
  log.push_front(msg_text);

  // Delete old messages once the log reaches a certain length.
  if (log.size() > log_max)
  {
    sf::Text *to_delete = log.back();
    delete to_delete;
    log.pop_back();
  }

  log_offset = 0;
  updateLogPositions();
}

/* If the current command does not exceed the maximum length, adds
 * the character to the end of the command. */
void Console::readText(int character)
{
  if (listen_to_input && cmdstring.length() < max_cmd_length)
  {
    cmdstring += character;
    cmdline.setString(cmdstring);
  }
}

// Deletes a character from the current command.
void Console::deleteText()
{
  if (listen_to_input && cmdstring.length() > 2)
  {
    cmdstring.erase(cmdstring.length() - 1, 1);
    cmdline.setString(cmdstring);
  }
}

/* Accepts the current command, sends it to Game for parsing, and
 * records it in the message log. */
bool Console::enterCommand()
{
  if (listen_to_input)
  {
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
void Console::scroll(bool up)
{
  if (up)
  {
    if (log_offset + log_length < log.size())
    {
      log_offset = log_offset + 1;
    }
  }
  else
  {
    if (log_offset > 0)
    {
      log_offset = log_offset - 1;
    }
  }
  updateLogPositions();
}

/* Allows the render window to draw the console. This method draws
 * the command line and a subset of the message log. */
void Console::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  // Display the command line.
  target.draw(cmdline);
  // Display log_length number of messages starting from log_offset.
  int log_index = log_offset;
  int i = 0;
  while (i < log_length && log_index < log.size())
  {
    sf::Text *msg = log.at(log_index);
    target.draw(*msg);
    log_index = log_index + 1;
    i = i + 1;
  }
}

/* Sets the positions of the *log_length* number of messages that are
 * to be drawn to the screen. */
void Console::updateLogPositions()
{
  int i = 0;
  while (i < log_length && log_offset + i < log.size())
  {
    log.at(log_offset + i)->setPosition(0, log_position - i * font_size);
    i = i + 1;
  }
}
