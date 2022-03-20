#include "fairy_tail.hpp"
#include "graph.hpp"
#include "pathfinder.hpp"

namespace pathfinder {
    Direction directionToDirection(const graph::Direction direction) noexcept
    {
        switch (direction) {
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

    bool movePals(
        const Pathfinder& pathfinder,
        const AdviceRoute& advice_route
    ) noexcept
    {
        bool meeting = false;
        if (pathfinder.getCharacter() == Character::Ivan) {
            meeting = pathfinder.getWorld()->go(advice_route.world, Direction::Pass);
        }
        else {
            meeting = pathfinder.getWorld()->go(Direction::Pass, advice_route.world);
        }
        pathfinder.go(advice_route.graph);
        return meeting;
    }

    bool movePals(
        const Pathfinder& ivan_p,
        const Pathfinder& elena_p,
        const AdviceRoute& ivan_ar,
        const AdviceRoute& elena_ar
    ) noexcept
    {
        auto meeting = ivan_p.getWorld()->go(ivan_ar.world, elena_ar.world);
        ivan_p.go(ivan_ar.graph);
        elena_p.go(elena_ar.graph);
        return meeting;
    }

    /* Structs */
    AdviceRoute::AdviceRoute(const Direction t_world, const graph::Direction t_graph) noexcept : world(t_world), graph(t_graph) {}

    Advice::Advice(const AdviceType t_type, const std::vector<graph::Direction> t_route) noexcept : type(t_type)
    {
        route = std::vector<AdviceRoute>();
        route.reserve(t_route.size());
        for (auto& direction : t_route) {
            route.push_back(AdviceRoute(directionToDirection(direction), direction));
        }
    }

    Advice::Advice(const AdviceType t_type, const std::initializer_list<graph::Direction> t_route) noexcept
        : Advice(t_type, std::vector<graph::Direction>(t_route))
    {}

    Advice::Advice(const AdviceType t_type) noexcept : Advice(t_type, {}) {}

    /* Pathfinder */
    Pathfinder::Pathfinder(
        const std::shared_ptr<Fairyland> t_world,
        const Character t_char,
        const std::shared_ptr<graph::Graph> t_graph) noexcept
        : m_world(t_world), m_character(t_char), m_graph(t_graph)
    {}

    Advice Pathfinder::getAdvice() const noexcept
    {
        // For meeting in the labyrinth only one plan exists:
        // I. Explore the labyrinth for knowing it's size and your position in.
        // II. You met your pal.
        // III. You just need to restore the map.
        // 
        // II. You doesn't met your pal:
        // III. Wait until he\she is done with the exploring
        //         (at this step you can move in labyrinth center direction).
        // IV. Count nodes of the labyrinth.
        // V. If they are different - you cannot meet.
        // 
        // V. Otherwise restore the map (e.g. by deadends of graph)
        // VI. Find nearest node to both of you and move at it direction.
        // VII. If you met then all is good.
        // 
        // VII. But.. What if you doesn't met you friend in the rendezvous.
        // 
        // Then two variants exits:
        // 1. You have two equal labyrinth which are not connected. - Good case
        // 2. You have symmetric labyrinth. - Ass tear case
        // 
        //     In the first case only one way to restore map exists. So you can
        // know here and know that game is over. But in the second case you need
        // to find all ways to restore the map and try to reach the rendezvous
        // again OR one is waitng at the outer side and other is going to follow
        // the outer side (more realistic variant).
        //     When one follow the outer side is normal to reset visit nodes flags
        // and go and go until meeting friend or reaching visited node.
        //

        // DEADEND ADVICE
        auto node = m_graph->getCurrent().lock();
        if (node->deadendCheck()) {
            // Find only one no deadend
            for (auto& neig : node->getNeighbors()) {
                if (!neig.node.expired() && !neig.node.lock()->deadendCheck()) {
                    return Advice(AdviceType::Move, { neig.direction });
                }
            }
        }

        // VISIT UNVISITED ADVICE
        if (!m_graph->isExplored()) {
            auto route = m_graph->findUnvisitedNode();
            if (!route.empty()) {
                return Advice(AdviceType::Move, route);
            }
        }

        return Advice(AdviceType::Rendezvous);
    }

    Character Pathfinder::getCharacter() const noexcept
    {
        return m_character;
    }

    std::shared_ptr<Fairyland> Pathfinder::getWorld() const noexcept
    {
        return m_world;
    }

    void Pathfinder::go(const graph::Direction direction) const noexcept
    {
        m_graph->go(direction);
        updateNode();
        m_graph->getCurrent().lock()->deadendCheck();
    }

    void Pathfinder::updateNode() const noexcept
    {
        const auto directions = {
            graph::Direction::Left,
            graph::Direction::Right,
            graph::Direction::Up,
            graph::Direction::Down
        };

        for (auto direction : directions) {
            if (m_world->canGo(m_character, directionToDirection(direction))) {
                m_graph->createNodeAt(direction);
            }
        }
    }
}
