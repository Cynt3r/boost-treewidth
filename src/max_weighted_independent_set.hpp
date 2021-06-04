#ifndef BOOST_GRAPH_MAX_WEIGHTED_INDEPENDENT_SET_HPP
#define BOOST_GRAPH_MAX_WEIGHTED_INDEPENDENT_SET_HPP

#include <algorithm>
#include <list>
#include <map>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <boost/functional/hash.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/subgraph.hpp>

/**
 * @file max_weighted_independent_set.hpp
 *
 * The \p max_weighted_independent_set() finds the maximum weighted set based on provided nice tree decomposition and stores it in provided property map \p colors. The total weight of the found set is the return value of the function.
 */

namespace boost {
    #ifndef DOXYGEN_SHOULD_SKIP_THIS
    namespace detail {
        /**
         * Finds difference between two collections
         * @tparam T    Type of the collection
         * @param left  First collection
         * @param right Second collection
         * @return Element from (left \ right) or (right \ left) - there is always exactly one
         */
        template <class T>
        typename T::value_type get_diff(T & left, T & right) {
            using Container_value_type = typename T::value_type;

            for (const Container_value_type curr : left) {
                if (std::find(right.begin(), right.end(), curr) == right.end()) return curr;
            }

            //will never reach here
            return *left.begin();
        }

        /**
         * Checks if vertex v is adjacent to any vertex from set s in graph g 
         * @tparam Graph    Type of the graph
         * @param g         Graph provided by the user
         * @param s         Set of vertices from graph g
         * @param v         Vertex for which we are checking adjacency
         * @return True if v is adjacent to any vertex from s, otherwise false
         */
        template <class Graph>
        bool is_adjacent(Graph & g,
                         std::set<typename graph_traits<Graph>::vertex_descriptor> & s,
                         typename graph_traits<Graph>::vertex_descriptor v) {
            //vertex descriptor that is used to address vertices in graph
            using Graph_vertex = typename graph_traits<Graph>::vertex_descriptor;

            for (const Graph_vertex curr : s) {
                if (edge(curr, v, g).second) return true;
            }

            return false;
        }

        /**
         * Rewrites found independent set into a property map provided by the user
         * @tparam Graph            Type of the graph
         * @tparam Colors           Type of the property map representing the independent set
         * @param g                 Graph provided by the user
         * @param independent_set   Set of vertices that belong to the independent set
         * @param colors            Property map (colormap) representing independent set (white vertices belong to the set, black not)
         */
        template <class Graph, class Colors>
        void write_colors(Graph & g,
                          std::set<typename graph_traits<Graph>::vertex_descriptor> & independent_set,
                          Colors & colors) {
            //vertex descriptor that is used to address vertices in graph
            using Graph_vertex = typename graph_traits<Graph>::vertex_descriptor;
            //type of value of property map
            using Colors_value_type = typename property_traits<Colors>::value_type;

            for (const Graph_vertex v : make_iterator_range(vertices(g))) colors[v] = color_traits<Colors_value_type>::black();
            for (const Graph_vertex v : independent_set) colors[v] = color_traits<Colors_value_type>::white();
        }

        /**
         * Finds all neighbours of t excluding its parent
         * @tparam Decomposition    Type of the decomposition
         * @param d                 Nice tree decomposition of graph g
         * @param t                 Target node of d
         * @param parent            Parent of node t
         * @return List of neighbours of t
         */
        template <class Decomposition>
        std::list<typename graph_traits<Decomposition>::vertex_descriptor>
        get_neighbours(Decomposition & d,
                       typename graph_traits<Decomposition>::vertex_descriptor t,
                       typename graph_traits<Decomposition>::vertex_descriptor parent) {
            //vertex descriptor that is used to address vertices in decomposition graph
            using Decomposition_vertex = typename graph_traits<Decomposition>::vertex_descriptor;

            std::list<Decomposition_vertex> neighbours;
            //get all neighbours of t
            auto neighbours_it = adjacent_vertices(t, d);
            for (const Decomposition_vertex neighbour : make_iterator_range(neighbours_it)) {
                //skipping parent
                if (neighbour != parent) neighbours.push_back(neighbour);
            }

            return neighbours;
        }

