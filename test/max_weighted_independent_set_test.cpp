#include "../src/max_weighted_independent_set.hpp"
#include <map>
#include <set>
#include <unordered_set>
#include <boost/graph/adjacency_list.hpp>
#include <boost/test/unit_test.hpp>

using namespace boost;
using Graph = adjacency_list<listS, vecS, undirectedS, property<vertex_name_t, int, property<vertex_index_t, size_t>>, property<edge_index_t, int>>;
using Graph_traits = graph_traits<Graph>;
using Graph_vertex = Graph_traits::vertex_descriptor;
using Decomposition_bags_map = std::map<Graph_vertex, std::set<Graph_vertex>>;
using Decomposition_bags = associative_property_map<Decomposition_bags_map>;
using Weights_map = std::map<Graph_vertex, unsigned int>;
using Weights = associative_property_map<Weights_map>;
using Colors_map = std::map<Graph_vertex, int>;
using Colors = associative_property_map<Colors_map>;
using Colors_value_type = Colors::value_type;

struct Fixtures_mwis {
    Fixtures_mwis() {
        construct_d1();
        construct_d2();
        construct_d3();
        construct_d4();
    }

    ~Fixtures_mwis() = default;

    void construct_d1() {
        g1 = Graph(1);

        d1 = Graph(3);
        add_edge(0, 1, d1);
        add_edge(1, 2, d1);

        bm1[0] = {};
        bm1[1] = {0};
        bm1[2] = {};
        b1 = Decomposition_bags(bm1);

        wm1[0] = 5;
        w1 = Weights(wm1);
    }

    void construct_d2() {
        g2 = Graph(3);
        add_edge(0, 1, g2);
        add_edge(1, 2, g2);

        d2 = Graph(7);
        add_edge(0, 1, d2);
        add_edge(1, 2, d2);
        add_edge(2, 3, d2);
        add_edge(3, 4, d2);
        add_edge(4, 5, d2);
        add_edge(5, 6, d2);

        bm2[0] = {};
        bm2[1] = {0};
        bm2[2] = {0,1};
        bm2[3] = {1};
        bm2[4] = {1,2};
        bm2[5] = {2};
        bm2[6] = {};
        b2 = Decomposition_bags(bm2);

        wm2[0] = 2;
        wm2[1] = 8;
        wm2[2] = 5;
        w2 = Weights(wm2);
    }

    void construct_d3() {
        g3 = Graph(7);
        add_edge(0, 1, g3);
        add_edge(1, 2, g3);
        add_edge(0, 2, g3);
        add_edge(3, 2, g3);
        add_edge(4, 2, g3);
        add_edge(3, 4, g3);
        add_edge(5, 2, g3);
        add_edge(6, 2, g3);
        add_edge(6, 5, g3);

        d3 = Graph(18);
        add_edge(0, 1, d3);
        add_edge(1, 2, d3);
        add_edge(2, 3, d3);
        add_edge(3, 4, d3);
        add_edge(4, 5, d3);
        add_edge(5, 6, d3);
        add_edge(6, 7, d3);
        add_edge(7, 8, d3);
        add_edge(8, 9, d3);
        add_edge(9, 10, d3);
        add_edge(10, 11, d3);
        add_edge(5, 12, d3);
        add_edge(12, 13, d3);
        add_edge(13, 14, d3);
        add_edge(14, 15, d3);
        add_edge(15, 16, d3);
        add_edge(16, 17, d3);

        bm3[0] = {};
        bm3[1] = {0};
        bm3[2] = {0,1};
        bm3[3] = {0,1,2};
        bm3[4] = {1,2};
        bm3[5] = {2};
        bm3[6] = {2};
        bm3[7] = {2,3};
        bm3[8] = {2,3,4};
        bm3[9] = {3,4};
        bm3[10] = {4};
        bm3[11] = {};
        bm3[12] = {2};
        bm3[13] = {2,5};
        bm3[14] = {2,5,6};
        bm3[15] = {5,6};
        bm3[16] = {6};
        bm3[17] = {};
        b3 = Decomposition_bags(bm3);

        wm3[0] = 5;
        wm3[1] = 5;
        wm3[2] = 29;
        wm3[3] = 5;
        wm3[4] = 5;
        wm3[5] = 5;
        wm3[6] = 5;
        w3 = Weights(wm3);

        wm3_2[0] = 10;
        wm3_2[1] = 5;
        wm3_2[2] = 29;
        wm3_2[3] = 10;
        wm3_2[4] = 5;
        wm3_2[5] = 10;
        wm3_2[6] = 5;
        w3_2 = Weights(wm3_2);
    }

