#pragma once
#include "NumberTheory.hpp"
#include <random>
// Utilizamos esta estructura para hacer una funcion de hash para un vector.
template <class T>
struct polynomial_hash
{
    using ull = unsigned long long;
    using ll = long long;

    ull operator()(std::vector<T> const& pieces) const
    {
        ll x = 31;
        ll p = 1000000007;
        ll result = 0;

        for (ll v = 0; v < pieces.size(); ++v)
        {
            ll color = 0;
            if (pieces[v] == 'B')
                color = 2;
            else if (pieces[v] == 'W')
                color = 1;

            result *= x;
            result += color;
            reduce_mod(result, p);
        }

        return result;
    }
};

// Utilizamos para comprobar de forma rápida si hay Ko en el archivo BoardGame.
class Zebrist_Hash
{
public:
    explicit Zebrist_Hash(int lenght)
        : hash_array(lenght), gen(std::random_device{}())
    {
        for (int i = 0; i < lenght; ++i)
            hash_array[i] = gen();
    }

    std::uint64_t get_hash_value(int pos) const { return hash_array[pos]; }

    void create_hash_value() { hash_array.push_back(gen()); }

    int size() const { return hash_array.size(); }

private:
    std::mt19937_64 gen;
    std::vector<std::uint64_t> hash_array;
};