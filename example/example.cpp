#include <iostream>
#include <vector>
#include <set>
#include <utility>
#include <boost/graph/adjacency_list.hpp>
#include "./../src/tree_decomposition.hpp"
#include "./../src/nice_tree_decomposition.hpp"
#include "./../src/max_weighted_independent_set.hpp"
#include "./example_helper.hpp"

using namespace boost;
//representation of graph
using Graph = adjacency_list<listS, vecS, undirectedS, property<vertex_name_t, int, property<vertex_index_t, size_t>>, property<edge_index_t, int>>;
using Graph_traits = graph_traits<Graph>;
//descriptor of vertices in Graph
using Graph_vertex = Graph_traits::vertex_descriptor;
//container representing bags of decomposition
using Decomposition_bags_map = std::map<Graph_vertex, std::set<Graph_vertex>>;
//property map wrapping the Decomposition_bags_map
using Decomposition_bags = associative_property_map<Decomposition_bags_map>;
//container representing weights of vertices of Graph
using Weights_map = std::map<Graph_vertex, unsigned int>;
//property map wrapping the Weights_map
using Weights = associative_property_map<Weights_map>;
//container representing colors of vertices of Graph (used to distinguish between vertices belonging to maximum independent set)
using Colors_map = std::map<Graph_vertex, int>;
//property map wrapping the Colors_map
using Colors = associative_property_map<Colors_map>;

//set this to false, if you don't want to save graphs into PNG - instead they will be output into stdout
const bool SAVE_GRAPHS = true;

int main() {
    //=== CONSTRUCTING GRAPH
    //graph g with 15 vertices indexed from 0 to 14
	Graph g = Graph(15);
    add_edge(0, 1, g);
    add_edge(0, 2, g);
    add_edge(1, 2, g);
    for (int i = 3; i < 6; i++) {
        for (int j = i+1; j < 7; j++) add_edge(i, j, g);
    }
    add_edge(6, 7, g);
    for (int i = 7; i < 10; i++) {
        for (int j = i+1; j < 11; j++) add_edge(i, j, g);
    }
    add_edge(6, 11, g);
    for (int i = 11; i < 14; i++) {
        for (int j = i+1; j < 15; j++) add_edge(i, j, g);
    }

    if (SAVE_GRAPHS) save_graph("graph.dot", g);
    else stdin_graph(g, "g");

    //=== TREE DECOMPOSITION
    //graph in which decomposition of g will be stored
    Graph decomposition;
    //std::map that will represent bags of tree decomposition
    Decomposition_bags_map bags_map;
    //wrapping bags_map in Property Map
    Decomposition_bags bags_property_map(bags_map);
    //constructing the decomposition with width not exceeding 3*0 + 4
    boost::tree_decomposition(g, decomposition, bags_property_map, 0);

    if (SAVE_GRAPHS) save_graph("decomposition.dot", decomposition, bags_property_map);
    else stdin_graph(decomposition, "decomposition", bags_property_map);


    //=== NICE DECOMPOSITION
    //graph in which nice decomposition of decomposition will be stored
    Graph nice_decomposition;
    //std::map that will represent bags of tree decomposition
    Decomposition_bags_map nice_bags_map;
    Decomposition_bags nice_bags_property_map(nice_bags_map);
    //root of the nice decomposition
    Graph_vertex root;
    //constructing the nice decomposition
    root = boost::nice_tree_decomposition(decomposition, bags_property_map, nice_decomposition, nice_bags_property_map);

    if (SAVE_GRAPHS) save_graph("nice_decomposition.dot", nice_decomposition, nice_bags_property_map);
    else stdin_graph(nice_decomposition, "nice decomposition", nice_bags_property_map);


    //=== MAX WEIGHTED INDEPENDENT SET
    //defines weights for each vertex of g
    Weights_map weights_map;
    weights_map[0] = 4;
    weights_map[1] = 8;
    weights_map[2] = 3;
    weights_map[3] = 1;
    weights_map[4] = 2;
    weights_map[5] = 3;
    weights_map[6] = 4;
    weights_map[7] = 5;
    weights_map[8] = 6;
    weights_map[9] = 2;
    weights_map[10] = 5;
    weights_map[11] = 10;
    weights_map[12] = 6;
    weights_map[13] = 2;
    weights_map[14] = 4;
    //wrapping weights_map in property map
    Weights weights = Weights(weights_map);
    //defines if vertex is in max weighted independent set
    Colors_map colors_map;
    Colors colors = Colors(colors_map);
    //total weight of independent set
    unsigned int total;
    total = boost::max_weighted_independent_set(g, nice_decomposition, nice_bags_property_map, root, weights, colors);

    print_weight(g, colors, total);

    return 0;
}
