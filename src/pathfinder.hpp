#pragma once

#include "graph.hpp"

#include <memory>
#include <vector>

namespace pathfinder {
    enum class AdviceType {
        Move,
        Rendezvous,
        Syncronize,
        Wait
    };

    enum class Connection {
        Linked,
        Divided,
        Unknown
    };

    struct AdviceRoute {
        Direction world;
        graph::Direction graph;

        AdviceRoute(const Direction t_world, const graph::Direction graph) noexcept;
    };

    struct Advice {
        AdviceType type;
        std::vector<AdviceRoute> w_route;

        Advice(const AdviceType t_type, const std::vector<graph::Direction> t_route) noexcept;
        Advice(const AdviceType t_type, const std::initializer_list<graph::Direction> t_route) noexcept;
        Advice(const AdviceType t_type) noexcept;
    };

    Direction directionToDirection(const graph::Direction direction) noexcept;

    class Pathfinder {
    public:
        Pathfinder(const std::shared_ptr<Fairyland> world, const Character t_char, const std::shared_ptr<graph::Graph>) noexcept;

    public:
        Advice getAdvice() noexcept;
        Connection isConnected(const Pathfinder& other) const noexcept;
        void updateNode() noexcept;

    private:
        std::shared_ptr<Fairyland> m_world;
        std::shared_ptr<graph::Graph> m_graph;
        Character m_char;
    };
}