        /**
         * Finds independent set using the backlinks stored in memory
         * @tparam Decomposition    Type of the decomposition
         * @tparam Bags             Type of the bags of Decomposition
         * @tparam Memory           Type representing memory of calculate_weight() calls
         * @tparam ISet             Type of set representing the independent set
         * @param d                 Nice tree decomposition of graph g
         * @param bags              Property map representing the bags of d
         * @param curr              Current node of d
         * @param parent            Parent of current node of d
         * @param memory            Memory containing weights and forgotten nodes for each calculate_weight() call
         * @param independent_set   Independent set of vertices of graph g
         */
        template <class Decomposition, class Bags, class Memory, class ISet>
        void get_independent_set(Decomposition & d,
                                 Bags & bags,
                                 typename graph_traits<Decomposition>::vertex_descriptor curr,
                                 typename graph_traits<Decomposition>::vertex_descriptor parent,
                                 Memory & memory,
                                 ISet & independent_set) {
            //vertex descriptor that is used to address vertices in decomposition graph
            using Decomposition_vertex = typename graph_traits<Decomposition>::vertex_descriptor;

            ISet set_diff;
            std::list<Decomposition_vertex> neighbours;

            //if current node is forget node
            if (bags[parent].size() <= bags[curr].size()) {
                std::set_intersection(bags[curr].begin(), bags[curr].end(),
                                      independent_set.begin(), independent_set.end(),
                                      std::inserter(set_diff, set_diff.begin()));
                for (auto v : memory[curr][set_diff].second) independent_set.insert(v);
            }
            neighbours = get_neighbours(d, curr, parent);
            for (Decomposition_vertex neighbour : neighbours) get_independent_set(d, bags, neighbour, curr, memory, independent_set);
        }

