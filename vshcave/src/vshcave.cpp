#include "vshcave/vshcave.h"
#include "vshcave/vshjson.h"
#include "vshcave/vshrandom.h"
#include "vshcave/vshnoise.h"
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
			rooms.push_back(vshcave::Room{ .room_position = vshcave::RoomPosition{.x = json_info["rooms"][i]["center"]["x"],.y = json_info["rooms"][i]["center"]["y"]},.radius = json_info["rooms"][i]["radius"],.fill_probability = json_info["rooms"][i]["fill_probability"] });
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



int main()
{
	std::cout << "Hello CMake." << std::endl;




	auto dungeon_loaded = vshcave::load_dungeon();

	auto engine = vshcave::vshrandom::engine(dungeon_loaded.master_seed);


	auto cells = vshcave::generate_cells(dungeon_loaded.grid_info, engine);

	print_grid(cells, dungeon_loaded.grid_info);

	std::vector<vshcave::CellType> new_cells{};

	for (std::uint16_t i = 0; i <= dungeon_loaded.ca_rules.iterations; i++)
	{
		new_cells= vshcave::apply_ca_step(cells, dungeon_loaded.grid_info, dungeon_loaded.ca_rules);
	}

	std::cout << "\n";
	std::cout << "\n";

	print_grid(new_cells, dungeon_loaded.grid_info);

	return 0;
}
