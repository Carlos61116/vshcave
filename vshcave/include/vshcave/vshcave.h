#pragma once

#include <iostream>
#include <vector>


namespace vshcave
{
	enum class CellType
	{
		FLOOR,
		WALL
	};

	struct GridInfo
	{
		std::uint32_t width;
		std::uint32_t height;
	};

	struct CA_Rules
	{
		std::uint16_t iterations;
		std::uint16_t birth_threshold;
		std::uint16_t survival_threshold;

	};

	struct RoomPosition
	{
		int x;
		int y;
	};

	struct Room
	{
		RoomPosition room_position;
		float radius;
		float fill_probability;
	};
	
	struct Dungeon
	{
		std::uint16_t format_version;
		std::uint32_t master_seed;
		GridInfo grid_info;
		CA_Rules ca_rules;
		float background_fill_probability;
		std::vector<vshcave::Room> rooms;
	};

	vshcave::Dungeon load_dungeon();
}