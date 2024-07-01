#include "game.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <functional>

const int Game::initial_army_size[] = {0, 0, 50, 35, 30, 25, 20};
const int Game::card_reinforcements[] = {4, 6, 8, 10, 12, 15, 20, 25};

Game::Game(sf::Vector2<int> screen_size, const std::string &map_file) : console(10, screen_size),
                                                                        map(map_file,
                                                                            sf::Vector2<int>(0, 0),
                                                                            sf::Vector2<int>(screen_size.x, screen_size.y - console.getSize().y)),
                                                                        g(rd()),
                                                                        cardset_counter(0)
{
  std::srand((unsigned)std::time(0));
  players.push_back(new PlanningAgent(map, 0, "Red", sf::Color::Red));
  // players.push_back(new HumanAgent(map, console, 1, sf::Color::Red));
  players.push_back(new RandomAgent(map, 1, "Purple", sf::Color::Magenta));
  players.push_back(new RandomAgent(map, 2, "Blue", sf::Color::Blue));
  players.push_back(new RandomAgent(map, 3, "Green", sf::Color::Green));

  // create the deck of cards.
  std::vector<std::string> types = {"foot", "cavalry", "artillery"};
  int type_idx = 0;
  std::vector<Territory *> shuffled_territories = map.getTerritories();
  std::shuffle(begin(shuffled_territories), end(shuffled_territories), g);
  for (int t_idx = 0; t_idx < shuffled_territories.size(); ++t_idx)
  {
    Card card;
    card.id = t_idx;
    card.type = types.at(type_idx);
    card.territory = shuffled_territories.at(t_idx)->getName();
    card_pile.push_back(card);

    type_idx = (type_idx == 2) ? 0 : type_idx + 1;
  }
  for (int i = 0; i < 2; ++i)
  {
    Card joker;
    joker.id = card_pile.size() + i;
    joker.type = "joker";
    joker.territory = "joker";
    card_pile.push_back(joker);
  }
}

Game::~Game()
{
  for (auto player : players)
  {
    delete player;
  }
}

bool Game::run()
{
  // testing
  for (const auto &card : card_pile)
  {
    std::cout << card.id << " : " << card.type << " : " << card.territory << std::endl;
  }

  // Players roll the dice to determine who will go first.

  // The game starts with players claiming territories until all are taken.
  // The first player places one army, second player places one army on unoccupied
  // territory, ... so on until all territories claimed.

  // Then, first player places second army on any of his territories, and so on
  // until all armies are placed.
  claimTerritories();

  // Now each player takes their 3 step turn until the game is over.
  auto iter = begin(players);
  while (!isOver())
  {
    // Player can only take their turn if they haven't lost - ie they still control territories.
    // If the player doesn't have any more territories, remove them from the game.
    IAgent *player = *iter;
    if (player->getNumberOfTerritories() > 0)
    {
      takeTurn(*iter);
      iter++;
    }
    else
    {
      iter = players.erase(std::find(begin(players), end(players), player));
    }
    // Move on to the next player.

    // this is very dumb apparently to increment the iterator here.
    // if I delete a player the iterator is invalidated
    // i.e. the pointer points to junk.
    if (iter == end(players))
    {
      iter = begin(players);
    }
  }
  return true;
}

bool Game::isOver()
{
  return players.size() == 1;
}

