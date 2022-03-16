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

    /* Structs */
    AdviceRoute::AdviceRoute(const Direction t_world, const graph::Direction t_graph) noexcept : world(t_world), graph(t_graph) {}

    Advice::Advice(const AdviceType t_type, const std::vector<graph::Direction> t_route) noexcept : type(t_type)
    {
        auto advice_route = std::vector<AdviceRoute>();
        for (auto& direction : t_route) {
            advice_route.push_back(AdviceRoute(directionToDirection(direction), direction));
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
        : m_world(t_world), m_char(t_char), m_graph(t_graph)
    {}

    Advice Pathfinder::getAdvice() noexcept
    {
        auto node = m_graph->getCurrent().lock();

        // DEADEND ADVICE
        if (node->deadendCheck()) {
            // Find only one no deadend
            for (auto& neig : node->getNeighbors()) {
                if (!neig.node.expired() && !neig.node.lock()->deadendCheck()) {
                    return Advice(AdviceType::Move, { neig.direction });
                }
            }
        }

        // VISIT UNVISITED ADVICE
        for (auto& neig : node->getNeighbors()) {
            if (!neig.node.expired() && !neig.node.lock()->m_visited) {
                return Advice(AdviceType::Move, { neig.direction });
            }
        }
    }

    Connection Pathfinder::isConnected(const Pathfinder& other) const noexcept
    {
        if (!m_graph->isExplored() || !other.m_graph->isExplored()) {
            return Connection::Unknown;
        }
        return m_graph->getNodeCount() == other.m_graph->getNodeCount()
            ? Connection::Linked : Connection::Divided;
    }

    void Pathfinder::updateNode() noexcept
    {
        const auto directions = {
            graph::Direction::Left,
            graph::Direction::Right,
            graph::Direction::Up,
            graph::Direction::Down
        };

        for (auto direction : directions) {
            if (m_world->canGo(m_char, directionToDirection(direction))) {
                m_graph->createNodeAt(direction);
            }
        }
    }
}
