#pragma once
#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

class disjoint_sets
{
public:
    using size_type = std::int64_t;
    using index_type = std::int64_t;

    explicit disjoint_sets(index_type n) : 
    P(n),
    m_num_components(n),
    num_elements_in_component(n,1)
    {
        std::iota(P.begin(), P.end(), 0L);
        std::cout<<"Tamanio de arreglo: "<<num_elements_in_component.size()<<std::endl;
    }

    index_type find_root(index_type t) const
    {
        std::vector<index_type> branch;
        branch.emplace_back(t);
        while (t != P[t])
        {
            t = P[t];
            branch.emplace_back(t);
        }
        for (auto u : branch)
            P[u] = t;
        return t;
    }

    void reset(size_type n_vertices)
    {
        P.clear();
        num_elements_in_component.clear();
        P.resize(n_vertices);
        num_elements_in_component.resize(n_vertices,1);
        std::iota(P.begin(), P.end(), 0L);
        m_num_components = n_vertices;
    }

    void reset_parent(int x)
    { 
        P[x] = x; 
        num_elements_in_component[x] = 1;
    }

    void merge(index_type a, index_type b)
    {
        index_type ra = find_root(a);
        index_type root_b = find_root(b);

        index_type rb = set_P(b, ra);


        if (ra != rb)
        {
            --m_num_components;
            num_elements_in_component[ra] += num_elements_in_component[root_b];
        }
    }

    bool are_in_same_connected_component(index_type a, index_type b) const
    {
        return find_root(a) == find_root(b);
    }

    size_type num_components() const { return m_num_components; }

    size_type get_num_elements_in_component(index_type x) const 
    {
        index_type root_component = find_root(x);
         return num_elements_in_component[root_component];
    }

    index_type size() const { return num_elements_in_component.size(); }

    //  std::vector<index_type> parents() { return P; }
    std::vector<index_type>& parents() const { return P; }

    void parent(index_type node) { P[node] = node; }

private:
    // returns ORIGINAL P of x
    index_type set_P(index_type x, index_type p)
    {
        while (x != P[x])
        {
            index_type t = P[x];
            P[x] = p;
            x = t;
        }
        P[x] = p;
        return x;
    }

    mutable std::vector< size_type > num_elements_in_component; 
    mutable std::vector<index_type> P;
    size_type m_num_components;
};
