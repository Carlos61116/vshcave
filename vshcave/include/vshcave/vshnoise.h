#pragma once
#include "vshcave.h"
#include <vector>
#include <random>
namespace vshcave
{

    struct FloodCells
    {
        std::vector<int> component_id;
        std::vector<int> sizes;
    };

	std::vector<vshcave::CellType> generate_cells(const GridInfo& grid_info, float porcentage,std::mt19937& mt);

	std::vector<vshcave::CellType> apply_ca_step(const std::vector<vshcave::CellType>& cells,const GridInfo& grid_info, const CA_Rules& ca_rules);

	int count_neighbor(const std::vector<CellType>& cells, const GridInfo& grid_info, int x, int y);

    int size_bubble(const std::vector<CellType>& cells, const GridInfo& grid_info, int x, int y, std::vector<int>& component_id, int actual_id, int indice);

    FloodCells flood_fill(const std::vector<CellType>& cells, const GridInfo& grid_info);

    std::vector<CellType> clean_map(const GridInfo& grid_info, const FloodCells& flood_cells);



}