#include "vshcave/vshnoise.h"
#include "vshcave/vshrandom.h"
namespace vshcave
{

	std::vector<vshcave::CellType> generate_cells(const GridInfo& grid_info, std::mt19937& mt)
	{
		std::vector<vshcave::CellType> cells_generated{};
		for (int i = 0; i < grid_info.height; i++)
		{
			for (int e = 0; e < grid_info.width; e++)
			{
				cells_generated.push_back(vshcave::vshrandom::bernoulli_distribution(0.45, mt) ? vshcave::CellType::WALL : vshcave::CellType::FLOOR);
			}
		}

		return cells_generated;
	}

	int count_neighbor(const std::vector<vshcave::CellType>& cells, const GridInfo& grid_info, int x, int y)
	{
		int cont = 0;
		for (int dy = -1; dy <= 1; dy++)
		{
			for (int dx = -1; dx <= 1; dx++)
			{
				if (dy == 0 && dx == 0) continue;
				int nx = x + dx;
				int ny = y + dy;
				if (nx < 0 || nx >= grid_info.width || ny >= grid_info.height || ny < 0)
				{
					cont++;
				}
				else
				{
					int index = ny * grid_info.width + nx;
					if (cells[index] == vshcave::CellType::WALL)
					{
						cont++;
					}
				}
			}
		}

		return cont;
	}

	std::vector<vshcave::CellType> apply_ca_step(const std::vector<vshcave::CellType>& cells, const GridInfo& grid_info,const CA_Rules& ca_rules)
	{
		std::vector<CellType> cells_applied_ca;
		
		for (int y = 0; y < grid_info.height; y++)
		{
			for (int x = 0; x < grid_info.width; x++)
			{
				int indice = y * grid_info.width + x;

				int cont = vshcave::count_neighbor(cells, grid_info, x, y);
				CellType newCellType{};
				bool is_wall = cells[indice] == vshcave::CellType::WALL;
				if (is_wall)
				{
					newCellType = (cont >= ca_rules.survival_threshold) ? vshcave::CellType::WALL : vshcave::CellType::FLOOR;
				}
				else
				{
					newCellType = (cont >= ca_rules.birth_threshold) ? vshcave::CellType::WALL : vshcave::CellType::FLOOR;
				}
				cells_applied_ca.push_back(newCellType);
			}
		}

		return cells_applied_ca;
	}

	
}