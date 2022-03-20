#pragma once

#include <memory>
#include <string>
#include <vector>

namespace graph {
    class Node;  // Predefinition

    enum class Direction {
        Left,
        Right,
        Up,
        Down
    };

    /// Represents the neighbor of the choosen one graph. Neighbor node can be expired (wall)
    struct Neighbor {
        std::weak_ptr<Node> node;  //!< A weak pointer to existing node (passage) or to expired node (wall)
        Direction direction;       //!< Direction from other node to this

        Neighbor(const std::weak_ptr<Node> t_node, const Direction t_direction) noexcept;
    };

    /// Represents node position relative to the start node in the graph
    struct Position {
        int x;
        int y;

        Position(const int t_x, const int t_y) noexcept;

        /// Returns a new position at the direction relative to this position
        Position at(const Direction direction) const noexcept;
    };

    /// Represents a 'rectangle' which could be used to cover the graph and all it's nodes
    struct Rectangle {
        int min_x;
        int min_y;
        int max_x;
        int max_y;

        Rectangle(const int t_min_x, const int t_min_y, const int t_max_x, const int t_max_y) noexcept;
    };

    /// Represents a route to the head relative to the current node using the world directions and the graph directions
    struct Tadpole {
        std::vector<Direction> route;  //!< The route to head node
        std::vector<Position> nodes;   //!< Positions of visited by route nodes
        std::weak_ptr<Node> head;

        Tadpole(
            const std::vector<Direction> t_route,
            const std::vector<Position> t_nodes,
            const std::weak_ptr<Node> t_head) noexcept;

        /// @returns The vector of tadpoles from this to the neighbor passages in the labyrinth
        std::vector<Tadpole> produceTadpole() const noexcept;
    };

    /// Represents the graph node with a position
    class Node {
    public:
        Node(const Node& node) noexcept;
        Node(const Position& pos,
             const bool visited,
             const std::weak_ptr<Node> left,
             const std::weak_ptr<Node> right,
             const std::weak_ptr<Node> up,
             const std::weak_ptr<Node> down) noexcept;
        Node(const Position& pos, const bool visited) noexcept;
        Node(const bool visited) noexcept;
        Node() noexcept;

    public:
        /// Gets a cached value or does deadend check. Node is a deadend if only one non-deadend node exists near
        /// to this
        /// 
        /// @returns A value of internal boolean field m_deadend or do deadend check if node is visited.
        bool deadendCheck() noexcept;

        /// @returns All neighbors of the current node even when some of them is wall. In that case some
        /// of them is expired
        std::vector<Neighbor> getNeighbors() const noexcept;

        /// @returns The node at the indicated direction even when direction point to the wall. In that case 
        /// weak ptr is expired
        std::weak_ptr<Node> getNode(const Direction direction) const noexcept;

        /// Sets the internal boolean field m_deadend to false which could help for rerun the labyrinth
        void resetDeadend() noexcept;

    public:
        Position m_position;
        bool m_visited;
        std::weak_ptr<Node> m_left;
        std::weak_ptr<Node> m_right;
        std::weak_ptr<Node> m_up;
        std::weak_ptr<Node> m_down;

    private:
        bool m_deadend;
    };

    /// Represents a node net which represent explored parts of the labyrinth
    class Graph {
    public:
        Graph(std::shared_ptr<Node> start) noexcept;

    public:
        /// Creates or find the node at the direction relative to the current and linking this node to another known nodes.
        /// Have O(n) complexity
        void createNodeAt(const Direction direction) noexcept;

        /// Searching the nearest node. Nodes are tooks in (left, right, up, down) order.
        /// Flat version of recursive function - will not occur stack overflow error. Have O(n) complexity
        ///
        /// @returns The nearest unvisited node using tadpoles. 
        std::vector<Direction> findUnvisitedNode() const noexcept;

        /// @returns The latest visited node (node where person right now in Fairyland)
        std::weak_ptr<Node> getCurrent() const noexcept;

        /// @returns Amount of known nodes
        size_t getNodeCount() const noexcept;

        /// @returns Positions of known nodes
        std::vector<Position> getPassagesPositions() const noexcept;

        /// @returns Amount of expired neighbor nodes only of visited nodes. These neighbors represent walls
        /// in the labyrinth
        std::vector<Position> getWallsPositions() const noexcept;

        // Sets node at the direction as current and makes it visited. Must be used only after `Pathfinder::updateNode`.
        // For now `Pathfinder::updateNode` is called in `Pathfinder::go` which must be used for this kind of operations
        // due to synchronization of person position in the world and in the graph.
        // 
        // @throws std::runtime_error when current node is expired (when previous wasn't updated)
        void go(const Direction direction);

        /// Checks if all nodes are visited
        bool isExplored() const noexcept;

        /// Checks if walls of this graph are intersected with passages of another graph and does the same for walls
        /// of another graph. Have O((n + m)^2) complexity
        bool isIntersectedWith(const Graph& graph) const noexcept;

        /// Shifts nodes position such way that graph will have only non-negative nodes positions
        void normalizeRect() noexcept;

        /// Resets deadend node's internal variables. Must be used before rerun the labyrinth
        void resetDeadendNodes() const noexcept;

        /// Resets visit of all node except the current. Must be used before rerun the labyrinth
        void resetVisitedNodes() const noexcept;

        /// Tries to restore map using information of this and partner's graphs. Does it relative to five spots:
        /// Current node position (if they had met here) and left, right, up, and down positions relative
        /// to the current node.
        /// Could returns the empty string when it impossible to do relative to this graph (in that case you need
        /// to swap these graphs) or when algorithm cannot restore this map.
        /// 
        /// @param graph Graph which will be tried to combines relative to this
        /// @param this_start Char that represents this start on the future map
        /// @param other_start Char that represents start of other graph on the future map
        /// 
        /// @returns Map of the labyrinth or empty string
        std::string restoreMap(Graph& graph, const char this_start, const char other_start) noexcept;

        /// Shifts graph by delta_x and delta_y relative to the current position
        void shiftRect(const int delta_x, const int delta_y) noexcept;

    private:
        /// Draws map using shifted graphs (this and other). Must be used only when rectangle contains by 10x10 map
        std::string drawMap(const Graph& graph, const char this_start, const char other_start) noexcept;

        /// Updates rectangle if the given position has max or / and min values then rect has. Rect has this meaning:
        /// [min x, min y; max x, max y]
        /// And it's being used for map normalization after Ivan and Elena meeting.
        void updateRectangle(const Position& pos) noexcept;

    private:
        Rectangle m_rectangle;
        std::vector<std::shared_ptr<Node>> m_nodes;
        std::weak_ptr<Node> m_current;
        std::weak_ptr<Node> m_start;
    };
}
