#include "fairy_tail.hpp"
#include "graph.hpp"
#include "pathfinder.hpp"

namespace pathfinder
{
    Pathfinder::Pathfinder(
        const std::shared_ptr<Fairyland> t_world,
        const Character t_char,
        const std::shared_ptr<graph::Graph> t_graph) noexcept
        : m_world(t_world), m_char(t_char), m_graph(t_graph), m_cur_strategy(Strategy::None) {}

    graph::Direction Pathfinder::getAdvice() noexcept
    {
        switch (m_cur_strategy)
        {
        default:  // None
            // if deadlock then no choice
            return graph::Direction::Left;
        }
    }

    void Pathfinder::updateNode() noexcept
    {
        if (m_world->canGo(m_char, Direction::Left))
        {
            m_graph->createNodeAt(graph::Direction::Left);
        }
        if (m_world->canGo(m_char, Direction::Right))
        {
            m_graph->createNodeAt(graph::Direction::Right);
        }
        if (m_world->canGo(m_char, Direction::Up))
        {
            m_graph->createNodeAt(graph::Direction::Up);
        }
        if (m_world->canGo(m_char, Direction::Down))
        {
            m_graph->createNodeAt(graph::Direction::Down);
        }
    }
}
