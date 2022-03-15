#include "fairy_tail.hpp"
#include "graph.hpp"
#include "pathfinder.hpp"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>


int main()
{
    auto world = std::make_shared<Fairyland>();

    /**/
    auto ivan_graph = std::make_shared<graph::Graph>(std::make_shared<graph::Node>(true));
    auto ivan_pathfinder = pathfinder::Pathfinder(world, Character::Ivan, ivan_graph);

    ivan_pathfinder.updateNode();

    std::cout << "Is Ivan deadend: " << ivan_graph->getCurrent().lock()->deadendCheck() << std::endl;
    
    ivan_graph->normalizeRect();
    std::cout << ivan_graph->printMap('@') << std::endl;

    auto elena_graph = std::make_shared<graph::Graph>(std::make_shared<graph::Node>(true));
    auto elena_pathfinder = pathfinder::Pathfinder(world, Character::Elena, elena_graph);

    elena_pathfinder.updateNode();

    std::cout << "Is Elena deadend: " << elena_graph->getCurrent().lock()->deadendCheck() << std::endl;

    elena_graph->normalizeRect();
    std::cout << elena_graph->printMap('&');
    /**/

    std::string conn;
    switch (ivan_pathfinder.isConnected(elena_pathfinder))
    {
    case pathfinder::Connection::Linked:
        conn = "Linked";
    case pathfinder::Connection::Divided:
        conn = "Divided";
    default:
        conn = "Unknown";
    }

    std::cout << "Connection state: " << conn << std::endl;

    return 0;
}
