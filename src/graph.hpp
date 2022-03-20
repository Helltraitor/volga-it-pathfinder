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

    struct Neighbor {
        std::weak_ptr<Node> node;
        Direction direction;

        Neighbor(const std::weak_ptr<Node> t_node, const Direction t_direction) noexcept;
    };

    struct Position {
        int x;
        int y;

        Position(const int t_x, const int t_y) noexcept;
        Position at(const Direction direction) const noexcept;
    };

    struct Rectangle {
        int min_x;
        int min_y;
        int max_x;
        int max_y;

        Rectangle(const int t_min_x, const int t_min_y, const int t_max_x, const int t_max_y) noexcept;
    };

    struct Tadpole {
        std::vector<Direction> route;
        std::vector<Position> nodes;
        std::weak_ptr<Node> head;

        Tadpole(const std::vector<Direction> t_route, const std::vector<Position> t_nodes, std::weak_ptr<Node> t_head) noexcept;
        std::vector<Tadpole> produceTadpole() const noexcept;
    };

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
        Node(bool visited) noexcept;
        Node() noexcept;

    public:
        bool deadendCheck() noexcept;
        std::vector<Neighbor> getNeighbors() const noexcept;
        std::weak_ptr<Node> getNode(const Direction direction) const noexcept;
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

    class Graph {
    public:
        Graph(std::shared_ptr<Node> start) noexcept;

    public:
        void createNodeAt(const Direction direction) noexcept;
        std::vector<Direction> findUnvisitedNode() const noexcept;
        std::weak_ptr<Node> getCurrent() const noexcept;
        size_t getNodeCount() const noexcept;
        std::vector<Position> getPassagesPositions() const noexcept;
        std::weak_ptr<Node> getPrevious() const noexcept;
        std::vector<Position> getWallsPositions() const noexcept;
        Rectangle getRectangle() const noexcept;
        void go(const Direction direction);
        bool isExplored() const noexcept;
        bool isIntersectedWith(const Graph& graph) const noexcept;
        void normalizeRect() noexcept;
        void resetDeadendNodes() const noexcept;
        void resetVisitedNodes() const noexcept;
        std::string restoreMap(Graph& graph, const char this_start, const char other_start) noexcept;
        void shiftRect(const int delta_x, const int delta_y) noexcept;

    private:
        std::string drawMap(const Graph& graph, const char this_start, const char other_start) noexcept;
        void updateRectangle(const Position& pos) noexcept;

    private:
        Rectangle m_rectangle;
        std::vector<std::shared_ptr<Node>> m_nodes;
        std::weak_ptr<Node> m_current;
        std::weak_ptr<Node> m_previous;
        std::weak_ptr<Node> m_start;
    };
}
