#include "graph.hpp"

#include <stdexcept>
#include <tuple>

namespace graph {
    /* Structs */
    Neighbor::Neighbor(const std::weak_ptr<Node> t_node, const Direction t_direction) noexcept
        : node(t_node), direction(t_direction)
    {}

    Position::Position(const int t_x, const int t_y) noexcept : x(t_x), y(t_y)
    {}

    Position Position::at(const Direction direction) const noexcept
    {
        switch (direction) {
            case Direction::Left:
                return Position(x - 1, y);
            case Direction::Right:
                return Position(x + 1, y);
            case Direction::Up:
                return Position(x, y + 1);
            default:
                return Position(x, y - 1);
        }
    }

    bool operator == (const Position& first, const Position& second)
    {
        return first.x == second.x
            && first.y == second.y;
    }

    Rectangle::Rectangle(const int t_min_x, const int t_min_y, const int t_max_x, const int t_max_y) noexcept
        : min_x(t_min_x), min_y(t_min_y), max_x(t_max_x), max_y(t_max_y)
    {}

    Tadpole::Tadpole(
        const std::vector<Direction> t_route,
        const std::vector<Position> t_nodes,
        const std::weak_ptr<Node> t_head) noexcept
        : route(t_route), nodes(t_nodes), head(t_head)
    {}

