#include "fairy_tail.hpp"
#include "graph.hpp"
#include "pathfinder.hpp"

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
    
    /* ------------------------ THE MAIN ALGORITHM ------------------------ */
    
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
                    std::cout << "Turn count: " << world->getTurnCount() << std::endl;
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
                    std::cout << "Turn count: " << world->getTurnCount() << std::endl;
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
                    std::cout << "Turn count: " << world->getTurnCount() << std::endl;
                    return 0;
                }

                // Is more effective to visit all nodes again then do something else
                // (linking graphs, counting coordinates and extra checks in case
                // when the labyrinth is divided on symmetric parts)
                ivan_g->resetDeadendNodes();
                ivan_g->resetVisitedNodes();
                ivan_g->getCurrent().lock()->deadendCheck();

                while (!meeting) {
                    ivan_a = ivan_p.getAdvice();
                    if (ivan_a.type == pathfinder::AdviceType::Rendezvous) {
                        if (!ivan_g->isExplored()) {
                            std::cout << "Algorithm error: labyrinth are not explored but Ivan got a Rendezvous advice" << std::endl;
                            return 0;
                        }
                        // For example, this could happen when the labyrinth have symmetric unlinked parts
                        std::cout << "Ivan and Elena cannot meet!" << std::endl;
                        std::cout << "Turn count: " << world->getTurnCount() << std::endl;
                        return 0;
                    }
                    for (size_t index = 0; index < ivan_a.route.size() && !meeting; ++index) {
                        meeting = pathfinder::movePals(ivan_p, ivan_a.route[index]);
                    }
                    // Possible everlasting cycle when algorithm is broken
                    // and give move advice even when all nodes are visited
                }
            }
        }
    }

    /* -------------------------------------------------------------------- */

    /* ---------------------------- CONCLUSION ---------------------------- */
    // At this point meeting must happened
    std::cout << "Ivan and Elena had meet!" << std::endl;
    std::cout << "Turn count: " << world->getTurnCount() << std::endl;

    // Only two cases possible (according to the task):
    // I. They meet at the one point
    // II. They went through each other
    //
    // Task ask to give one of possible map. In first case that simple: just connect and draw.
    // But in the second case we need to check if the general rectangle is less or equal
    // to the labyrinth size.

    // Restoring map is relative operation, so I belive (I cannot check everything)
    // that some maps could be lost, so I put check in order to take lost map variants
    // realtive to elena. And if elena also doens't have these variants then my algorithm
    // cannot display such map

    auto sheet = ivan_g->restoreMap(*elena_g, '@', '&');
    if (sheet.empty()) {
        sheet = elena_g->restoreMap(*ivan_g, '&', '@');
    }
    if (sheet.empty()) {
        std::cout << "Restore map error: This algorithm cannot restore map for this case" << std::endl;
        return 0;
    }
    std::cout << sheet << std::endl;
    return 0;
}