void Game::claimTerritories()
{
  // In this instance I do want a copy, but in general maybe not?
  std::vector<Territory *> unoccupied_territories = map.getTerritories();

  // Keep track of how many units each player has to place.
  std::vector<int> armies = std::vector<int>(players.size(), initial_army_size[players.size()]);

  int i = 0;
  while (!unoccupied_territories.empty())
  {
    IAgent *player = players.at(i);
    // Ask the player to choose a territory.
    Territory *territory = askAgentToChooseTerritory(player, unoccupied_territories, true);
    console.inform(player->getName() + " takes " + territory->getName());
    // Assign that territory to the player.
    assignTerritoryToAgent(territory, player, 1);
    // Remove the territory from the unoccupied list.
    unoccupied_territories.erase(
        std::find(unoccupied_territories.begin(), unoccupied_territories.end(), territory));

    // Decrement the number of armies this player has.
    --armies.at(i);
    // Move on to the next player.
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    i = (i < players.size() - 1) ? i + 1 : 0;
  }
  // All players still have some armies left at the end of the previous loop.
  i = 0;
  bool armies_left = true;
  while (armies_left)
  {
    armies_left = false;
    // Start with the first player, ask each player to reinforce one territory
    // they own until all players are out of units.
    if (armies.at(i) > 0)
    {
      IAgent *player = players.at(i);
      // need to implement the below. Can reuse the same function, rather just need to
      // pass in a container of territories that the player owns instead of a container
      // of unoccupied territories.
      Territory *territory = askAgentToChooseTerritory(player, map.getTerritories(player->getId()), false);
      // reinforce that territory with one unit.
      territory->reinforce(1);
      // decrement the player's units.
      --armies.at(i);
      // if this player still has units, set armies_left to true.
      if (armies.at(i) > 0)
      {
        armies_left = true;
      }
      // move on to the next player.
      i = (i < players.size() - 1) ? i + 1 : 0;
      // std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  }
}

void Game::takeTurn(IAgent *player)
{
  bool earns_card = false;
  // Player assigns reinforcements.
  assignReinforcements(player);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Player attacks.
  Territory *to, *from;
  int attacking_units;
  do
  {
    // When the player passes, they return nullptr for to and from.
    std::tie(to, from, attacking_units) = askAgentToAttack(player);
    if (to != nullptr)
    {
      IAgent *defender = players.at(to->getOccupierId());
      int defending_units = askAgentToDefend(defender, to, from, attacking_units);
      console.inform(player->getName() + " attacks " + to->getName() + " from " + from->getName() + " with " + std::to_string(attacking_units) + " units. " + defender->getName() + " defends with " + std::to_string(defending_units) + " units.");
      bool captured = resolveBattle(from, to, attacking_units, defending_units);
      if (captured)
      {
        earns_card = true;
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  } while (to != nullptr);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Player fortifies.
  int fortifying_units;
  std::tie(to, from, fortifying_units) = askAgentToFortify(player);
  if (to != nullptr)
  {
    from->reinforce(-fortifying_units);
    to->reinforce(fortifying_units);
    console.inform(player->getName() + " moves " + std::to_string(fortifying_units) + " units from " + from->getName() + " to " + to->getName() + ".");
  }

  std::this_thread::sleep_for(std::chrono::seconds(1));
  // Ask the player to draw a card if they captured a territory.
  if (earns_card)
  {
    bool draws = player->drawCard();
    if (draws)
    {
      // Perhaps we should throw an exception if the card_pile is empty?
      Card card = card_pile.at(card_pile.size() - 1);
      card_pile.pop_back();
      player->cards.push_back(card);

      std::cout << player->getName() << " draws " << cardstr(card) << "\n";
    }
  }
}

bool Game::resolveBattle(Territory *attacker, Territory *defender, int attacking_units, int defending_units)
{
  bool captured = false;
  int *attacker_dice = new int[attacking_units];
  int *defender_dice = new int[defending_units];
  // Roll the dice.
  for (int i = 0; i < attacking_units; i++)
  {
    attacker_dice[i] = rollDie();
  }
  for (int i = 0; i < defending_units; i++)
  {
    defender_dice[i] = rollDie();
  }

  int count = 0;
  int min_rolls = (attacking_units < defending_units) ? attacking_units : defending_units;
  int attacker_loss = 0, defender_loss = 0;
  while (count < min_rolls)
  {
    // Find the greatest dice roll for each player.
    int attacker_index = findMax(attacker_dice, attacking_units);
    int defender_index = findMax(defender_dice, defending_units);
    console.inform(players.at(attacker->getOccupierId())->getName() + "'s best dice roll is " + std::to_string(attacker_dice[attacker_index]) + " and " + players.at(defender->getOccupierId())->getName() + "'s best roll is " + std::to_string(defender_dice[defender_index]));
    // The defender wins in case of tie.
    if (attacker_dice[attacker_index] > defender_dice[defender_index])
    {
      console.inform(players.at(attacker->getOccupierId())->getName() + " wins the battle.");
      defender_loss += 1;
    }
    else
    {
      console.inform(players.at(defender->getOccupierId())->getName() + " wins the battle.");
      attacker_loss += 1;
    }
    // Discard the two highest dice for the next comparison.
    attacker_dice[attacker_index] = 0;
    defender_dice[defender_index] = 0;
    count += 1;
  }

  delete[] defender_dice;
  delete[] attacker_dice;

  // Update the units in the territories.
  defender->reinforce(-defender_loss);
  attacker->reinforce(-attacker_loss);

  // If the defender lost all their units, the attacker captures the territory.
  if (defender->getUnits() == 0)
  {
    console.inform(players.at(attacker->getOccupierId())->getName() + " captures " + defender->getName() + " from " + players.at(defender->getOccupierId())->getName() + "!");
    IAgent *attacking_agent = players.at(attacker->getOccupierId());
    int capturing_units = askAgentToCapture(attacking_agent, attacker, defender, attacking_units - attacker_loss);
    assignTerritoryToAgent(defender, attacking_agent, capturing_units);
    attacker->reinforce(-capturing_units);
    captured = true;
  }

  // Inform the attacker and defender of the outcome.
  players.at(attacker->getOccupierId())->informOfBattleOutcome(attacker->getOccupierId(), defender->getOccupierId(), attacker_loss, defender_loss, captured);
  players.at(defender->getOccupierId())->informOfBattleOutcome(attacker->getOccupierId(), defender->getOccupierId(), attacker_loss, defender_loss, captured);
  return captured;
}

void Game::assignReinforcements(IAgent *player)
{
  // Handle card logic.
  // If they have more than 5, they must choose a triplet to turn in.
  // Otherwise, they have a chance to turn in cards and gain more reinforcements
  int total_reinforcements = getNumberOfReinforcements(player);

  std::cout << player->getName() << " has " << total_reinforcements << " reinforcements.\n";

  Territory *territory;
  int reinforcements;

  while (total_reinforcements > 0)
  {
    std::tie(territory, reinforcements) = askAgentToReinforce(player, total_reinforcements);
    territory->reinforce(reinforcements);
    total_reinforcements = total_reinforcements - reinforcements;
    console.inform(player->getName() + " reinforces " + territory->getName() + " with " + std::to_string(reinforcements) + " units.");
  }
}

int Game::getNumberOfReinforcements(IAgent *player)
{
  int reinforcements;
  int player_territories = player->getNumberOfTerritories();
  if (player_territories <= 9)
  {
    reinforcements = 3;
  }
  else
  {
    reinforcements = player_territories / 3;
  }

  // Determine if player owns any continents and get their bonus.
  std::vector<std::string> player_continents = map.getContinents(player->getId());
  const std::map<std::string, int> continents_bonus = map.getContinentsBonus();
  for (auto &name : player_continents)
  {
    std::cout << player->getName() << " owns " << name << std::endl;
    reinforcements += continents_bonus.at(name);
  }

  // Check if player has enough cards to turn in.
  // Return a tuple of CardSets, if it's not empty
  // let the player choose one or None. Unless the player
  // has more than five cards, they are forced to turn in a set.
  std::vector<std::tuple<int, int, int>> card_sets = findCardSets(player);
  if (!card_sets.empty())
  {
    std::cout << player->getName() << " has " << card_sets.size() << " cardsets.\n";
    for (int i = 0; i < card_sets.size(); ++i)
    {
      int a, b, c;
      std::tie(a, b, c) = card_sets.at(i);
      Card c1, c2, c3;
      c1 = player->cards.at(a);
      c2 = player->cards.at(b);
      c3 = player->cards.at(c);
      std::cout << "\t" << cardstr(c1) << " " << cardstr(c2) << " " << cardstr(c3) << "\n";
    }
    // Ask player if they want to turn in a set of cards.
    CardSet cards_turned_in = player->turnInCards(card_sets);
    // if they have more than 5 cards, they must turn in a set.
    bool skip = std::get<0>(cards_turned_in).id == -1;
    while (skip && player->cards.size() > 5)
    {
      cards_turned_in = player->turnInCards(card_sets);
      skip = std::get<0>(cards_turned_in).id == -1;
    }

    if (!skip)
    {
      // remove the cards from the player's pile
      auto card_eq = [](const Card &a, const Card &b)
      {
        return a.id == b.id;
      };
      Card c1 = std::get<0>(cards_turned_in);
      Card c2 = std::get<1>(cards_turned_in);
      Card c3 = std::get<2>(cards_turned_in);
      std::vector<Card> cs = {c1, c2, c3};
      for (int i = 0; i < 3; ++i)
      {
        // remove card from player's pile -- std::remove doesn't act the way I expect it to.
        auto it = std::find_if(
            begin(player->cards),
            end(player->cards),
            std::bind(card_eq, std::placeholders::_1, cs.at(i)));
        player->cards.erase(it);
        // add card to bottom of the game pile.
        card_pile.insert(begin(card_pile), cs.at(i));
        // alternatively, we could put them on top and shuffle the deck.
      }
      // add to the reinforcements and increment the counter.
      int card_bonus;
      if (cardset_counter < 8)
      {
        card_bonus = card_reinforcements[cardset_counter];
      }
      else
      {
        card_bonus = card_reinforcements[7] + 5 * (cardset_counter - 7);
      }
      ++cardset_counter;
      std::cout << player->getName() << " receives " << card_bonus << "reinforcements.";

      reinforcements += card_bonus;
      // if player owns territories corresponding to turned in cards, those
      // territories get a bonus two reinforcements.
      for (int i = 0; i < 3; ++i)
      {
        Card c = cs.at(i);
        if (c.territory != "joker")
        {
          Territory *t = const_cast<Territory *>(map.getTerritory(c.territory));
          if (t->getOccupierId() == player->getId())
          {
            std::cout << player->getName() << " gets bonus 2 for owning " << c.territory << "\n";
            t->reinforce(2);
          }
        }
      }
    }
  }

  return reinforcements;
}

std::tuple<Territory *, int> Game::askAgentToReinforce(IAgent *agent, int total_reinforcements)
{
  const Territory *territory;
  int reinforcements;
  std::tie(territory, reinforcements) = agent->reinforce(total_reinforcements);
  /* Agent must return a territory they occupy and a positive number of units
     which does not exceed the remaining amount of reinforcements. */
  while (!agent->hasTerritory(territory) || reinforcements <= 0 || reinforcements > total_reinforcements)
  {
    std::tie(territory, reinforcements) = agent->reinforce(total_reinforcements);
  }
  return std::make_tuple(const_cast<Territory *>(territory), reinforcements);
}

Territory *Game::askAgentToChooseTerritory(IAgent *agent,
                                           const std::vector<Territory *> &valid_territories,
                                           bool choose_unoccupied)
{
  const Territory *territory;
  if (choose_unoccupied)
  {
    // First, agent must choose unoccupied territories.
    territory = agent->chooseTerritory(valid_territories, true);
    while (territory->isOccupied())
    {
      territory = agent->chooseTerritory(valid_territories, true);
    }
  }
  // Then, they must choose to reinforce territories they own.
  else
  {
    territory = agent->chooseTerritory(valid_territories, false);
    while (territory->getOccupierId() != agent->getId())
    {
      territory = agent->chooseTerritory(valid_territories, false);
    }
  }
  return const_cast<Territory *>(territory);
}

std::tuple<Territory *, Territory *, int> Game::askAgentToAttack(IAgent *agent)
{
  const Territory *to, *from;
  int attacking_units;
  std::tie(to, from, attacking_units) = agent->attack();
  // If agent returns nullptr for to, then they are passing their turn.
  if (to == nullptr)
  {
    return std::make_tuple(nullptr, nullptr, 0);
  }
  /* Agent must return an enemy territory and one of their own territories adjacent to it.
     The attacker can roll 1, 2, or 3 dice but must have at least one more unit in the attacking
     territory than the number of dice rolled. */
  while (agent->hasTerritory(to) || !agent->hasTerritory(from) || !from->hasNeighbor(to) ||
         attacking_units < 1 || attacking_units > 3 || from->getUnits() <= attacking_units || from->getUnits() <= 1)
  {
    std::tie(to, from, attacking_units) = agent->attack();
    if (to == nullptr)
    {
      return std::make_tuple(nullptr, nullptr, 0);
    }
  }
  // we're casting away const here because Game is allowed to modify territories but no agent should
  // be able to touch them directly.
  return std::make_tuple(const_cast<Territory *>(to), const_cast<Territory *>(from), attacking_units);
}

int Game::askAgentToDefend(IAgent *agent, Territory *defender, Territory *attacker, int attacking_units)
{
  /* Agent may roll either 1 or 2 dice, but must have at least as many units
     in the territory as the number of dice being rolled. */
  int defending_units = agent->defend(attacker, defender, attacking_units);
  while (defending_units < 1 || defending_units > 2 || defender->getUnits() < defending_units)
  {
    defending_units = agent->defend(attacker, defender, attacking_units);
  }
  return defending_units;
}

int Game::askAgentToCapture(IAgent *agent, Territory *from, Territory *to, int attacking_units)
{
  /* Agent must return a number that is at least as big as the number of attacking units but
     strictly less than the number of units in the territory the units are being moved from. */
  int capturing_units = agent->capture(from, to, attacking_units);
  while (capturing_units < attacking_units || capturing_units >= from->getUnits())
  {
    capturing_units = agent->capture(from, to, attacking_units);
  }
  return capturing_units;
}

std::tuple<Territory *, Territory *, int> Game::askAgentToFortify(IAgent *agent)
{
  /* Agent must return a territory they own, another territory they own, and a number
     that is strictly less than the number of units in the territory being moved from.
     If the agent does not wish to fortify, it should return nullptr for the territories. */
  const Territory *to, *from;
  int fortifying_units;
  std::tie(to, from, fortifying_units) = agent->fortify();
  if (to == nullptr)
  {
    return std::make_tuple(nullptr, nullptr, 0);
  }
  while (!agent->hasTerritory(to) || !agent->hasTerritory(from) || from->getUnits() <= fortifying_units || !map.areConnected(to, from))
  {
    std::tie(to, from, fortifying_units) = agent->fortify();
    if (to == nullptr)
    {
      return std::make_tuple(nullptr, nullptr, 0);
    }
  }
  return std::make_tuple(const_cast<Territory *>(to), const_cast<Territory *>(from), fortifying_units);
}

void Game::assignTerritoryToAgent(Territory *territory, IAgent *agent, int units)
{
  if (territory->isOccupied())
  {
    int old_id = territory->getOccupierId();
    players.at(old_id)->removeTerritory(territory);
  }
  territory->setOccupier(agent, units);
  agent->addTerritory(territory);
}

int Game::rollDie()
{
  return (rand() % 6) + 1;
}

int Game::findMax(int *arr, int size)
{
  int max = arr[0];
  int max_index = 0;
  for (int i = 1; i < size; i++)
  {
    if (arr[i] > max)
    {
      max = arr[i];
      max_index = i;
    }
  }
  return max_index;
}

std::vector<std::tuple<int, int, int>> Game::findCardSets(IAgent *player)
{
  std::vector<std::tuple<int, int, int>> sets;
  // look through all combinations of three cards.
  std::vector<Card> cards = player->cards;
  if (cards.size() < 3)
  {
    return sets;
  }

  for (int i = 0; i < cards.size(); ++i)
  {
    for (int j = i + 1; j < cards.size(); ++j)
    {
      for (int k = j + 1; k < cards.size(); ++k)
      {
        Card a = cards.at(i);
        Card b = cards.at(j);
        Card c = cards.at(k);

        if (a.type == "joker" || b.type == "joker" || c.type == "joker")
        {
          sets.push_back(std::tie(i, j, k));
        }
        else if (a.type == b.type && a.type == c.type)
        {
          sets.push_back(std::tie(i, j, k));
        }
        else if (a.type != b.type && b.type != c.type && c.type != a.type)
        {
          sets.push_back(std::tie(i, j, k));
        }
      }
    }
  }
  return sets;
}