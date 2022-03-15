#include "fairy_tail.hpp"
#include "graph.hpp"
#include "pathfinder.hpp"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

Direction graphToFairytail(const graph::Direction direction)
{
    switch (direction)
    {
    case graph::Direction::Left:
        return Direction::Left;
    case graph::Direction::Right:
        return Direction::Right;
    case graph::Direction::Up:
        return Direction::Up;
    default:
        return Direction::Down;
    }
}

int main()
{
    auto world = std::make_shared<Fairyland>();

    /**/
    auto ivan_graph = std::make_shared<graph::Graph>(std::make_shared<graph::Node>(true));
    auto ivan_pathfinder = pathfinder::Pathfinder(world, Character::Ivan, ivan_graph);
    ivan_pathfinder.updateNode();
    std::cout << ivan_graph->getRect().first.second << std::endl;
    auto advice = ivan_pathfinder.getAdvice();

    std::cout << "Is Ivan deadend: " << ivan_graph->getCurrent().lock()->deadendCheck() << std::endl;
    
    ivan_graph->normalizeRect();
    std::cout << ivan_graph->printMap('@') << std::endl;

    auto elena_graph = std::make_shared<graph::Graph>(std::make_shared<graph::Node>(true));
    auto elena_pathfinder = pathfinder::Pathfinder(world, Character::Elena, elena_graph);
    elena_pathfinder.updateNode();
    advice = elena_pathfinder.getAdvice();

    std::cout << "Is Elena deadend: " << elena_graph->getCurrent().lock()->deadendCheck() << std::endl;

    elena_graph->normalizeRect();
    std::cout << elena_graph->printMap('&');
    /**/

    std::cout << "Is connected: " << ivan_pathfinder.isConnected(elena_pathfinder) << std::endl;

    return 0;
}
