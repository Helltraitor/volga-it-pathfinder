#include "graph.hpp"

#include <stdexcept>
#include <tuple>

namespace graph {
    /* Structs */
    Neighbor::Neighbor(const std::weak_ptr<Node> t_node, const Direction t_direction)
    {
        node = t_node;
        direction = t_direction;
    }

    Position::Position(const int t_x, const int t_y)
    {
        x = t_x;
        y = t_y;
    }

    bool operator == (const Position& first, const Position& second)
    {
        return first.x == second.x
            && first.y == second.y;
    }

    Rectangle::Rectangle(const int t_min_x, const int t_min_y, const int t_max_x, const int t_max_y)
    {
        min_x = t_min_x;
        min_y = t_min_y;
        max_x = t_max_x;
        max_y = t_max_y;
    }

    bool operator == (const Rectangle& first, const Rectangle& second)
    {
        return first.min_x == second.min_x
            && first.min_y == second.min_y
            && first.max_x == second.max_x
            && first.max_y == second.max_y;
    }

    Tadpole::Tadpole(const std::vector<Direction> t_route, const std::vector<Position> t_nodes, std::weak_ptr<Node> t_head) noexcept
        : route(t_route), nodes(t_nodes), head(t_head)
    {}

    std::vector<Tadpole> Tadpole::produceTadpole() const noexcept
    {
        std::vector<Tadpole> passages;
        auto position = head.lock()->m_position;
        for (auto& neig : head.lock()->getNeighbors()) {
            if (neig.node.expired()) {
                continue;
            }

            bool visited = false;
            for (auto& node : nodes) {
                if (neig.node.lock()->m_position == node) {
                    visited = true;
                    break;
                }
            }
            if (visited) {
                continue;
            }

            auto tad = Tadpole(*this);
            tad.head = neig.node;
            tad.nodes.push_back(position);
            // Adding neighbor direction because this direction is point
            // on this node realtive to previous. This allows to construct
            // a full path to the head.
            tad.route.push_back(neig.direction);
            passages.push_back(std::move(tad));
        }
        return passages;
    }

    /* Node */
    Node::Node(const Position& pos,
               const bool visited,
               const std::weak_ptr<Node> left,
               const std::weak_ptr<Node> right,
               const std::weak_ptr<Node> up,
               const std::weak_ptr<Node> down) noexcept
        : m_position(pos),
        m_visited(visited),
        m_deadend(false),
        m_left(left),
        m_right(right),
        m_up(up),
        m_down(right)
    {
        if (!m_left.expired()) {
            m_left.lock()->m_position = Position(m_position.x - 1, m_position.y);
        }
        if (!m_right.expired()) {
            m_right.lock()->m_position = Position(m_position.x + 1, m_position.y);
        }
        if (!m_up.expired()) {
            m_up.lock()->m_position = Position(m_position.x, m_position.y + 1);
        }
        if (!m_down.expired()) {
            m_down.lock()->m_position = Position(m_position.x, m_position.y - 1);
        }
    }

    Node::Node(const Position& pos, const bool visited) noexcept
        : Node::Node(pos,
                     visited,
                     std::weak_ptr<Node>(),
                     std::weak_ptr<Node>(),
                     std::weak_ptr<Node>(),
                     std::weak_ptr<Node>())
    {}

    Node::Node(bool visited) noexcept : Node::Node(Position(0, 0), visited) {}

    Node::Node() noexcept : Node::Node(false) {}

    bool Node::deadendCheck() noexcept
    {
        if (m_deadend) {
            return true;
        }
        int exit_count =
            static_cast<int>(!m_left.expired() && !m_left.lock()->m_deadend) +
            static_cast<int>(!m_right.expired() && !m_right.lock()->m_deadend) +
            static_cast<int>(!m_up.expired() && !m_up.lock()->m_deadend) +
            static_cast<int>(!m_down.expired() && !m_down.lock()->m_deadend);
        if (exit_count < 2) {
            m_deadend = true;
        }
        return m_deadend;
    }

    std::vector<Neighbor> Node::getNeighbors() const noexcept
    {
        return {
            Neighbor(m_left, graph::Direction::Left),
            Neighbor(m_right, graph::Direction::Right),
            Neighbor(m_up, graph::Direction::Up),
            Neighbor(m_down, graph::Direction::Down),
        };
    }

    std::weak_ptr<Node> Node::getNode(const Direction direction) const noexcept
    {
        switch (direction) {
            case Direction::Left:
                return m_left;
            case Direction::Right:
                return m_right;
            case Direction::Up:
                return m_up;
            case Direction::Down:
                return m_down;
        }

        // Unreachable
        return std::weak_ptr<Node>();
    }

    void Node::setNode(const Direction direction, const std::weak_ptr<Node> self, const std::weak_ptr<Node> node) noexcept
    {
        if (node.expired() || self.expired() || &(*self.lock()) != this) {
            return;
        }

        switch (direction) {
            case Direction::Left:
                m_left = node;
                m_left.lock()->m_position = Position(m_position.x - 1, m_position.y);
                m_left.lock()->m_right = self;
                break;
            case Direction::Right:
                m_right = node;
                m_right.lock()->m_position = Position(m_position.x + 1, m_position.y);
                m_right.lock()->m_left = self;
                break;
            case Direction::Up:
                m_up = node;
                m_up.lock()->m_position = Position(m_position.x, m_position.y + 1);
                m_up.lock()->m_down = self;
                break;
            case Direction::Down:
                m_down = node;
                m_down.lock()->m_position = Position(m_position.x, m_position.y - 1);
                m_down.lock()->m_up = self;
                break;
        }
    }

