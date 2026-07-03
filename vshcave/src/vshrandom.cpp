#include "vshcave/vshrandom.h"

namespace vshcave
{
	namespace vshrandom
	{
        std::uint32_t resolve_seed(std::optional<std::uint32_t> user_seed)
        {
            if (user_seed.has_value())
                return user_seed.value();

            std::random_device rd;
            return rd();
        }

        std::mt19937 engine(std::uint32_t seed)
        {
            std::mt19937 gen(seed);
            return gen;
        }

        bool bernoulli_distribution(float percertange, std::mt19937& mt)
        {
            std::bernoulli_distribution dis(percertange);

            return dis(mt);
        }

        int random_in_range(int min, int max, std::mt19937& mt)
        {
            std::uniform_int_distribution<int> dist(min, max);
            return dist(mt);
        }

        bool roll_chance(float chance, std::mt19937& mt)
        {
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            return dist(mt) < chance;
        }
	}
}