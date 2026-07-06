#include "vshcave/vshnoise.h"
#include "vshcave/vshrandom.h"
#include <queue>
#include <algorithm>
#include <cmath>

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
		int umbr = 6;
		std::vector<bool> bubble_remaining = decide_bubbles_survivors(flood_cells.sizes,umbr);

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

	std::vector<bool> decide_bubbles_survivors(const std::vector<int>& sizes, float umb)
	{	
		
		std::vector<bool> bubble_remaining(sizes.size());

		for (size_t i = 0; i < sizes.size(); i++)
		{
			bubble_remaining[i] = sizes[i] >= umb;
		}

		return bubble_remaining;
	}

	std::vector<vshcave::BubbleCenter> bubble_center(const FloodCells& flood_cells, const GridInfo& grid_info, const std::vector<bool>& bubble_remaining)
	{
		std::vector<vshcave::BubbleCenter> centers{};
		std::vector<long> sum_x(flood_cells.sizes.size(), 0);
		std::vector<long> sum_y(flood_cells.sizes.size(), 0);

		for (int y = 0; y < grid_info.height; y++)	
		{
			for (int x = 0; x < grid_info.width; x++)
			{
				int indice = y * grid_info.width + x;
				int index = flood_cells.component_id[indice];
				if (index != -1)
				{
					sum_x[index] += x;
					sum_y[index] += y;
				}
			}

		}

		for (size_t i = 0; i < flood_cells.sizes.size(); i++)
		{
			if (!bubble_remaining[i]) continue;

			float centro_x = (float)sum_x[i] / flood_cells.sizes[i];
			float centro_y = (float)sum_y[i] / flood_cells.sizes[i];


			centers.push_back(vshcave::BubbleCenter{.x = centro_x,.y = centro_y,.component_id = static_cast<int>(i)});
		}

		return centers;

	}

	template<typename T>
	float distance_squared(const T& a, const T& b)
	{
		return (a.x - b.x)* (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
	}

	std::vector<PrimDistances> prim_calculator(const std::vector<BubbleCenter>& centers)
	{
		if (centers.size() < 2) return {};

		std::vector<bool> in_tree(centers.size(), false);
		std::vector<float> best_distance(centers.size(), std::numeric_limits<float>::max());
		std::vector<int> best_source(centers.size(), -1);
		std::vector<PrimDistances> prim_distances{};

		// --- Inicialización: el árbol solo contiene la bolsa 0, una única vez ---
		in_tree[0] = true;
		for (size_t i = 1; i < centers.size(); i++)
		{
			best_distance[i] = distance_squared(centers[0], centers[i]);
			best_source[i] = 0;
		}

		// --- El algoritmo: se repite una vez por cada bolsa que falta por añadir ---
		for (size_t paso = 0; paso < centers.size() - 1; paso++)
		{
			// Loop A: buscar, de entre las que NO están en el árbol, la de menor best_distance
			int ganador = -1;
			float menor_distancia = std::numeric_limits<float>::max();

			for (size_t i = 0; i < centers.size(); i++)
			{
				if (!in_tree[i] && best_distance[i] < menor_distancia)
				{
					menor_distancia = best_distance[i];
					ganador = static_cast<int>(i);
				}
			}

			// Marcar al ganador como parte del árbol, y guardar el pasillo (una sola vez por paso)
			in_tree[ganador] = true;
			prim_distances.push_back(vshcave::PrimDistances{
				.from = centers[best_source[ganador]].component_id,
				.to = centers[ganador].component_id
				});

			// Loop B: refrescar las distancias de las que siguen fuera, usando al ganador recién añadido
			for (size_t j = 0; j < centers.size(); j++)
			{
				if (!in_tree[j])
				{
					float distancia_al_ganador = distance_squared(centers[ganador], centers[j]);
					if (distancia_al_ganador < best_distance[j])
					{
						best_distance[j] = distancia_al_ganador;
						best_source[j] = ganador;
					}
				}
			}
		}

		return prim_distances;
	}

	std::vector<GridPosition> get_bubble_cells(const std::vector<int>& component_id, const GridInfo& grid_info, int component_objetive)
	{
		std::vector<GridPosition> bubble_position{};
		for (int y = 0; y < grid_info.height; y++)
		{
			for (int x = 0; x < grid_info.width; x++)
			{
				int indice = y * grid_info.width + x;
				if (component_id[indice] == component_objetive) {
					bubble_position.push_back(vshcave::GridPosition{.x=x,.y=y});
				}
			}
		}

		return bubble_position;
	}

	vshcave::ClosestPair closest_pair(const std::vector<GridPosition>& a, const std::vector<GridPosition>& b)
	{
		float best_distance = std::numeric_limits<float>::max();
		GridPosition best_a{};
		GridPosition best_b{};

		for (size_t p_a = 0; p_a < a.size(); p_a++)
		{
			for (size_t p_b = 0; p_b < b.size(); p_b++)
			{
				float distance = distance_squared(a[p_a], b[p_b]);
				if (best_distance > distance)
				{
					best_a = a[p_a];
					best_b = b[p_b];
					best_distance = distance;
				}
			}
		}

		return vshcave::ClosestPair{ .from = best_a, .to = best_b };
	}

	std::vector<GridPosition> bresenham_line(GridPosition from, GridPosition to)
	{

		std::vector<GridPosition> bresenham{};
		int dx = std::abs(to.x - from.x);
		int dy = -(std::abs(to.y - from.y));
		int err = dx + dy;
		int sx = (from.x < to.x) ? +1 : -1;
		int sy = (from.y < to.y) ? +1 : -1;
		vshcave::GridPosition actualPosition = { .x = from.x,.y = from.y };
		while (true)
		{
			bresenham.push_back(actualPosition);
			if (actualPosition.x == to.x && actualPosition.y == to.y) break;

			int e2 = 2 * err;
			if (e2 >= dy)
			{
				err += dy;
				actualPosition.x += sx;
			}
			if (e2 <= dx)
			{
				err += dx;
				actualPosition.y += sy;
			}
		}

		return bresenham;
	}

	void make_up_bresenham(std::vector<CellType>& cell_map, const std::vector<GridPosition>& bresenham, const GridInfo& grid_info, int radium)
	{
		for (size_t i = 0; i < bresenham.size(); i++)
		{
			for (int dy = -radium; dy <= radium; dy++)
			{
				for (int dx = -radium; dx <= radium; dx++)
				{
					int nx = bresenham[i].x + dx;
					int ny = bresenham[i].y + dy;

					if (nx >= 0 && nx < grid_info.width && ny >= 0 && ny < grid_info.height)
					{
						int indice = ny * grid_info.width + nx;
						cell_map[indice] = vshcave::CellType::FLOOR;
					}
				}
			}
		}
	}

	std::vector<int> generate_control_radii(size_t path_length, int min_radius, int max_radius, int control_spacing, std::mt19937& mt)
	{	
		std::vector<int> radi_list{	};
		int amount_radius = (static_cast<int>(path_length) / control_spacing) + 1;
		for (int i = 0; i < amount_radius; i++)
		{
			radi_list.push_back(vshcave::vshrandom::random_in_range(min_radius,max_radius,mt));
		}

		return radi_list;
	}

	std::vector<int> generate_variable_radii(size_t path_length, int min_radius, int max_radius, int control_spacing, std::mt19937& mt)
	{
		std::vector<int> variables;
		std::vector<int> points = vshcave::generate_control_radii(path_length, min_radius, max_radius, control_spacing, mt);

		for (size_t i = 0; i < path_length; i++)
		{
			int control_index = static_cast<int>(i) / control_spacing;

			if (control_index + 1 < static_cast<int>(points.size()))
			{
				float frac = (i % control_spacing) / float(control_spacing);
				float t_suave = frac * frac * (3 - (2 * frac));
				variables.push_back(points[control_index] + (points[control_index + 1] - points[control_index]) * t_suave);
			}
			else
			{
				variables.push_back(points[control_index]);
			}
		}

		return variables;
	}

	void make_up_bresenham_variable(std::vector<CellType>& cell_map, const std::vector<GridPosition>& bresenham, const GridInfo& grid_info, const std::vector<int>& radii)
	{
		for (size_t i = 0; i < bresenham.size(); i++)
		{
			int radio = radii[i];
			for (int dy = -radio; dy <= radio; dy++)
			{
				for (int dx = -radio; dx <= radio; dx++)
				{
					int nx = bresenham[i].x + dx;
					int ny = bresenham[i].y + dy;

					if (nx >= 0 && nx < grid_info.width && ny >= 0 && ny < grid_info.height)
					{
						int indice = ny * grid_info.width + nx;
						cell_map[indice] = vshcave::CellType::FLOOR;
					}
				}
			}
		}
	}
	
}