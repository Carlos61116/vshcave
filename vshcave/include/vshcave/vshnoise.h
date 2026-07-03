#pragma once
#include "vshcave.h"
#include <vector>
#include <random>
namespace vshcave
{
	std::vector<vshcave::CellType> generate_cells(const GridInfo& grid_info, std::mt19937& mt);

	std::vector<vshcave::CellType> apply_ca_step(const std::vector<vshcave::CellType>& cells,const GridInfo& grid_info, const CA_Rules& ca_rules);

	int count_neighbor(const std::vector<CellType>& cells, const GridInfo& grid_info, int x, int y);

}