        /**
         * Recursive function that calculates total weight of node t and nodes under it
         * @tparam Graph            Type of the graph
         * @tparam Decomposition    Type of the decomposition
         * @tparam Bags             Type of the bags of Decomposition
         * @tparam Weights          Type of property map representing maps
         * @param g                 Graph provided by the user
         * @param d                 Nice tree decomposition of graph g
         * @param bags              Property map representing the bags of d
         * @param weights           Property map representing weights of vertices of graph g
         * @param t                 Currently processed vertex from d
         * @param parent            Parent of vertex t
         * @param s                 Set of vertices of graph g
         * @param memory            Memory of results of previous call of this function stored in 2D map as follows: memory[t][s] = pair(weight, independent set)
         */
        template <class Graph, class Decomposition, class Bags, class Weights>
        void calculate_weight(Graph & g,
                              Decomposition & d,
                              Bags & bags,
                              Weights & weights,
                              typename graph_traits<Decomposition>::vertex_descriptor t,
                              typename graph_traits<Decomposition>::vertex_descriptor parent,
                              std::set<typename graph_traits<Graph>::vertex_descriptor> & s,
                              std::map<
                                  typename graph_traits<Decomposition>::vertex_descriptor,
                                  std::unordered_map<
                                      std::set<typename graph_traits<Graph>::vertex_descriptor>,
                                      std::pair<typename property_traits<Weights>::value_type, std::list<typename graph_traits<Graph>::vertex_descriptor>>,
                                      hash<std::set<typename graph_traits<Graph>::vertex_descriptor>>
                                  >
                              > & memory) {
            //vertex descriptor that is used to address vertices in decomposition graph
            using Decomposition_vertex = typename graph_traits<Decomposition>::vertex_descriptor;
            //type representing weight
            using Weight_type = typename property_traits<Weights>::value_type;
            //vertex descriptor that is used to address vertices in graph
            using Graph_vertex = typename graph_traits<Graph>::vertex_descriptor;

            //checking if we have already solved this exact combination of t and s
            //first dimension
            auto memory_it_d1 = memory.find(t);
            if (memory_it_d1 != memory.end()) {
                //second dimension
                auto memory_it_d2 = memory_it_d1->second.find(s);
                if (memory_it_d2 != memory_it_d1->second.end()) return;
            }

            Weight_type res_weight = 0;
            std::list<Graph_vertex> w;
            std::list<Decomposition_vertex> neighbours = get_neighbours(d, t, parent);

            //leaf node
            if (neighbours.size() == 0) {
                memory[t][s] = make_pair(0, w);
                return;
            }
            //join node
            else if (neighbours.size() == 2) {
                for (const Decomposition_vertex neighbour : neighbours) {
                    calculate_weight(g, d, bags, weights, neighbour, t, s, memory);
                    res_weight += memory[neighbour][s].first;
                }
                for (const Graph_vertex v : s) res_weight -= weights[v];
                memory[t][s] = make_pair(res_weight, w);
                return;
            }
            //introduce node
            else if (neighbours.size() == 1 && (bags[t].size() == bags[neighbours.front()].size() + 1)) {
                Graph_vertex diff = get_diff(bags[t], bags[neighbours.front()]);
                //creating s \ diff
                std::set<Graph_vertex> s_new;
                if (s.find(diff) == s.end()) s_new = s;
                else {
                    for (const Graph_vertex curr : s) {
                        if (curr != diff) s_new.insert(curr);
                    }
                    res_weight = weights[diff];
                }
                calculate_weight(g, d, bags, weights, neighbours.front(), t, s_new, memory);
                res_weight += memory[neighbours.front()][s_new].first;
                memory[t][s] = make_pair(res_weight, w);
                return;
            }
            //forget node
            else if (neighbours.size() == 1 && (bags[t].size() + 1 == bags[neighbours.front()].size())) {
                Graph_vertex diff = get_diff(bags[neighbours.front()], bags[t]);
                //creating s+diff
                std::set<Graph_vertex> s_new = s;
                s_new.insert(diff);
                //check if s+diff is independent - if not, we can return
                if (is_adjacent(g, s, diff)) {
                    calculate_weight(g, d, bags, weights, neighbours.front(), t, s, memory);
                    memory[t][s] = make_pair(memory[neighbours.front()][s].first, w);
                }
                else {
                    calculate_weight(g, d, bags, weights, neighbours.front(), t, s, memory);
                    calculate_weight(g, d, bags, weights, neighbours.front(), t, s_new, memory);

                    if (memory[neighbours.front()][s_new].first > memory[neighbours.front()][s].first) {
                        w.push_back(diff);
                        res_weight = memory[neighbours.front()][s_new].first;
                    }
                    else res_weight = memory[neighbours.front()][s].first;
                    memory[t][s] = make_pair(res_weight, w);
                }
                return;
            }
            else throw std::invalid_argument("Invalid nice tree decomposition");
        }

        /**
         * Validates if decomposition is a tree; if not, std::invalid_argument is thrown
         * @tparam Decomposition    Type of the decomposition
         * @param d                 Nice tree decomposition
         * @throws std::invalid_argument If d is not a tree
         */
        template <class Decomposition>
        void validate_tree(Decomposition & d) {
            //vertex descriptor that is used to address vertices in decomposition graph
            using Decomposition_vertex = typename graph_traits<Decomposition>::vertex_descriptor;

            //|V(d)| - 1 == |E(d)|
            if (num_vertices(d) - 1 != num_edges(d)) throw std::invalid_argument("Invalid nice tree decomposition - decomposition is not a tree");

            //d is connected
            subgraph<Decomposition> sd;
            copy_graph(d, sd);
            std::map<Decomposition_vertex, Decomposition_vertex> c;
            if (connected_components(sd, make_assoc_property_map(c)) != 1) throw std::invalid_argument("Invalid nice tree decomposition - decomposition is not connected");;
        }
    } // namespace detail
    #endif // DOXYGEN_SHOULD_SKIP_THIS

