#include "vshcave/vshjson.h"


namespace vshcave
{
	namespace file
	{
		nlohmann::json read_json(std::string file)
		{
			std::string path = "./data/" + file + ".json";
			std::ifstream f(path);

			return nlohmann::json::parse(f);
		}

		//void save_vocabulary(const std::vector<vshlang::Languages>& languages, const std::string& output_path);
	}

}