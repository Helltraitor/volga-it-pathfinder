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

    auto ivan_g = std::make_shared<graph::Graph>(std::make_shared<graph::Node>(true));
    auto ivan_p = pathfinder::Pathfinder(world, Character::Ivan, ivan_g);
    ivan_p.updateNode();
    ivan_g->getCurrent().lock()->deadendCheck();

    auto elena_g = std::make_shared<graph::Graph>(std::make_shared<graph::Node>(true));
    auto elena_p = pathfinder::Pathfinder(world, Character::Elena, elena_g);
    elena_p.updateNode();
    elena_g->getCurrent().lock()->deadendCheck();
    /* -------------------------------------------------------------------- */
    
    /* -------------------- START OF THE MAIN ALGORITHM -------------------- */
    auto ivan_a = ivan_p.getAdvice();
    auto elena_a = elena_p.getAdvice();

    auto syncronized = true;
    auto meeting = false;

    while (!meeting) {
        if (!syncronized) {
            ivan_a = ivan_p.getAdvice();
            elena_a = elena_p.getAdvice();
        }

        if (ivan_a.type == pathfinder::AdviceType::Move) {
            if (elena_a.type == pathfinder::AdviceType::Move) {
                // Both must go until met or somebody reach spot
                // In second case this person needs new advice
                auto ivan_d = ivan_a.route.size();
                auto elena_d = elena_a.route.size();
                auto distance = ivan_d < elena_d ? ivan_d : elena_d;

                for (size_t index = 0; index < distance && !meeting; ++index) {
                    meeting = pathfinder::movePals(ivan_p, elena_p, ivan_a.route[index], elena_a.route[index]);
                }
                syncronized = false;
            }
            else /* elena_a.type == pathfinder::AdviceType::Rendezvous */ {
                // Rendezvous appears only if one of them is done with the explore
                if (elena_g->getNodeCount() < ivan_g->getNodeCount() && elena_g->isExplored()) {
                    std::cout << "Ivan and Elena cannot meet!" << std::endl;
                    return 0;
                }
                for (size_t index = 0; index < ivan_a.route.size() && !meeting; ++index) {
                    meeting = pathfinder::movePals(ivan_p, ivan_a.route[index]);
                }
                syncronized = false;
            }
        }
        else /* ivan_a.type == pathfinder::AdviceType::Rendezvous */ {
            // Same as previous but relative to Elena
            if (elena_a.type == pathfinder::AdviceType::Move) {
                if (ivan_g->getNodeCount() < elena_g->getNodeCount() && ivan_g->isExplored()) {
                    std::cout << "Ivan and Elena cannot meet!" << std::endl;
                    return 0;
                }
                for (size_t index = 0; index < elena_a.route.size() && !meeting; ++index) {
                    meeting = pathfinder::movePals(elena_p, elena_a.route[index]);
                }
                syncronized = false;
            }
            else /* elena_a.type == pathfinder::AdviceType::Rendezvous */ {
                // That case is the most interesting because if labyrinths have the same node count
                // then we need to check topologic and then try to concat graphs
                if (!ivan_g->isExplored() || !elena_g->isExplored()) {
                    std::cout << "Algorithm error: labyrinth are not explored but both pals got a Rendezvous advice" << std::endl;
                    return 0;
                }

                if (ivan_g->getNodeCount() != elena_g->getNodeCount()) {
                    std::cout << "Ivan and Elena cannot meet!" << std::endl;
                    return 0;
                }

                // They knows they polar coords so you need only check topologic (by coords)
                // and then one (ivan for e.g.) visit all nodes again and if he find Elena
                // then labyrinth is connected otherwise there are two labyrinth on one map
                break; // TODO
            }
        }
    }
    /* --------------------------------------------------------------------- */

    std::cout << "MEETING == " << meeting << std::endl;
    std::cout << "IVAN NODES: " << ivan_g->getNodeCount() << std::endl;
    std::cout << "ELENA NODES: " << elena_g->getNodeCount() << std::endl;

    std::cout << "Is Ivan deadend: " << ivan_g->getCurrent().lock()->deadendCheck() << std::endl;

    ivan_g->normalizeRect();
    std::cout << ivan_g->printMap('@') << std::endl;


    std::cout << "Is Elena deadend: " << elena_g->getCurrent().lock()->deadendCheck() << std::endl;

    elena_g->normalizeRect();
    std::cout << elena_g->printMap('&');
    /**/

    return 0;
}
