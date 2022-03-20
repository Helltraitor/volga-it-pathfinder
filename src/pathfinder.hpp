#pragma once

#include "graph.hpp"

#include <memory>
#include <vector>

namespace pathfinder {
    struct AdviceRoute; // Predefines
    class Pathfinder;

    /// Converts graph direction enum into fairytail version. Cannot returns Pass variant
    /// 
    /// @param direction graph::Direction that must be converted to Direction that compatible to Fairyland
    /// 
    /// @returns Fairytail direction that can be used with Fairyland world
    Direction directionToDirection(const graph::Direction direction) noexcept;

    /// Moves pals in the world and in the graph using Pathfinder::go method. This overload moves only
    /// Pathfinder::getCharacter person and makes another to pass
    /// 
    /// @param pathfinder Pathfinder of person who will be moved
    /// @param advice_route AdviceRoute that will be applied to the world and pathfinder's graph
    /// 
    /// @returns True if pals had met otherwise false
    bool movePals(
        const Pathfinder& pathfinder,
        const AdviceRoute& advice_route
    ) noexcept;

    /// Moves pals in the world and in the graph using Pathfinder::go method. This overload moves both pals
    /// 
    /// @returns True if pals had met otherwise false
    bool movePals(
        const Pathfinder& ivan_p,
        const Pathfinder& elena_p,
        const AdviceRoute& ivan_ar,
        const AdviceRoute& elena_ar
    ) noexcept;

    /// Represents basic type of Advice
    enum class AdviceType {
        Move,        //!< Used with non-empty AdviceRoute
        Rendezvous,  //!< Used when the labyrinth is explored and you ready to meet you friend.
    };

    /// Represents directions relative to the world and to the graph which must be applied to reach some neighbor node
    struct AdviceRoute {
        Direction world;         //!< World direction to neighbor node
        graph::Direction graph;  //!< Graph direction to neighbor node

        AdviceRoute(const Direction t_world, const graph::Direction graph) noexcept;
    };

    /// Represent given advice by Pathfinder::getAdvice method
    struct Advice {
        AdviceType type;                 //!< Type of given advice
        std::vector<AdviceRoute> route;  //!< Optional route, that can be empty but will be initialized

        Advice(const AdviceType t_type, const std::vector<graph::Direction>& t_route) noexcept;
        Advice(const AdviceType t_type, const std::initializer_list<graph::Direction>& t_route) noexcept;
        Advice(const AdviceType t_type) noexcept;
    };

    /// Represents class that used for searching friend in the labyrinth
    class Pathfinder {
    public:
        /// @param t_world Fairyland shared pointer to the world
        /// @param t_char A fairyland character that represents person on the map
        /// @param t_graph A graph with the initialized start node of this person
        Pathfinder(const std::shared_ptr<Fairyland> t_world, const Character t_char, const std::shared_ptr<graph::Graph> t_graph) noexcept;

    public:
        /// Gives an advice. For full algorinth check source code
        /// 
        /// @returns An advice according to current situation in the labyrinth
        Advice getAdvice() const noexcept;

        /// @returns A fairytail character which used this pathfinder to reach pal
        Character getCharacter() const noexcept;

        /// @returns A fairytail world where person tries to find the pal
        std::shared_ptr<Fairyland> getWorld() const noexcept;

        /// Moves the pal in the indicated direction in the graph, also updates node. Normally must be used through
        /// the movePals function
        void go(const graph::Direction direction) const noexcept;

        /// Updates node using Graph::createNodeAt and Fairyland::canGo. Must be used after every pals move.
        /// Normally must be used through the Pathfinder::go method
        void updateNode() const noexcept;

    private:
        std::shared_ptr<Fairyland> m_world;     //!< A shared pointer to the world (world must be same with the pal)
        std::shared_ptr<graph::Graph> m_graph;  //!< An unique graph of the labyrinth (both must have different graphs)
        Character m_character;                  //!< The character who relative to which the labyrinth being explored
    };
}