    /* Graph */
    Graph::Graph(std::shared_ptr<Node> start) noexcept
        : m_start(start),
        m_current(start),
        m_rectangle(0, 0, 0, 0)
    {
        m_nodes.push_back(start);
    }

    void Graph::createNodeAt(const Direction direction) noexcept
    {
        auto pos = m_current.lock()->m_position;
        switch (direction) {
            case Direction::Left:
                pos.x -= 1;
                break;
            case Direction::Right:
                pos.x += 1;
                break;
            case Direction::Up:
                pos.y += 1;
                break;
            case Direction::Down:
                pos.y -= 1;
                break;
        }

        std::weak_ptr<Node> target;
        for (auto& node : m_nodes) {
            if (pos == node->m_position) {
                target = node;
            }
        }
        if (target.expired()) {
            auto node = std::make_shared<Node>();
            m_nodes.push_back(node);
            target = node;
        }
        m_current.lock()->setNode(direction, m_current, target);
        updateRectangle(pos);
    }

    // Returns nearest unvisited node. Nodes are tooks in (left, right, up, down)
    // order. Non-recursive function - will not occur fatal error.
    std::vector<Direction> Graph::findUnvisitedNode() const noexcept
    {
        std::vector<Tadpole> tads{ Tadpole({}, {}, m_current) };
        while (!tads.empty()) {
            std::vector<Tadpole> processed;
            processed.reserve(tads.size() * 4);
            for (auto& tad : tads) {
                if (!tad.head.lock()->m_visited) {
                    return tad.route;
                }
                auto subtads = tad.produceTadpole();
                processed.insert(processed.end(), subtads.begin(), subtads.end());
            }
            tads = processed;
        }
        return {};
    }

    std::weak_ptr<Node> Graph::getCurrent() const noexcept
    {
        return m_current;
    }

    size_t Graph::getNodeCount() const noexcept
    {
        return m_nodes.size();
    }

    std::weak_ptr<Node> Graph::getPrevious() const noexcept
    {
        return m_previous;
    }

    Rectangle Graph::getRectangle() const noexcept
    {
        return m_rectangle;
    }

    void Graph::go(const Direction direction)
    {
        if (m_current.expired()) {
            throw std::runtime_error(
                "Current node is expired or equals nullptr_t. This occurred because this node wasn't updated");
        }

        m_previous = m_current;
        m_current = m_current.lock()->getNode(direction);
        m_current.lock()->m_visited = true;
    }

    bool Graph::isExplored() const noexcept
    {
        for (auto& node : m_nodes) {
            // All nodes must be updated or some paths will leak
            if (!node->m_visited) {
                return false;
            }
        }
        return true;
    }

    void Graph::normalizeRect() noexcept
    {
        int dif_x = -m_rectangle.min_x;
        int dif_y = -m_rectangle.min_y;

        if (dif_x == 0 && dif_y == 0) {
            return;
        }

        m_rectangle.min_x += dif_x;
        m_rectangle.min_y += dif_y;
        m_rectangle.max_x += dif_x;
        m_rectangle.max_y += dif_y;

        for (auto& node : m_nodes) {
            node->m_position.x += dif_x;
            node->m_position.y += dif_y;
        }
    }

    std::string Graph::printMap(const char start) const noexcept
    {
        char map[10][10] = {};
        memset((char*)map, '?', 100);

        for (auto& node : m_nodes) {
            auto x = node->m_position.x;
            auto y = node->m_position.y;
            map[y][x] = '.';

            if (!node->m_visited) {
                continue;
            }

            if (x > 0 && node->getNode(Direction::Left).expired()) {
                map[y][x - 1] = '#';
            }
            if (x < 9 && node->getNode(Direction::Right).expired()) {
                map[y][x + 1] = '#';
            }
            if (y < 9 && node->getNode(Direction::Up).expired()) {
                map[y + 1][x] = '#';
            }
            if (y > 0 && node->getNode(Direction::Down).expired()) {
                map[y - 1][x] = '#';
            }
        }
        map[m_start.lock()->m_position.y][m_start.lock()->m_position.x] = int(start);

        std::string buffer;
        for (int y = 9; y > -1; --y) {
            for (int x = 0; x < 10; ++x) {
                buffer.push_back(map[y][x]);
            }
            buffer.push_back('\n');
        }
        return buffer;
    }

    /// Takes position and set max and min values in rect. Rect has this meaning:
    /// [min x, min y; max x, max y]
    /// And it's being used for map normalization after Ivan and Elena meeting.
    void Graph::updateRectangle(const Position& pos) noexcept
    {
        if (m_rectangle.min_x > pos.x) {
            m_rectangle.min_x = pos.x;
        }
        else if (m_rectangle.max_x < pos.x) {
            m_rectangle.max_x = pos.x;
        }

        if (m_rectangle.min_y > pos.y) {
            m_rectangle.min_y = pos.y;
        }
        else if (m_rectangle.max_y < pos.y) {
            m_rectangle.max_y = pos.y;
        }
    }
}
