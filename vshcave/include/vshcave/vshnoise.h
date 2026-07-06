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

    struct PrimDistances
    {
        int from;
        int to;
    };

    struct ClosestPair
    {
        GridPosition from;
        GridPosition to;
    };


	std::vector<vshcave::CellType> generate_cells(const GridInfo& grid_info, float porcentage,std::mt19937& mt);

	std::vector<vshcave::CellType> apply_ca_step(const std::vector<vshcave::CellType>& cells,const GridInfo& grid_info, const CA_Rules& ca_rules);

	int count_neighbor(const std::vector<CellType>& cells, const GridInfo& grid_info, int x, int y);

    int size_bubble(const std::vector<CellType>& cells, const GridInfo& grid_info, int x, int y, std::vector<int>& component_id, int actual_id, int indice);

    FloodCells flood_fill(const std::vector<CellType>& cells, const GridInfo& grid_info);

    std::vector<CellType> clean_map(const GridInfo& grid_info, const FloodCells& flood_cells);

    std::vector<bool> decide_bubbles_survivors(const std::vector<int>& sizes, float umb);

    std::vector<vshcave::BubbleCenter> bubble_center(const FloodCells& flood_cells, const GridInfo& grid_info, const std::vector<bool>& bubble_remaining);

    template<typename T>
    float distance_squared(const T& a, const T& b);

    std::vector<PrimDistances> prim_calculator(const std::vector<BubbleCenter>& centers);

    std::vector<GridPosition> get_bubble_cells(const std::vector<int>& component_id, const GridInfo& grid_info, int component_objetive);

    vshcave::ClosestPair closest_pair(const std::vector<GridPosition>& a, const std::vector<GridPosition>& b);

    std::vector<GridPosition> bresenham_line(GridPosition from, GridPosition to);

    void make_up_bresenham(std::vector<CellType>& cell_map, const std::vector<GridPosition>& bresenham, const GridInfo& grid_info, int radium);

    std::vector<int> generate_control_radii(size_t path_length, int min_radius, int max_radius, int control_spacing, std::mt19937& mt);

    std::vector<int> generate_variable_radii(size_t path_length, int min_radius, int max_radius, int control_spacing, std::mt19937& mt);

    void make_up_bresenham_variable(std::vector<CellType>& cell_map, const std::vector<GridPosition>& bresenham, const GridInfo& grid_info, const std::vector<int>& radii);


}