    void construct_d4() {
        g4 = Graph(5);
        for (int i = 0; i < 5; i++) {
            for (int j = i+1; j < 5; j++) add_edge(i, j, g4);
        }

        d4 = Graph(11);
        for (int i = 0; i < 10; i++) add_edge(i, i+1, d4);

        bm4[0] = {};
        bm4[1] = {0};
        bm4[2] = {0,1};
        bm4[3] = {0,1,2};
        bm4[4] = {0,1,2,3};
        bm4[5] = {0,1,2,3,4};
        bm4[6] = {1,2,3,4};
        bm4[7] = {2,3,4};
        bm4[8] = {3,4};
        bm4[9] = {4};
        bm4[10] = {};
        b4 = Decomposition_bags(bm4);

        wm4[0] = 0;
        wm4[1] = 1;
        wm4[2] = 2;
        wm4[3] = 3;
        wm4[4] = 4;
        w4 = Weights(wm4);
    }

    Graph g1, g2, g3, g4;
    Graph d1, d2, d3, d4;
    Decomposition_bags b1, b2, b3, b4;
    Decomposition_bags_map bm1, bm2, bm3, bm4;
    Weights w1, w2, w3, w3_2, w4;
    Weights_map wm1, wm2, wm3, wm3_2, wm4;
};

BOOST_FIXTURE_TEST_SUITE(max_weighted_independent_set, Fixtures_mwis)
    void check_colors(Graph & g, Colors & colors, std::unordered_set<Graph_vertex> & ref) {
        for (auto v : make_iterator_range(vertices(g))) {
            if (ref.count(v) == 1) BOOST_CHECK_EQUAL(color_traits<Colors_value_type>::white(), colors[v]);
            else BOOST_CHECK_EQUAL(color_traits<Colors_value_type>::black(), colors[v]);
        }
    }

    BOOST_AUTO_TEST_CASE(max_weighted_independent_set_d1)
    {
        Colors_map cm;
        Colors c = Colors(cm);
        std::unordered_set<Graph_vertex> ref = {0};
        int max_w = boost::max_weighted_independent_set(g1, d1, b1, 0, w1, c);
        BOOST_CHECK_EQUAL(5, max_w);
        check_colors(g1, c, ref);
    }

    BOOST_AUTO_TEST_CASE(max_weighted_independent_set_d2)
    {
        Colors_map cm;
        Colors c = Colors(cm);
        std::unordered_set<Graph_vertex> ref = {1};
        int max_w = boost::max_weighted_independent_set(g2, d2, b2, 0, w2, c);
        BOOST_CHECK_EQUAL(8, max_w);
        check_colors(g2, c, ref);
    }

    BOOST_AUTO_TEST_CASE(max_weighted_independent_set_d3_case1)
    {
        Colors_map cm;
        Colors c = Colors(cm);
        std::unordered_set<Graph_vertex> ref = {2};
        int max_w = boost::max_weighted_independent_set(g3, d3, b3, 0, w3, c);
        BOOST_CHECK_EQUAL(29, max_w);
        check_colors(g3, c, ref);
    }

    BOOST_AUTO_TEST_CASE(max_weighted_independent_set_d3_case2)
    {
        Colors_map cm;
        Colors c = Colors(cm);
        std::unordered_set<Graph_vertex> ref = {0,3,5};
        int max_w = boost::max_weighted_independent_set(g3, d3, b3, 0, w3_2, c);
        BOOST_CHECK_EQUAL(30, max_w);
        check_colors(g3, c, ref);
    }

    BOOST_AUTO_TEST_CASE(max_weighted_independent_set_d4_case1)
    {
        Colors_map cm;
        Colors c = Colors(cm);
        std::unordered_set<Graph_vertex> ref = {4};
        int max_w = boost::max_weighted_independent_set(g4, d4, b4, 0, w4, c);
        BOOST_CHECK_EQUAL(4, max_w);
        check_colors(g4, c, ref);
    }

    BOOST_AUTO_TEST_CASE(max_weighted_independent_set_d4_case2)
    {
        Colors_map cm;
        Colors c = Colors(cm);
        std::unordered_set<Graph_vertex> ref = {4};
        int max_w = boost::max_weighted_independent_set(g4, d4, b4, 10, w4, c);
        BOOST_CHECK_EQUAL(4, max_w);
        check_colors(g4, c, ref);
    }
BOOST_AUTO_TEST_SUITE_END()
