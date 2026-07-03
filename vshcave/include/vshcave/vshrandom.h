#pragma once
#include <optional>
#include <random>

namespace vshcave
{
	namespace vshrandom
	{
		std::uint32_t resolve_seed(std::optional<std::uint32_t> user_seed);

		std::mt19937 engine(std::uint32_t seed);

		bool bernoulli_distribution(float percertange,std::mt19937& mt);

		int random_in_range(int min, int max, std::mt19937& mt);

		bool roll_chance(float chance, std::mt19937& mt);
	}
}