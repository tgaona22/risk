#include <SFML/Graphics.hpp>
#include <cstdio>
#include <string>
#include <vector>
#include "console.h"
#include "map.h"

int main() {
  sf::Vector2<int> window_size(600, 400);
  sf::RenderWindow window(sf::VideoMode(window_size.x, window_size.y), "RISK");

  Console console(5, window_size);
  Map map("../testboard.txt");

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
	window.close();
      }
      if (event.type == sf::Event::TextEntered) {
	if (event.text.unicode < 128 && event.text.unicode > 31) {
	  console.readText(event.text.unicode);
	}	  
      }
      if (event.type == sf::Event::KeyPressed) {
	switch (event.key.code) {
	case sf::Keyboard::BackSpace:
	  console.deleteText();
	  break;
	case sf::Keyboard::Return:
	  console.enterCommand();
	  break;
	case sf::Keyboard::Up:
	  console.scroll(true);
	  break;
	case sf::Keyboard::Down:
	  console.scroll(false);
	  break;
	default:
	  break;
	}
      }      
    }

    window.clear();
    window.draw(console);
    window.draw(map);
    window.display();
  }

}
