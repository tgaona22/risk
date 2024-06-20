#include <SFML/Graphics.hpp>
#include <cstdio>
#include <string>
#include <vector>
#include <thread>

#include "game.h"
#include "console.h"
#include "map.h"

void startGame(Game *game);

int main()
{
  sf::Vector2<int> window_size(900, 600);
  sf::RenderWindow window(sf::VideoMode(window_size.x, window_size.y), "RISK");

  std::string root_dir = "C:\\Users\\TylerGaona\\Documents\\Projects\\tag2\\risk\\";

  Game game(window_size, root_dir + "testboard2.txt");
  // It is necessary to run the game on a separate thread so the UI doesn't block.
  std::thread game_thread(startGame, &game);
  game_thread.detach();

  while (window.isOpen())
  {
    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
      {
        window.close();
      }
      if (event.type == sf::Event::TextEntered)
      {
        if (event.text.unicode < 128 && event.text.unicode > 31)
        {
          game.getConsole().readText(event.text.unicode);
        }
      }
      if (event.type == sf::Event::KeyPressed)
      {
        switch (event.key.code)
        {
        case sf::Keyboard::BackSpace:
          game.getConsole().deleteText();
          break;
        case sf::Keyboard::Return:
          game.getConsole().enterCommand();
          break;
        case sf::Keyboard::Up:
          game.getConsole().scroll(true);
          break;
        case sf::Keyboard::Down:
          game.getConsole().scroll(false);
          break;
        default:
          break;
        }
      }
    }

    window.clear();
    window.draw(game.getConsole());
    window.draw(game.getMap());
    window.display();
  }
}

void startGame(Game *game)
{
  game->run();
}
