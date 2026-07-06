#include "vshcave/vshcave.h"
#include "vshcave/vshjson.h"
#include "vshcave/vshrandom.h"
#include "vshcave/vshnoise.h"
#include <iomanip>
namespace vshcave
{
	vshcave::Dungeon load_dungeon()
	{
		auto json_info = vshcave::file::read_json("input");

		std::optional<std::uint32_t> seed_from_json = json_info["master_seed"].is_null()
			? std::nullopt
			: std::optional<std::uint32_t>(json_info["master_seed"].get<std::uint32_t>());

		std::vector<vshcave::Room> rooms;

		std::uint32_t resolved_seed = vshcave::vshrandom::resolve_seed(seed_from_json);

		for (size_t i = 0; i < json_info["rooms"].size(); i++)
		{
			rooms.push_back(vshcave::Room{ .room_position = vshcave::GridPosition{.x = json_info["rooms"][i]["center"]["x"],.y = json_info["rooms"][i]["center"]["y"]},.radius = json_info["rooms"][i]["radius"],.fill_probability = json_info["rooms"][i]["fill_probability"] });
		}
		return vshcave::Dungeon{ .format_version = json_info["format_version"],.master_seed = resolved_seed,.grid_info = vshcave::GridInfo{.width = json_info["grid"]["width"],.height = json_info["grid"]["height"]},.ca_rules = vshcave::CA_Rules{.iterations = json_info["ca_rules"]["iterations"],.birth_threshold = json_info["ca_rules"]["birth_threshold"],.survival_threshold = json_info["ca_rules"]["survival_threshold"]},.background_fill_probability = json_info["background_fill_probability"],.rooms = rooms};
	}
}

void print_grid(const std::vector<vshcave::CellType>& cells, const vshcave::GridInfo& grid_info)
{
	for (std::uint32_t y = 0; y < grid_info.height; y++)
	{
		for (std::uint32_t x = 0; x < grid_info.width; x++)
		{
			std::uint32_t index = y * grid_info.width + x;
			std::cout << (cells[index] == vshcave::CellType::WALL ? '#' : '.');
		}
		std::cout << '\n';
	}
}



void print_flood_cells(const vshcave::FloodCells& flood, const vshcave::GridInfo& grid_info)
{
	for (int y = 0; y < grid_info.height; y++)
	{
		for (int x = 0; x < grid_info.width; x++)
		{
			int index = y * grid_info.width + x;
			int id = flood.component_id[index];

			if (id == -1)
				std::cout << " . ";
			else
				std::cout << std::setw(2) << id << ' ';
		}
		std::cout << '\n';
	}

	std::cout << "\nTamanos por bolsa:\n";
	for (std::size_t i = 0; i < flood.sizes.size(); i++)
	{
		std::cout << "  bolsa " << i << ": " << flood.sizes[i] << " celdas\n";
	}
}


int main()
{
	auto dungeon_loaded = vshcave::load_dungeon();

	auto engine = vshcave::vshrandom::engine(dungeon_loaded.master_seed);

	std::cout <<"Seed: " << dungeon_loaded.master_seed << "\n";

	auto cells = vshcave::generate_cells(dungeon_loaded.grid_info,dungeon_loaded.background_fill_probability, engine);

	print_grid(cells, dungeon_loaded.grid_info);

	std::vector<vshcave::CellType> new_cells = cells;

	for (std::uint16_t i = 0; i < dungeon_loaded.ca_rules.iterations; i++)
	{
		new_cells= vshcave::apply_ca_step(new_cells, dungeon_loaded.grid_info, dungeon_loaded.ca_rules);
	}

	std::cout << "\n";
	std::cout << "\n";

	print_grid(new_cells, dungeon_loaded.grid_info);

	auto flood = vshcave::flood_fill(new_cells, dungeon_loaded.grid_info);

	std::cout << "\n";
	std::cout << "\n";

	print_flood_cells(flood, dungeon_loaded.grid_info);
	
	std::cout << "\n";
	std::cout << "\n";
	auto it = std::max_element(flood.sizes.begin(), flood.sizes.end());
	int valor_maximo = *it;
	int umbr = 6;   
	auto bubble_remaining = vshcave::decide_bubbles_survivors(flood.sizes, umbr);

	auto final_map = vshcave::clean_map(dungeon_loaded.grid_info, flood);
	print_grid(final_map, dungeon_loaded.grid_info);


	auto centers = vshcave::bubble_center(flood, dungeon_loaded.grid_info, bubble_remaining);


	auto prim_result = vshcave::prim_calculator(centers);

	for (const auto& edge : prim_result)
	{
		auto cells_a = vshcave::get_bubble_cells(flood.component_id, dungeon_loaded.grid_info, edge.from);
		auto cells_b = vshcave::get_bubble_cells(flood.component_id, dungeon_loaded.grid_info, edge.to);

		auto pair = vshcave::closest_pair(cells_a, cells_b);
		auto breseham = vshcave::bresenham_line(pair.from, pair.to);
		std::cout << "Pasillo sala " << edge.from << " -> sala " << edge.to
			<< ": (" << pair.from.x << "," << pair.from.y << ") a ("
			<< pair.to.x << "," << pair.to.y << ")\n";

		
		vshcave::make_up_bresenham(final_map, breseham,dungeon_loaded.grid_info,1);
		

	}

	std::cout << "\n";
	std::cout << "\n";

	print_grid(final_map, dungeon_loaded.grid_info);



	return 0;
}
