#include "../src/nice_tree_decomposition.hpp"
#include <list>
#include <map>
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/copy.hpp>
#include <boost/test/unit_test.hpp>

using namespace boost;
using Graph = adjacency_list<listS, vecS, undirectedS, property<vertex_name_t, int, property<vertex_index_t, size_t>>, property<edge_index_t, int>>;
using Graph_traits = graph_traits<Graph>;
using Graph_vertex = Graph_traits::vertex_descriptor;
using Graph_vertex_size_type = Graph_traits::vertices_size_type;
using Decomposition_bags_map = std::map<Graph_vertex, std::set<Graph_vertex>>;
using Decomposition_bags = associative_property_map<Decomposition_bags_map>;

struct Fixtures_nd {
    Fixtures_nd() {
        construct_d1();
        construct_d2();
        construct_d3();
        construct_d4();
    }

    ~Fixtures_nd() = default;

    void construct_d1() {
        g1 = Graph(1);

        d1 = Graph(1);

        bm1[0] = {0};
        b1 = Decomposition_bags(bm1);
    }

    void construct_d2() {
        g2 = Graph(8);
        add_edge(0, 1, g2);
        add_edge(0, 2, g2);
        add_edge(1, 2, g2);
        add_edge(2, 3, g2);
        add_edge(2, 4, g2);
        add_edge(3, 4, g2);
        add_edge(4, 5, g2);
        add_edge(4, 6, g2);
        add_edge(4, 7, g2);

        d2 = Graph(3);
        add_edge(0, 1, d2);
        add_edge(1, 2, d2);

        bm2[0] = {0, 1, 2};
        bm2[1] = {2, 3, 4};
        bm2[2] = {4, 5, 6, 7};
        b2 = Decomposition_bags(bm2);
    }

    void construct_d3() {
        g3 = Graph(8);
        add_edge(0, 1, g3);
        add_edge(0, 3, g3);
        add_edge(1, 2, g3);
        add_edge(1, 3, g3);
        add_edge(1, 4, g3);
        add_edge(1, 6, g3);
        add_edge(2, 4, g3);
        add_edge(3, 5, g3);
        add_edge(3, 6, g3);
        add_edge(4, 6, g3);
        add_edge(4, 7, g3);
        add_edge(5, 6, g3);
        add_edge(6, 7, g3);

        d3 = Graph(6);
        add_edge(0, 1, d3);
        add_edge(1, 2, d3);
        add_edge(1, 3, d3);
        add_edge(3, 4, d3);
        add_edge(3, 5, d3);

        bm3[0] = {0, 1, 3};
        bm3[1] = {1, 3, 6};
        bm3[2] = {3, 5, 6};
        bm3[3] = {1, 4, 6};
        bm3[4] = {1, 2, 4};
        bm3[5] = {4, 6, 7};
        b3 = Decomposition_bags(bm3);
    }

    void construct_d4() {
        g4 = Graph(9);
        add_edge(0, 1, g4);
        add_edge(0, 2, g4);
        add_edge(1, 2, g4);
        add_edge(2, 3, g4);
        add_edge(2, 4, g4);
        add_edge(3, 4, g4);
        add_edge(2, 5, g4);
        add_edge(2, 6, g4);
        add_edge(5, 6, g4);
        add_edge(2, 7, g4);
        add_edge(2, 8, g4);
        add_edge(7, 8, g4);

        d4 = Graph(4);
        add_edge(0, 1, d4);
        add_edge(0, 2, d4);
        add_edge(0, 3, d4);

        bm4[0] = {0, 1, 2};
        bm4[1] = {2, 3, 4};
        bm4[2] = {2, 5, 6};
        bm4[3] = {2, 7, 8};
        b4 = Decomposition_bags(bm4);
    }

    Graph g1, g2, g3, g4;
    Graph d1, d2, d3, d4;
    Decomposition_bags b1, b2, b3, b4;
    Decomposition_bags_map bm1, bm2, bm3, bm4;
};

