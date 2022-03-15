#pragma once

#include <memory>
#include <string>
#include <vector>

namespace graph {
    enum class Direction
    {
        Left,
        Right,
        Up,
        Down
    };

    struct Position
    {
        int x;
        int y;

        Position(const int t_x, const int t_y);
    };

    struct Rectangle
    {
        int min_x;
        int min_y;
        int max_x;
        int max_y;

        Rectangle(const int t_min_x, const int t_min_y, const int t_max_x, const int t_max_y);
    };

    class Node
    {
    public:
        Node(Position pos,
             bool visited,
             const std::weak_ptr<Node> left,
             const std::weak_ptr<Node> right,
             const std::weak_ptr<Node> up,
             const std::weak_ptr<Node> down) noexcept;
        Node(Position pos, bool visited) noexcept;
        Node(bool visited) noexcept;
        Node() noexcept;

    public:
        bool deadendCheck() noexcept;
        std::weak_ptr<Node> getNode(const Direction direction) const noexcept;
        void setNode(const Direction direction, const std::weak_ptr<Node> self, const std::weak_ptr<Node> node) noexcept;

    public:
        Position m_pos;
        bool m_visited;

    private:
        bool m_deadend;
        std::weak_ptr<Node> m_left;
        std::weak_ptr<Node> m_right;
        std::weak_ptr<Node> m_up;
        std::weak_ptr<Node> m_down;
    };

    class Graph
    {
    public:
        Graph(std::shared_ptr<Node> start) noexcept;

    public:
        void createNodeAt(const Direction direction) noexcept;
        std::weak_ptr<Node> getCurrent() const noexcept;
        size_t getNodeCount() const noexcept;
        Rectangle getRect() const noexcept;
        void go(const Direction direction);
        bool isExplored() const noexcept;
        void normalizeRect() noexcept;
        std::string printMap(const char start) const noexcept;

    private:
        void updateRect(const Position pos) noexcept;

    private:
        Rectangle m_rect;
        std::vector<std::shared_ptr<Node>> m_nodes;
        std::weak_ptr<Node> m_current;
        std::weak_ptr<Node> m_previous;
        std::weak_ptr<Node> m_start;
    };
}
