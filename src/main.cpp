#include "fairy_tail.hpp"
#include "graph.hpp"
#include "pathfinder.hpp"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

int main()
{
    /* -------------------------- INITIALIZATION -------------------------- */
    auto world = std::make_shared<Fairyland>();

    auto ivan_graph = std::make_shared<graph::Graph>(std::make_shared<graph::Node>(true));
    auto ivan_p = pathfinder::Pathfinder(world, Character::Ivan, ivan_graph);
    ivan_p.updateNode();
    ivan_graph->getCurrent().lock()->deadendCheck();

    auto elena_graph = std::make_shared<graph::Graph>(std::make_shared<graph::Node>(true));
    auto elena_p = pathfinder::Pathfinder(world, Character::Elena, elena_graph);
    elena_p.updateNode();
    elena_graph->getCurrent().lock()->deadendCheck();
    /* -------------------------------------------------------------------- */

    // NODES CANNOT UPDATES
    std::cout << "IVAN NODES: " << ivan_graph->getNodeCount() << std::endl;
    std::cout << "ELENA NODES: " << elena_graph->getNodeCount() << std::endl;

    auto ivan_advice = ivan_p.getAdvice();
    auto elena_advice = elena_p.getAdvice();

    auto syncronized = true;
    auto meeting = false;

    while (!meeting) {
        if (!syncronized) {
            ivan_advice = ivan_p.getAdvice();
            elena_advice = elena_p.getAdvice();
        }

        if (ivan_advice.type == pathfinder::AdviceType::Move) {
            if (elena_advice.type == pathfinder::AdviceType::Move) {
                // Both must go until met or somebody reach spot
                // In second case this person needs new advice
                auto ivan_d = ivan_advice.route.size();
                auto elena_d = elena_advice.route.size();
                auto distance = ivan_d < elena_d ? ivan_d : elena_d;

                for (size_t index = 0; index < distance && !meeting; ++index) {
                    meeting = pathfinder::movePals(ivan_p, elena_p, ivan_advice.route[index], elena_advice.route[index]);
                }
                syncronized = false;
            }
            else /* elena_advice.type == pathfinder::AdviceType::Rendezvous */ {
                if (ivan_p.isConnected(elena_p) == pathfinder::Connection::Divided) {
                    std::cout << "Ivan and Elena cannot meet!" << std::endl;
                    return 0;
                }
                for (size_t index = 0; index < ivan_advice.route.size() && !meeting; ++index) {
                    meeting = pathfinder::movePals(ivan_p, ivan_advice.route[index]);
                }
                syncronized = false;
            }
        }
        else /* ivan_advice.type == pathfinder::AdviceType::Rendezvous */ {
            if (elena_advice.type == pathfinder::AdviceType::Move) {
                if (ivan_p.isConnected(elena_p) == pathfinder::Connection::Divided) {
                    std::cout << "Ivan and Elena cannot meet!" << std::endl;
                    return 0;
                }
                for (size_t index = 0; index < elena_advice.route.size() && !meeting; ++index) {
                    meeting = pathfinder::movePals(elena_p, elena_advice.route[index]);
                }
                syncronized = false;
            }
            else /* elena_advice.type == pathfinder::AdviceType::Rendezvous */ {
                if (ivan_p.isConnected(elena_p) == pathfinder::Connection::Divided) {
                    std::cout << "Ivan and Elena cannot meet!" << std::endl;
                    return 0;
                }
                std::cout << "Rand" << std::endl;
                // Help them find eachother
                break; // TODO
            }
        }
    }

    std::cout << "MEETING == " << meeting << std::endl;
    std::cout << "IVAN NODES: " << ivan_graph->getNodeCount() << std::endl;
    std::cout << "ELENA NODES: " << elena_graph->getNodeCount() << std::endl;

    std::cout << "Is Ivan deadend: " << ivan_graph->getCurrent().lock()->deadendCheck() << std::endl;

    ivan_graph->normalizeRect();
    std::cout << ivan_graph->printMap('@') << std::endl;


    std::cout << "Is Elena deadend: " << elena_graph->getCurrent().lock()->deadendCheck() << std::endl;

    elena_graph->normalizeRect();
    std::cout << elena_graph->printMap('&');
    /**/

    std::string conn;
    switch (ivan_p.isConnected(elena_p)) {
        case pathfinder::Connection::Linked:
            conn = "Linked";
            break;
        case pathfinder::Connection::Divided:
            conn = "Divided";
            break;
        default:
            conn = "Unknown";
            break;
    }

    std::cout << "Connection state: " << conn << std::endl;

    return 0;
}