BOOST_FIXTURE_TEST_SUITE(nice_tree_decomposition, Fixtures_nd)
    template <class Container, class T>
    bool uni_find(const Container & container, T el) {
        for (auto it = container.begin(); it != container.end(); it++) {
            if (*it == el) return true;
        }

        return false;
    }

    //condition 1 - width of decomposition is at most 3*k + 4
    template <class Bags>
    bool td_condition_1(Graph & d, Bags & b, unsigned int width) {
        for (auto bag : make_iterator_range(vertices(d))) {
            if (!b[bag].empty() && (b[bag].size() - 1) > (3 * width + 4)) return false;
        }

        return true;
    }

    //condition 2 - every vertex is in at least one bag
    template <class Bags>
    bool td_condition_2(Graph & g, Graph & d, Bags & b) {
        for (auto g_vertex : make_iterator_range(vertices(g))) {
            bool found = false;
            for (auto bag : make_iterator_range(vertices(d))) {
                if (uni_find(b[bag], g_vertex)) {
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        }

        return true;
    }

    //condition 3 - every edge is in at least one bag
    template <class Bags>
    bool td_condition_3(Graph & g, Graph & d, Bags & b) {
        for (auto g_edge : make_iterator_range(edges(g))) {
            bool found = false;
            for (auto bag : make_iterator_range(vertices(d))) {
                if (uni_find(b[bag], source(g_edge, g))
                    && (uni_find(b[bag], target(g_edge, g)))) {
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        }
        return true;
    }

    //condition 4 - set of every node containing vertex v induces connected component
    template <class Bags>
    bool td_condition_4(Graph & g, Graph & d, Bags & b) {
        subgraph<Graph> sd, ssd;
        copy_graph(d, sd);
        for (auto g_vertex : make_iterator_range(vertices(g))) {
            ssd = sd.create_subgraph();
            for (auto bag : make_iterator_range(vertices(d))) {
                //if node bag contains g_vertex, insert node to ssg
                if (uni_find(b[bag], g_vertex)) add_vertex(bag, ssd);
            }
            std::map<Graph_vertex, Graph_vertex_size_type> c;
            if (connected_components(ssd, make_assoc_property_map(c)) > 1) return false;
        }

        return true;
    }

    //tree decomposition condition 5 - decomposition is connected graph
    bool td_condition_5(Graph & d) {
        if (num_vertices(d) - 1 != num_edges(d)) return false;
        subgraph<Graph> sd;
        copy_graph(d, sd);
        std::map<Graph_vertex, Graph_vertex_size_type> c;

        return (connected_components(sd, make_assoc_property_map(c)) == 1);
    }

    //check if decomposition is a nice one
    template <class Bags>
    bool is_nice_decomposition(Graph & d, Bags & b, Graph_vertex root, unsigned int width) {
        std::list<Graph_vertex> queue;
        std::map<Graph_vertex, bool> visitor_map;

        //initializing visitor map for every vertex
        for (auto v : make_iterator_range(vertices(d))) visitor_map[v] = false;

        if (!b[root].empty()) return false;
        queue.push_back(root);
        while (!queue.empty()) {
            std::list<Graph_vertex> neighbours;

            Graph_vertex curr = queue.front();
            visitor_map[curr] = true;
            queue.pop_front();

            //if size of curr's bag exceeds width+1, return false
            if (b[curr].size() > width + 1) return false;

            //iterating over every neighbour vertex and inserting them to neighbour list if visit=false
            for (auto v : make_iterator_range(adjacent_vertices(curr, d))) {
                if (!visitor_map[v]) neighbours.push_back(v);
            }

            //if no neighbours are left, bag for curr should be empty
            if (neighbours.empty()) {
                if (!b[curr].empty()) return false;
            }
            //if there is only one neighbour, difference between curr and neighbour bag size should be 1
            else if (neighbours.size() == 1) {
                long diff = ((long)b[curr].size()) - b[neighbours.front()].size();
                if (abs(diff) != 1) return false;
                queue.push_back(neighbours.front());
            }
            //if there are two neighbours, their bag should be same as curr's bag
            else if (neighbours.size() == 2) {
                for (auto curr_neighbour : neighbours) {
                    std::set<Graph_vertex> a_set(b[curr].begin(), b[curr].end());
                    std::set<Graph_vertex> b_set(b[curr_neighbour].begin(), b[curr_neighbour].end());
                    if (a_set != b_set) return false;
                    queue.push_back(curr_neighbour);
                }
            }
            //if there are more than two neighbours, nice decomposition is not valid
            else return false;
        }

        return true;
    }

    template <class Bags>
    void ntd_conditions_all(Graph & g, Graph & d, Bags & b, Graph_vertex root, unsigned int width) {
        BOOST_CHECK_EQUAL(true, td_condition_1(d, b, width));
        BOOST_CHECK_EQUAL(true, td_condition_2(g, d, b));
        BOOST_CHECK_EQUAL(true, td_condition_3(g, d, b));
        BOOST_CHECK_EQUAL(true, td_condition_4(g, d, b));
        BOOST_CHECK_EQUAL(true, td_condition_5(d));
        BOOST_CHECK_EQUAL(true, is_nice_decomposition(d, b, root, width));
    }

    BOOST_AUTO_TEST_CASE(nice_tree_decomposition_d1)
    {
        Graph nice_d;
        Decomposition_bags_map bm;
        Decomposition_bags nice_b(bm);
        Graph_vertex root = boost::nice_tree_decomposition(d1, b1, nice_d, nice_b);
        ntd_conditions_all(g1, nice_d, nice_b, root, 0);
    }

    BOOST_AUTO_TEST_CASE(nice_tree_decomposition_d2)
    {
        Graph nice_d;
        Decomposition_bags_map bm;
        Decomposition_bags nice_b(bm);
        Graph_vertex root = boost::nice_tree_decomposition(d2, b2, nice_d, nice_b);
        ntd_conditions_all(g2, nice_d, nice_b, root, 3);
    }

    BOOST_AUTO_TEST_CASE(nice_tree_decomposition_d3) {
        Graph nice_d;
        Decomposition_bags_map bm;
        Decomposition_bags nice_b(bm);
        Graph_vertex root = boost::nice_tree_decomposition(d3, b3, nice_d, nice_b);
        ntd_conditions_all(g3, nice_d, nice_b, root, 2);
    }

    BOOST_AUTO_TEST_CASE(nice_tree_decomposition_d4) {
        Graph nice_d;
        Decomposition_bags_map bm;
        Decomposition_bags nice_b(bm);
        Graph_vertex root = boost::nice_tree_decomposition(d4, b4, nice_d, nice_b);
        ntd_conditions_all(g4, nice_d, nice_b, root, 2);
    }

    BOOST_AUTO_TEST_CASE(nice_tree_decomposition_d4_list_bags) {
        using DBM = std::map<Graph_vertex, std::list<Graph_vertex>>;
        using DB = associative_property_map<DBM>;

        Graph nice_d;
        DBM bm, bm_in;
        DB nice_b(bm);
        bm_in[0] = {0, 1, 2};
        bm_in[1] = {2, 3, 4};
        bm_in[2] = {2, 5, 6};
        bm_in[3] = {2, 7, 8};
        DB b_in(bm_in);

        Graph_vertex root = boost::nice_tree_decomposition(d4, b_in, nice_d, nice_b);
        ntd_conditions_all(g4, nice_d, nice_b, root, 2);
    }

    BOOST_AUTO_TEST_CASE(nice_tree_decomposition_d4_vector_bags) {
        using DBM = std::map<Graph_vertex, std::vector<Graph_vertex>>;
        using DB = associative_property_map<DBM>;

        Graph nice_d;
        DBM bm, bm_in;
        DB nice_b(bm);
        bm_in[0] = {0, 1, 2};
        bm_in[1] = {2, 3, 4};
        bm_in[2] = {2, 5, 6};
        bm_in[3] = {2, 7, 8};
        DB b_in(bm_in);

        Graph_vertex root = boost::nice_tree_decomposition(d4, b_in, nice_d, nice_b);
        ntd_conditions_all(g4, nice_d, nice_b, root, 2);
    }

    BOOST_AUTO_TEST_CASE(nice_tree_decomposition_d4_uset_bags) {
        using DBM = std::map<Graph_vertex, std::unordered_set<Graph_vertex>>;
        using DB = associative_property_map<DBM>;

        Graph nice_d;
        DBM bm, bm_in;
        DB nice_b(bm);
        bm_in[0] = {0, 1, 2};
        bm_in[1] = {2, 3, 4};
        bm_in[2] = {2, 5, 6};
        bm_in[3] = {2, 7, 8};
        DB b_in(bm_in);

        Graph_vertex root = boost::nice_tree_decomposition(d4, b_in, nice_d, nice_b);
        ntd_conditions_all(g4, nice_d, nice_b, root, 2);
    }
BOOST_AUTO_TEST_SUITE_END()
