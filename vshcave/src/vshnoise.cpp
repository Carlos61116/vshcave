#include "vshcave/vshnoise.h"
#include "vshcave/vshrandom.h"
#include <queue>
#include <algorithm>
namespace vshcave
{

	std::vector<vshcave::CellType> generate_cells(const GridInfo& grid_info, float porcentage, std::mt19937& mt)
	{
		std::vector<vshcave::CellType> cells_generated{};
		for (int i = 0; i < grid_info.height; i++)
		{
			for (int e = 0; e < grid_info.width; e++)
			{
				cells_generated.push_back(vshcave::vshrandom::bernoulli_distribution(porcentage, mt) ? vshcave::CellType::WALL : vshcave::CellType::FLOOR);
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

	int size_bubble(const std::vector<CellType>& cells, const GridInfo& grid_info, int x, int y, std::vector<int>& component_id, int actual_id, int indice)
	{
		std::queue<std::pair<int, int>> cola;
		cola.push({ x, y });
		component_id[indice] = actual_id;

		int contador = 0;
		while (!cola.empty())
		{
			auto [cx, cy] = cola.front();
			cola.pop();
			contador++;

			const std::pair<int, int> directions[] =
			{
				{ 0, -1 },
				{ 0,  1 },
				{-1,  0 },
				{ 1,  0 }
			};

			for (auto [dx, dy] : directions)
			{
				int nx = cx + dx;
				int ny = cy + dy;
				int nindice = ny * grid_info.width + nx;


				if (nx >= 0 && nx < grid_info.width && ny >= 0 && ny < grid_info.height && cells[nindice] == vshcave::CellType::FLOOR && component_id[nindice] == -1)
				{
					component_id[nindice] = actual_id;
					cola.push(std::pair<int, int>{nx, ny});
				}
			}
		}
		return contador;
	}

	FloodCells flood_fill(const std::vector<CellType>& cells, const GridInfo& grid_info)
	{
		std::vector<int> component_id(cells.size(), -1);
		int actual_id{ 0 };
		std::vector<int> sizes{};
		for (int y = 0; y < grid_info.height; y++)
		{
			for (int x = 0; x < grid_info.width; x++)
			{
				int indice = y * grid_info.width + x;
				if (cells[indice] == vshcave::CellType::FLOOR && component_id[indice] == -1)
				{
					int size = vshcave::size_bubble(cells,grid_info,x,y,component_id,actual_id,indice);
					sizes.push_back(size);
					actual_id++;
				}
			}
		}

		return vshcave::FloodCells{ .component_id = component_id,.sizes = sizes };
	}

	std::vector<CellType> clean_map(const GridInfo& grid_info, const FloodCells& flood_cells)
	{
		std::vector<CellType> new_cells(flood_cells.component_id.size());
		auto it = std::max_element(flood_cells.sizes.begin(), flood_cells.sizes.end());
		int valor_maximo = *it;
		int umbr = 0.25 * valor_maximo;
		std::vector<bool> bubble_remaining(flood_cells.sizes.size());

		for (size_t i = 0; i < flood_cells.sizes.size(); i++)
		{
			bubble_remaining[i] = flood_cells.sizes[i] >= umbr;
		}


		for (int y = 0; y < grid_info.height; y++)
		{
			for (int x = 0; x < grid_info.width; x++)
			{
				int indice = y * grid_info.width + x;
				int index = flood_cells.component_id[indice];
				if (index == -1) {	
					new_cells[indice] = vshcave::CellType::WALL;
				}
				else
				{
					new_cells[indice] = bubble_remaining[index] ? vshcave::CellType::FLOOR : vshcave::CellType::WALL;
				}
			}
		}

		return new_cells;
	}

	
	
}