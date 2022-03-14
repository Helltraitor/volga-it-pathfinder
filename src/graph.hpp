#pragma once

#include <memory>
#include <string>
#include <vector>

namespace graph {
    using Position = std::pair<int, int>;

    using Rect = std::pair<Position, Position>;

    enum class Direction
    {
        Left,
        Right,
        Up,
        Down
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
        Rect getRect() const noexcept;
        void go(const Direction direction);
        void normalizeRect() noexcept;
        std::string printMap(const char start) const noexcept;

    private:
        void updateRect(const Position pos) noexcept;

    private:
        Rect m_rect;
        std::vector<std::shared_ptr<Node>> m_nodes;
        std::weak_ptr<Node> m_current;
        std::weak_ptr<Node> m_previous;
        std::weak_ptr<Node> m_start;
    };
}
