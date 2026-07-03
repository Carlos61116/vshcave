#pragma once
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>


namespace vshcave
{
	namespace file
	{
		nlohmann::json read_json(std::string file);

		//void save_vocabulary(const std::vector<vshlang::Languages>& languages, const std::string& output_path);
	}
	
}