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

        // DEADEND ADVICE
        auto node = m_graph->getCurrent().lock();
        if (node->deadendCheck()) {
            // Find only one no deadend
            for (auto& neig : node->getNeighbors()) {
                if (!neig.node.expired() && !neig.node.lock()->deadendCheck()) {
                    return Advice(AdviceType::Move, { neig.direction });
                }
            }

            // If all deadend then all are visited, so use shortcut
            return Advice(AdviceType::Rendezvous);
        }

        // VISIT UNVISITED ADVICE
        auto route = m_graph->findUnvisitedNode();
        if (!route.empty()) {
            return Advice(AdviceType::Move, route);
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

    Connection Pathfinder::isConnected(const Pathfinder& other) const noexcept
    {
        auto self_explored = static_cast<int>(m_graph->isExplored());
        auto other_explored = static_cast<int>(other.m_graph->isExplored());

        switch ((self_explored << 1) | other_explored) {
            // False, True
            case 1:
                return m_graph->getNodeCount() > other.m_graph->getNodeCount()
                    ? Connection::Divided : Connection::Unknown;
            // True, False
            case 2:
                return m_graph->getNodeCount() < other.m_graph->getNodeCount()
                    ? Connection::Divided : Connection::Unknown;
            // True, True
            case 3:
                return m_graph->getNodeCount() == other.m_graph->getNodeCount()
                    ? Connection::Linked : Connection::Divided;
            // False, False
            default:
                return Connection::Unknown;
        }
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