    /**
     * @tparam      Graph           Type of the graph.
     * @tparam      Decomposition   Type of the decomposition.
     * @tparam      Bags            Type of the property map representing bags of \p Decomposition.
     * @tparam      Weights         Type of the property map representing weights of vertices of \p g.
     * @tparam      Colors          Type of the property map representing the maximum weighted set of \p g.
     * @param[in]   g               An undirected graph. The graph type must be a model of VertexListGraph.
     * @param[in]   d               An undirected graph containing the nice tree decomposition of graph \p g. The graph type must be a model of VertexListGraph.
     * @param[in]   bags            The bags property map. The type must be a model of a mutable Readable Property Map. The key type of the map must be a vertex descriptor of \p d. The value type of the map must be a model of Mutable_Container. It also must implement function insert([position/hint iterator], [element]). Container value type must be vertex descriptor of \p g.
     * @param[in]   root            Vertex descriptor of the root of \p d.
     * @param[in]   weights         Property map representing weights of vertices of graph \p g. The type must be a model of a mutable Readable Property Map. The key type of the map must be a vertex descriptor of \p g. The value type of the map must be an unsigned integer.
     * @param[out]  colors          Property map used by the algorithm to represent the maximum weighted set. At the end of algorithm, the white vertices define the maximum weighted set. The type must be a model of a mutable ReadWrite Property Map. The key type of the map must be a vertex descriptor of \p g. The value type of the map must be a model of ColorValue.
     * @throws std::invalid_argument If \p d is not a valid nice tree decomposition
     * @return Total weight of the found maximum weighted set.
     */
    template <class Graph, class Decomposition, class Bags, class Weights, class Colors>
    typename property_traits<Weights>::value_type
    max_weighted_independent_set(Graph & g,
                                 Decomposition & d,
                                 Bags & bags,
                                 typename graph_traits<Decomposition>::vertex_descriptor root,
                                 Weights & weights,
                                 Colors & colors) {
        //vertex descriptor that is used to address vertices in graph
        using Graph_vertex = typename graph_traits<Graph>::vertex_descriptor;
        //vertex descriptor that is used to address vertices in decomposition graph
        using Decomposition_vertex = typename graph_traits<Decomposition>::vertex_descriptor;
        //type of value of property map
        using Bag_value_type = typename property_traits<Bags>::value_type;
        //type of value inside Bag_value_type
        using Bag_inside_type = typename Bag_value_type::value_type;
        //type representing weight
        using Weight_type = typename property_traits<Weights>::value_type;
        //type representing color
        using Colors_type = typename property_traits<Colors>::value_type;

        //Graph must be model of Vertex List Graph
        BOOST_CONCEPT_ASSERT((VertexListGraphConcept<Graph>));
        //Decomposition must be model of Vertex List Graph
        BOOST_CONCEPT_ASSERT((VertexListGraphConcept<Decomposition>));
        //Bags must be Readable Property Map (with Decomposition_vertex as a key)
        BOOST_CONCEPT_ASSERT((ReadablePropertyMapConcept<Bags, Decomposition_vertex>));
        //each bag of Bags container must be a model of Container
        BOOST_CONCEPT_ASSERT((Container<Bag_value_type>));
        //Graph_vertex and Bag_inside_type must be a same type
        BOOST_STATIC_ASSERT((is_same<Graph_vertex, Bag_inside_type>::value));
        //Weights must be Readable Property Map (with Graph_vertex as a key)
        BOOST_CONCEPT_ASSERT((ReadablePropertyMapConcept<Weights, Graph_vertex>));
        //Weight_type must be unsigned integer
        BOOST_CONCEPT_ASSERT((UnsignedIntegerConcept<Weight_type>));
        //Bags must be ReadWrite Property Map (with Graph_vertex as a key)
        BOOST_CONCEPT_ASSERT((ReadWritePropertyMapConcept<Colors, Graph_vertex>));
        //Colors_type must be integer
        BOOST_CONCEPT_ASSERT((IntegerConcept<Colors_type>));

        detail::validate_tree(d);
        //memory of dynamic programming
        std::map<
                Decomposition_vertex,
                std::unordered_map<
                        std::set<Graph_vertex>,
                        std::pair<Weight_type, std::list<Graph_vertex>>,
                        hash<std::set<Graph_vertex>>
                >
        > memory;
        std::set<Graph_vertex> s, independent_set;
        detail::calculate_weight(g, d, bags, weights, root, root, s, memory);
        detail::get_independent_set(d, bags, root, root, memory, independent_set);
        detail::write_colors(g, independent_set, colors);

        return memory[root][s].first;
    }
} // namespace boost
#endif // BOOST_GRAPH_MAX_WEIGHTED_INDEPENDENT_SET_HPP
