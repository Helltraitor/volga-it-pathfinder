#pragma once

#include "graph.hpp"

#include <memory>

namespace pathfinder
{
	enum class Strategy
	{
		None,
		GoLeft,
		GoRight,
		GoUp,
		GoDown
	};

	class Pathfinder
	{
	public:
		Pathfinder(const std::shared_ptr<Fairyland> world, const Character t_char, const std::shared_ptr<graph::Graph>) noexcept;

	public:
		graph::Direction getAdvice() noexcept;
		void updateNode() noexcept;

	private:
		

	private:
		std::shared_ptr<Fairyland> m_world;
		std::shared_ptr<graph::Graph> m_graph;
		Character m_char;
		Strategy m_cur_strategy;
	};
}