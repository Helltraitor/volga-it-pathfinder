#include "graph.hpp"

#include <stdexcept>
#include <tuple>

namespace graph
{
    /* Node */
    Node::Node(Position pos,
               bool visited,
               std::weak_ptr<Node> left,
               std::weak_ptr<Node> right,
               std::weak_ptr<Node> up,
               std::weak_ptr<Node> down) noexcept
        : m_pos(pos),
          m_visited(visited),
          m_deadend(false),
          m_left(left),
          m_right(right),
          m_up(up),
          m_down(right)
    {
        if (!m_left.expired())
        {
            m_left.lock()->m_pos = std::make_pair(m_pos.first - 1, m_pos.second);
        }
        if (!m_right.expired())
        {
            m_right.lock()->m_pos = std::make_pair(m_pos.first + 1, m_pos.second);
        }
        if (!m_up.expired())
        {
            m_up.lock()->m_pos = std::make_pair(m_pos.first, m_pos.second + 1);
        }
        if (!m_down.expired())
        {
            m_down.lock()->m_pos = std::make_pair(m_pos.first, m_pos.second - 1);
        }
    }

    Node::Node(const Position pos, bool visited) noexcept
        : Node::Node(pos,
                     visited,
                     std::weak_ptr<Node>(),
                     std::weak_ptr<Node>(),
                     std::weak_ptr<Node>(),
                     std::weak_ptr<Node>()) {}

    Node::Node(bool visited) noexcept : Node::Node(std::make_pair(0, 0), visited) {}

    Node::Node() noexcept : Node::Node(false) {}

    bool Node::deadendCheck() noexcept
    {
        if (m_deadend)
        {
            return true;
        }
        int exit_count =
            static_cast<int>(!m_left.expired() && !m_left.lock()->m_deadend) +
            static_cast<int>(!m_right.expired() && !m_right.lock()->m_deadend) +
            static_cast<int>(!m_up.expired() && !m_up.lock()->m_deadend) +
            static_cast<int>(!m_down.expired() && !m_down.lock()->m_deadend);
        if (exit_count < 2)
        {
            m_deadend = true;
        }
        return m_deadend;
    }

    std::weak_ptr<Node> Node::getNode(const Direction direction) const noexcept
    {
        switch (direction)
        {
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
        if (node.expired() || self.expired() || &(*self.lock()) != this)
        {
            return;
        }

        switch (direction)
        {
        case Direction::Left:
            m_left = node;
            m_left.lock()->m_pos = std::make_pair(m_pos.first - 1, m_pos.second);
            m_left.lock()->m_right = self;
            break;
        case Direction::Right:
            m_right = node;
            m_right.lock()->m_pos = std::make_pair(m_pos.first + 1, m_pos.second);
            m_right.lock()->m_left = self;
            break;
        case Direction::Up:
            m_up = node;
            m_up.lock()->m_pos = std::make_pair(m_pos.first, m_pos.second + 1);
            m_up.lock()->m_down = self;
            break;
        case Direction::Down:
            m_down = node;
            m_down.lock()->m_pos = std::make_pair(m_pos.first, m_pos.second - 1);
            m_down.lock()->m_up = self;
            break;
        }
    }

    /* Graph */
    Graph::Graph(std::shared_ptr<Node> start) noexcept
        : m_start(start),
          m_current(start)
    {
        m_nodes.push_back(start);
        m_rect = std::make_pair(std::make_pair(0, 0), std::make_pair(0, 0));
    }

    void Graph::createNodeAt(const Direction direction) noexcept
    {
        auto pos = m_current.lock()->m_pos;
        switch (direction)
        {
        case Direction::Left:
            pos.first -= 1;
            break;
        case Direction::Right:
            pos.first += 1;
            break;
        case Direction::Up:
            pos.second += 1;
            break;
        case Direction::Down:
            pos.second -= 1;
            break;
        }

        std::weak_ptr<Node> target;
        for (auto node : m_nodes)
        {
            if (pos == node->m_pos)
            {
                target = node;
            }
        }
        if (target.expired())
        {
            auto node = std::make_shared<Node>();
            m_nodes.push_back(node);
            target = node;
        }
        m_current.lock()->setNode(direction, m_current, target);
        updateRect(pos);
    }

    std::weak_ptr<Node> Graph::getCurrent() const noexcept
    {
        return m_current;
    }

    Rect Graph::getRect() const noexcept
    {
        return m_rect;
    }

    void Graph::go(const Direction direction)
    {
        if (m_current.expired())
        {
            throw std::runtime_error(
                "Current node is expired or equals nullptr_t. This occurred because this node wasn't updated");
        }

        m_previous = m_current;
        m_current = m_current.lock()->getNode(direction);
        m_current.lock()->m_visited = true;
    }

    void Graph::normalizeRect() noexcept
    {
        int delta_x = m_rect.second.first - m_rect.first.first;
        int delta_y = m_rect.second.second - m_rect.first.second;

        if (delta_x == 0 && delta_y == 0)
        {
            return;
        }

        m_rect.first.first += delta_x;
        m_rect.first.second += delta_y;
        m_rect.second.first += delta_x;
        m_rect.second.second += delta_y;

        for (auto node : m_nodes)
        {
            node->m_pos.first += delta_x;
            node->m_pos.second += delta_y;
        }
    }

    std::string Graph::printMap(const char start) const noexcept
    {
        char map[10][10] = {};
        for (size_t index = 0; index < 100; index++)
        {
            *(*map + index) = '?';
        }

        for (auto node : m_nodes)
        {
            auto y = node->m_pos.second;
            auto x = node->m_pos.first;
            map[y][x] = '.';

            if (!node->m_visited)
            {
                continue;
            }

            if (x > 0 && node->getNode(Direction::Left).expired())
            {
                map[y][x - 1] = '#';
            }
            if (x < 9 && node->getNode(Direction::Right).expired())
            {
                map[y][x + 1] = '#';
            }
            if (y < 9 && node->getNode(Direction::Up).expired())
            {
                map[y + 1][x] = '#';
            }
            if (y > 0 && node->getNode(Direction::Down).expired())
            {
                map[y - 1][x] = '#';
            }
        }
        map[m_start.lock()->m_pos.second][m_start.lock()->m_pos.first] = int(start);

        std::string buffer;
        for (int y = 9; y > -1; y--)
        {
            for (int x = 0; x < 10; x++)
            {
                buffer.push_back(map[y][x]);
            }
            buffer.push_back('\n');
        }
        return buffer;
    }

    /// <summary>
    /// Takes position and set max and min values in rect. Rect has this meaning:
    /// [min x, min y; max x, max y]
    /// And it's being used for map normalization after Ivan and Elena meeting.
    /// </summary>
    /// <param name="pos"></param>
    /// <returns></returns>
    void Graph::updateRect(const Position pos) noexcept
    {
        if (m_rect.first.first > pos.first)
        {
            m_rect.first.first = pos.first;
        }
        else if (m_rect.second.first < pos.first)
        {
            m_rect.second.first = pos.first;
        }

        if (m_rect.first.second > pos.second)
        {
            m_rect.first.second = pos.second;
        }
        else if (m_rect.second.second < pos.second)
        {
            m_rect.second.second = pos.second;
        }
    }
}