    std::vector<Tadpole> Tadpole::produceTadpole() const noexcept
    {
        std::vector<Tadpole> passages;
        const auto pos = head.lock()->m_position;
        for (const auto& neig : head.lock()->getNeighbors()) {
            if (neig.node.expired()) {
                continue;
            }

            bool visited = false;
            for (const auto& node : nodes) {
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
    Node::Node(const Node& node) noexcept
        : m_position(node.m_position),
        m_visited(node.m_visited),
        m_left(node.m_left),
        m_right(node.m_right),
        m_up(node.m_up),
        m_down(node.m_down),
        m_deadend(node.m_deadend)
    {}

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

    Node::Node(const bool visited) noexcept : Node::Node(Position(0, 0), visited) {}

    Node::Node() noexcept : Node::Node(false) {}

    bool Node::deadendCheck() noexcept
    {
        if (m_deadend) {
            return true;
        }
        if (!m_visited) {
            return false;
        }

        const int exit_count =
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
            default:
                return m_down;
        }
    }

    void Node::resetDeadend() noexcept
    {
        m_deadend = false;
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
        const auto pos = m_current.lock()->m_position.at(direction);

        std::weak_ptr<Node> target;
        for (const auto& node : m_nodes) {
            if (pos == node->m_position) {
                target = node;
            }
        }
        if (target.expired()) {
            const auto node = std::make_shared<Node>(pos, false);
            m_nodes.push_back(node);
            target = node;
        }

        // Link relative to target node
        // This need because target may exists at another existing node
        // and connection relative to this node will not connect target
        // with another nodes.

        auto updated_node = target.lock();
        auto neighbors = updated_node->getNeighbors();

        auto lt_node_expired = neighbors[0].node.expired();
        const auto lt_node_pos = pos.at(Direction::Left);

        auto rt_node_expired = neighbors[1].node.expired();
        const auto rt_node_pos = pos.at(Direction::Right);

        auto up_node_expired = neighbors[2].node.expired();
        const auto up_node_pos = pos.at(Direction::Up);

        auto dn_node_expired = neighbors[3].node.expired();
        const auto dn_node_pos = pos.at(Direction::Down);

        for (const auto& node : m_nodes) {
            if (lt_node_expired && lt_node_pos == node->m_position) {
                updated_node->m_left = node;
                node->m_right = updated_node;
                lt_node_expired = false;
            }
            else if (rt_node_expired && rt_node_pos == node->m_position) {
                updated_node->m_right = node;
                node->m_left = updated_node;
                rt_node_expired = false;
            }
            else if (up_node_expired && up_node_pos == node->m_position) {
                updated_node->m_up = node;
                node->m_down = updated_node;
                up_node_expired = false;
            }
            else if (dn_node_expired && dn_node_pos == node->m_position) {
                updated_node->m_down = node;
                node->m_up = updated_node;
                dn_node_expired = false;
            }
            else if (!(lt_node_expired || rt_node_expired || up_node_expired || dn_node_expired)) {
                // Node updating is over
                break;
            }
        }
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
            for (const auto& tad : tads) {
                if (!tad.head.lock()->m_visited) {
                    return tad.route;
                }
                const auto subtads = tad.produceTadpole();
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

    std::vector<Position> Graph::getPassagesPositions() const noexcept
    {
        std::vector<Position> passages;
        passages.reserve(m_nodes.size());
        for (const auto& node : m_nodes) {
            passages.push_back(node->m_position);
        }
        return passages;
    }

    std::vector<Position> Graph::getWallsPositions() const noexcept
    {
        std::vector<Position> walls;
        walls.reserve(m_nodes.size() * 4);
        for (const auto& node : m_nodes) {
            if (!node->m_visited) {
                continue;
            }

            for (const auto& neig : node->getNeighbors()) {
                if (!neig.node.expired()) {
                    continue;
                }
                walls.push_back(node->m_position.at(neig.direction));
            }
        }
        return walls;
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
        for (const auto& node : m_nodes) {
            // All nodes must be updated  and linked or some paths will leak
            if (!node->m_visited) {
                return false;
            }
        }
        return true;
    }

    bool Graph::isIntersectedWith(const Graph& graph) const noexcept
    {
        const auto t_passages = getPassagesPositions();
        const auto t_walls = getWallsPositions();

        const auto o_passages = graph.getPassagesPositions();
        const auto o_walls = graph.getWallsPositions();

        std::vector<Position> passages;
        passages.reserve(t_passages.size() + o_passages.size());
        passages.insert(passages.end(), t_passages.begin(), t_passages.end());
        passages.insert(passages.end(), o_passages.begin(), o_passages.end());

        std::vector<Position> walls;
        walls.reserve(t_walls.size() + o_walls.size());
        walls.insert(walls.end(), t_walls.begin(), t_walls.end());
        walls.insert(walls.end(), o_walls.begin(), o_walls.end());

        for (const auto& passage : passages) {
            for (const auto& wall : walls) {
                if (passage == wall) {
                    return true;
                }
            }
        }
        return false;
    }

    void Graph::normalizeRect() noexcept
    {
        shiftRect(-m_rectangle.min_x, -m_rectangle.min_y);
    }

    void Graph::resetDeadendNodes() const noexcept
    {
        for (const auto& node : m_nodes) {
            node->resetDeadend();
        }
    }

    void Graph::resetVisitedNodes() const noexcept
    {
        for (const auto& node : m_nodes) {
            node->m_visited = false;
        }
        m_current.lock()->m_visited = true;
    }

    std::string Graph::restoreMap(Graph& graph, const char this_start, const char other_start) noexcept
    {
        // TODO: CHECK RECT MIN MAX VALUE AFTER SHIFT

        normalizeRect();
        graph.normalizeRect();

        const auto this_cn_spot = m_current.lock()->m_position;
        const auto other_cn_spot = graph.m_current.lock()->m_position;

        // Possible spot for centering map
        const auto cn_invariants = {
            this_cn_spot,                                  // Center
            Position(this_cn_spot.x - 1, this_cn_spot.y),  // Left-center
            Position(this_cn_spot.x + 1, this_cn_spot.y),  // Right-center
            Position(this_cn_spot.x, this_cn_spot.y + 1),  // Up-center
            Position(this_cn_spot.x, this_cn_spot.y - 1)   // Down-center
        };
        for (const auto& cn_spot : cn_invariants) {
            normalizeRect();
            graph.normalizeRect();

            const auto delta_x = cn_spot.x - other_cn_spot.x;
            const auto delta_y = cn_spot.y - other_cn_spot.y;

            // If delta more then 0 then move other graph else move this graph
            // that will help align map at (0;0)

            const auto this_delta_x = delta_x < 0 ? -delta_x : 0;
            const auto this_delta_y = delta_y < 0 ? -delta_y : 0;
            const auto other_delta_x = delta_x > 0 ? delta_x : 0;
            const auto other_delta_y = delta_y > 0 ? delta_y : 0;

            shiftRect(this_delta_x, this_delta_y);
            graph.shiftRect(other_delta_x, other_delta_y);

            const auto this_rect = m_rectangle;
            const auto other_rect = graph.m_rectangle;

            // These rects can be out of bounds, in that case connection spot is wrong
            if (this_rect.max_x > 9 ||
                this_rect.max_y > 9 ||
                other_rect.max_x > 9 ||
                other_rect.max_y > 9) {
                continue;
            }

            if (isIntersectedWith(graph)) {
                continue;
            }

            return drawMap(graph, this_start, other_start);
        }
        return std::string();
    }

    void Graph::shiftRect(const int delta_x, const int delta_y) noexcept
    {
        if (delta_x == 0 && delta_y == 0) {
            return;
        }

        m_rectangle.min_x += delta_x;
        m_rectangle.min_y += delta_y;
        m_rectangle.max_x += delta_x;
        m_rectangle.max_y += delta_y;

        for (const auto& node : m_nodes) {
            node->m_position.x += delta_x;
            node->m_position.y += delta_y;
        }
    }

    std::string Graph::drawMap(const Graph& graph, const char this_start, const char other_start) noexcept
    {
        char map[10][10] = {};
        memset((char*)map, '?', 100);

        for (const auto& passage : getPassagesPositions()) {
            map[passage.y][passage.x] = '.';
        }

        for (const auto& passage : graph.getPassagesPositions()) {
            map[passage.y][passage.x] = '.';
        }

        for (const auto& wall : getWallsPositions()) {
            // Walls can be border of the labyrinth that can't be draw in 10x10 map
            if (wall.x > 9 || wall.x < 0 || wall.y > 9 || wall.y < 0) {
                continue;
            }
            map[wall.y][wall.x] = '#';
        }

        for (const auto& wall : graph.getWallsPositions()) {
            if (wall.x > 9 || wall.x < 0 || wall.y > 9 || wall.y < 0) {
                continue;
            }
            map[wall.y][wall.x] = '#';
        }

        const auto this_start_pos = m_start.lock()->m_position;
        map[this_start_pos.y][this_start_pos.x] = this_start;

        const auto other_start_pos = graph.m_start.lock()->m_position;
        map[other_start_pos.y][other_start_pos.x] = other_start;

        std::string sheet;
        for (int y = 9; y > -1; --y) {
            for (int x = 0; x < 10; ++x) {
                sheet.push_back(map[y][x]);
            }
            sheet.push_back('\n');
        }
        return sheet;
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
