#pragma once

#include "graph.hpp"

#include <memory>
#include <vector>

namespace pathfinder {
    class Pathfinder;  // Predefines

    enum class AdviceType {
        Move,
        Rendezvous,  // Used when the labyrinth is explored and you ready to meet you friend.
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

    bool movePals(
        const Pathfinder& pathfinder,
        const AdviceRoute& advice_route
    ) noexcept;
    bool movePals(
        const Pathfinder& ivan_p,
        const Pathfinder& elena_p,
        const AdviceRoute& ivan_ar,
        const AdviceRoute& elena_ar
    ) noexcept;

    class Pathfinder {
    public:
        Pathfinder(const std::shared_ptr<Fairyland> world, const Character t_char, const std::shared_ptr<graph::Graph>) noexcept;

    public:
        Advice getAdvice() const noexcept;
        Character getCharacter() const noexcept;
        std::shared_ptr<Fairyland> getWorld() const noexcept;
        void go(const graph::Direction direction) const noexcept;
        Connection isConnected(const Pathfinder& other) const noexcept;
        void updateNode() const noexcept;

    private:
        std::shared_ptr<Fairyland> m_world;
        std::shared_ptr<graph::Graph> m_graph;
        Character m_character;
    };
}
