#include "../src/tree_decomposition.hpp"
#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/copy.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/tuple/tuple.hpp>

using namespace boost;
using Graph = adjacency_list<listS, vecS, undirectedS, property<vertex_name_t, int, property<vertex_index_t, size_t>>, property<edge_index_t, int>>;
using Graph_traits = graph_traits<Graph>;
using Graph_vertex = Graph_traits::vertex_descriptor;
using Graph_vertex_size_type = Graph_traits::vertices_size_type;
using Decomposition_bags_map = std::map<Graph_vertex, std::set<Graph_vertex>>;
using Decomposition_bags = associative_property_map<Decomposition_bags_map>;

struct Fixtures_d {
    Fixtures_d() {
        construct_g1();
        construct_g2();
        construct_g3();
        construct_g4();
        construct_g5();
        construct_g6();
        construct_g7();
        construct_g8();
    }

    ~Fixtures_d() = default;

    void construct_g1() {
        g1 = Graph(1);
    }

    void construct_g2() {
        g2 = Graph(5);
        add_edge(0, 1, g2);
        add_edge(1, 2, g2);
        add_edge(2, 3, g2);
        add_edge(3, 4, g2);
    }

    void construct_g3() {
        g3 = Graph(5);
        add_edge(0, 1, g3);
        add_edge(0, 2, g3);
        add_edge(0, 3, g3);
        add_edge(1, 2, g3);
        add_edge(1, 3, g3);
        add_edge(2, 3, g3);
    }

    void construct_g4() {
        g4 = Graph(5);
        add_edge(2, 3, g4);
        add_edge(2, 4, g4);
        add_edge(3, 4, g4);
    }

    void construct_g5() {
        g5 = Graph(13);
        add_edge(0, 1, g5);
        add_edge(0, 2, g5);
        add_edge(1, 2, g5);
        add_edge(3, 4, g5);
        add_edge(3, 5, g5);
        add_edge(3, 6, g5);
        add_edge(3, 7, g5);
        add_edge(3, 8, g5);
        add_edge(4, 5, g5);
        add_edge(4, 6, g5);
        add_edge(4, 7, g5);
        add_edge(4, 8, g5);
        add_edge(5, 6, g5);
        add_edge(5, 7, g5);
        add_edge(5, 8, g5);
        add_edge(6, 7, g5);
        add_edge(6, 8, g5);
        add_edge(7, 8, g5);
        add_edge(9, 10, g5);
        add_edge(10, 11, g5);
        add_edge(11, 12, g5);
    }

    void construct_g6() {
        g6 = Graph(8);
        add_edge(0, 1, g6);
        add_edge(0, 2, g6);
        add_edge(1, 3, g6);
        add_edge(2, 3, g6);
        add_edge(3, 4, g6);
        add_edge(4, 5, g6);
        add_edge(4, 6, g6);
        add_edge(5, 7, g6);
        add_edge(6, 7, g6);
    }

    void construct_g7() {
        g7 = Graph(8);
        add_edge(0, 1, g7);
        add_edge(0, 3, g7);
        add_edge(1, 2, g7);
        add_edge(1, 3, g7);
        add_edge(1, 4, g7);
        add_edge(1, 6, g7);
        add_edge(2, 4, g7);
        add_edge(3, 5, g7);
        add_edge(3, 6, g7);
        add_edge(4, 6, g7);
        add_edge(4, 7, g7);
        add_edge(5, 6, g7);
        add_edge(6, 7, g7);
    }

    void construct_g8() {
        g8 = Graph(120);
        for (int i = 0; i < 50; i++) {
            for (int j = i+1; j < 50; j++) add_edge(i, j, g8);
        }
        for (int i = 49; i < 80; i++) add_edge(i, i+1, g8);
        for (int i = 79; i < 100; i++) {
            for (int j = i+1; j < 120; j++) add_edge(i, j, g8);
        }
        for (int i = 100; i < 120; i++) {
            for (int j = i+1; j < 120; j++) add_edge(i, j, g8);
        }
    }

    Graph g1, g2, g3, g4, g5, g6, g7, g8;
};

BOOST_FIXTURE_TEST_SUITE(split_set, Fixtures_d)
    template<class T>
    std::vector<tuple<std::set<T>, std::set<T>>> generate_splits(const std::set<T> & src) {
        using set_pair = tuple<std::set<T>, std::set<T>>;

        std::vector<set_pair> out;
        int max_c_num = 1 << (src.size() - 1);
        for (int i = 1; i < max_c_num; i++) {
            set_pair res = detail::split_set<T>(src, i);
            out.push_back(res);
        }

        return out;
    }
    template<class T>
    bool is_all_uniq(const std::vector<tuple<std::set<T>, std::set<T>>> & src) {
        for (unsigned int i = 0; i < src.size(); i++) {
            for (unsigned int j = i+1; j < src.size(); j++) {
                if (get<0>(src[i]) == get<0>(src[j])
                    || get<0>(src[i]) == get<1>(src[j])) return false;
            }
        }

        return true;
    }

    BOOST_AUTO_TEST_CASE(int_set)
    {
        std::set<int> s = {1,2,3,4,5,6};
        auto splits = generate_splits(s);
        BOOST_CHECK_EQUAL(true, is_all_uniq(splits));
        BOOST_CHECK_EQUAL(31, (int)splits.size());
    }
    BOOST_AUTO_TEST_CASE(string_set)
    {
        std::set<std::string> s = {"a","b","c","d","e"};
        auto splits = generate_splits(s);
        BOOST_CHECK_EQUAL(true, is_all_uniq(splits));
        BOOST_CHECK_EQUAL(15, (int)splits.size());
    }
    BOOST_AUTO_TEST_CASE(edge_cases)
    {
        std::set<int> s;
        tuple<std::set<int>, std::set<int>> res = detail::split_set(s, 1);
        BOOST_CHECK_EQUAL(true, get<0>(res).empty());
        BOOST_CHECK_EQUAL(true, get<1>(res).empty());
        s.insert(1);
        s.insert(2);
        res = detail::split_set(s, 1);
        BOOST_CHECK_EQUAL(1, (int)get<0>(res).size());
        BOOST_CHECK_EQUAL(1, (int)get<1>(res).size());
    }
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(get_components, Fixtures_d)
    template <class T>
    std::vector<subgraph<T>*> get_components_wrapper(T & g, subgraph<T> & sg) {
        copy_graph(g, sg);
        std::vector<subgraph<T>*> components = detail::get_components(sg);

        return components;
    }

    template <class Graph>
    //edges() segfaults when edge is not found
    bool contains_edge(Graph & g, typename graph_traits<Graph>::vertex_descriptor u, typename graph_traits<Graph>::vertex_descriptor v) {
        for (auto curr_e : make_iterator_range(edges(g))) {
            if ((g.local_to_global(source(curr_e, g)) == u && g.local_to_global(target(curr_e, g) == v))
                || (g.local_to_global(source(curr_e, g)) == v && g.local_to_global(target(curr_e, g) == u))) return true;
        }

        return false;
    }

    template <class Graph>
    bool contains_vertex(Graph & g, typename graph_traits<Graph>::vertex_descriptor v) {
        for (auto curr_v : make_iterator_range(vertices(g))) {
            if (g.local_to_global(curr_v) == v) return true;
        }

        return false;
    }

    BOOST_AUTO_TEST_CASE(components_g1)
    {
        subgraph<Graph> sg;
        auto components = get_components_wrapper(g1, sg);
        BOOST_CHECK_EQUAL(1, (int)components.size());
        BOOST_CHECK_EQUAL(1, (int)num_vertices(*components[0]));
        BOOST_CHECK_EQUAL(0, (int)num_edges(*components[0]));
        BOOST_CHECK_EQUAL(true, contains_vertex(*components[0], 0));
    }

    BOOST_AUTO_TEST_CASE(components_g2)
    {
        subgraph<Graph> sg;
        auto components = get_components_wrapper(g2, sg);
        BOOST_CHECK_EQUAL(1, (int)components.size());
        BOOST_CHECK_EQUAL(5, (int)num_vertices(*components[0]));
        BOOST_CHECK_EQUAL(4, (int)num_edges(*components[0]));
        BOOST_CHECK_EQUAL(true, contains_vertex(*components[0], 0));
        BOOST_CHECK_EQUAL(true, contains_vertex(*components[0], 1));
        BOOST_CHECK_EQUAL(true, contains_vertex(*components[0], 2));
        BOOST_CHECK_EQUAL(true, contains_vertex(*components[0], 3));
        BOOST_CHECK_EQUAL(true, contains_vertex(*components[0], 4));
        BOOST_CHECK_EQUAL(true, contains_edge(*components[0], 0, 1));
        BOOST_CHECK_EQUAL(true, contains_edge(*components[0], 1, 2));
        BOOST_CHECK_EQUAL(true, contains_edge(*components[0], 2, 3));
        BOOST_CHECK_EQUAL(true, contains_edge(*components[0], 3, 4));
    }

    BOOST_AUTO_TEST_CASE(components_g3)
    {
        subgraph<Graph> sg;
        auto components = get_components_wrapper(g3, sg);
        BOOST_CHECK_EQUAL(2, (int)components.size());
        BOOST_CHECK_EQUAL(4, (int)num_vertices(*components[0]));
        BOOST_CHECK_EQUAL(1, (int)num_vertices(*components[1]));
        BOOST_CHECK_EQUAL(6, (int)num_edges(*components[0]));
        BOOST_CHECK_EQUAL(0, (int)num_edges(*components[1]));
        BOOST_CHECK_EQUAL(true, contains_vertex(*components[0], 0));
        BOOST_CHECK_EQUAL(true, contains_vertex(*components[0], 1));
        BOOST_CHECK_EQUAL(true, contains_vertex(*components[0], 2));
        BOOST_CHECK_EQUAL(true, contains_vertex(*components[0], 3));
        BOOST_CHECK_EQUAL(true, contains_vertex(*components[1], 4));
        BOOST_CHECK_EQUAL(true, contains_edge(*components[0], 0, 1));
        BOOST_CHECK_EQUAL(true, contains_edge(*components[0], 0, 2));
        BOOST_CHECK_EQUAL(true, contains_edge(*components[0], 0, 3));
        BOOST_CHECK_EQUAL(true, contains_edge(*components[0], 1, 2));
        BOOST_CHECK_EQUAL(true, contains_edge(*components[0], 1, 3));
        BOOST_CHECK_EQUAL(true, contains_edge(*components[0], 2, 3));
    }

    BOOST_AUTO_TEST_CASE(components_g5)
    {
        subgraph<Graph> sg;
        auto components = get_components_wrapper(g5, sg);
        BOOST_CHECK_EQUAL(3, (int)components.size());
        BOOST_CHECK_EQUAL(3, (int)num_vertices(*components[0]));
        BOOST_CHECK_EQUAL(6, (int)num_vertices(*components[1]));
        BOOST_CHECK_EQUAL(4, (int)num_vertices(*components[2]));
        BOOST_CHECK_EQUAL(3, (int)num_edges(*components[0]));
        BOOST_CHECK_EQUAL(15, (int)num_edges(*components[1]));
        BOOST_CHECK_EQUAL(3, (int)num_edges(*components[2]));
        BOOST_CHECK_EQUAL(true, contains_vertex(*components[0], 2));
        BOOST_CHECK_EQUAL(true, contains_vertex(*components[1], 3));
        BOOST_CHECK_EQUAL(true, contains_vertex(*components[2], 11));
        BOOST_CHECK_EQUAL(true, contains_edge(*components[0], 0, 1));
        BOOST_CHECK_EQUAL(true, contains_edge(*components[1], 4, 5));
        BOOST_CHECK_EQUAL(true, contains_edge(*components[2], 9, 10));
    }
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(get_neighbor_vertices, Fixtures_d)
    BOOST_AUTO_TEST_CASE(neigbours_g1)
    {
        subgraph<Graph> sg;
        copy_graph(g1, sg);
        auto neighbours_in = detail::get_neighbour_vertices(sg, true);
        std::set<Graph_vertex> neighbours_in_ref = {0};
        BOOST_CHECK(neighbours_in_ref == neighbours_in);
        BOOST_CHECK_EQUAL(true, detail::get_neighbour_vertices(sg, false).empty());
    }

    BOOST_AUTO_TEST_CASE(neigbours_g2)
    {
        subgraph<Graph> sg, ssg;
        copy_graph(g2, sg);
        ssg = sg.create_subgraph();
        add_vertex(1, ssg);
        add_vertex(2, ssg);
        auto neighbours_in = detail::get_neighbour_vertices(ssg, true);
        auto neighbours_ex = detail::get_neighbour_vertices(ssg, false);
        std::set<Graph_vertex> neighbours_in_ref = {0, 1, 2, 3};
        std::set<Graph_vertex> neighbours_ex_ref = {0, 3};
        BOOST_CHECK(neighbours_in_ref == neighbours_in);
        BOOST_CHECK(neighbours_ex_ref == neighbours_ex);
    }

    BOOST_AUTO_TEST_CASE(neigbours_g3)
    {
        subgraph<Graph> sg;
        copy_graph(g3, sg);
        auto neighbours_in = detail::get_neighbour_vertices(sg, true);
        std::set<Graph_vertex> neighbours_in_ref = {0, 1, 2, 3, 4};
        BOOST_CHECK(neighbours_in_ref == neighbours_in);
        BOOST_CHECK_EQUAL(true, detail::get_neighbour_vertices(sg, false).empty());
    }

    BOOST_AUTO_TEST_CASE(neigbours_g4)
    {
        subgraph<Graph> sg, ssg;
        copy_graph(g4, sg);
        ssg = sg.create_subgraph();
        add_vertex(0, ssg);
        auto neighbours_in = detail::get_neighbour_vertices(ssg, true);
        std::set<Graph_vertex> neighbours_in_ref = {0};
        BOOST_CHECK(neighbours_in_ref == neighbours_in);
        BOOST_CHECK_EQUAL(true, detail::get_neighbour_vertices(ssg, false).empty());
    }
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(max_flow_sep, Fixtures_d)
    BOOST_AUTO_TEST_CASE(maxflow_g2_case1)
    {
        subgraph<Graph> sg;
        copy_graph(g2, sg);
        auto separation = detail::max_flow_sep(make_tuple<std::set<Graph_vertex>, std::set<Graph_vertex>>({0, 1, 2}, {3, 4}), sg, 1);
        BOOST_CHECK_EQUAL(1, (int)separation.size());
        BOOST_CHECK_EQUAL(1, (int)(separation.count(2) + separation.count(3)));
    }

    BOOST_AUTO_TEST_CASE(maxflow_g2_case2)
    {
        subgraph<Graph> sg;
        copy_graph(g2, sg);
        auto separation = detail::max_flow_sep(make_tuple<std::set<Graph_vertex>, std::set<Graph_vertex>>({0, 1, 2}, {3, 4}), sg, 5);
        BOOST_CHECK_EQUAL(1, (int)separation.size());
        BOOST_CHECK_EQUAL(1, (int)(separation.count(2) + separation.count(3)));
    }

    BOOST_AUTO_TEST_CASE(maxflow_g2_case3)
    {
        subgraph<Graph> sg;
        copy_graph(g2, sg);
        auto separation = detail::max_flow_sep(make_tuple<std::set<Graph_vertex>, std::set<Graph_vertex>>({0, 1, 2}, {3, 4}), sg, 0);
        BOOST_CHECK_EQUAL(false, separation.empty());
    }

    BOOST_AUTO_TEST_CASE(maxflow_g2_case4)
    {
        subgraph<Graph> sg;
        copy_graph(g2, sg);
        auto separation = detail::max_flow_sep(make_tuple<std::set<Graph_vertex>, std::set<Graph_vertex>>({0}, {4}), sg, 1);
        BOOST_CHECK_EQUAL(1, (int)separation.size());
    }

    BOOST_AUTO_TEST_CASE(maxflow_g3_case1)
    {
        subgraph<Graph> sg;
        copy_graph(g3, sg);
        auto separation = detail::max_flow_sep(make_tuple<std::set<Graph_vertex>, std::set<Graph_vertex>>({0, 1}, {2, 3}), sg, 0);
        BOOST_CHECK_EQUAL(true, (int)separation.empty());
    }

    BOOST_AUTO_TEST_CASE(maxflow_g3_case2)
    {
        subgraph<Graph> sg;
        copy_graph(g3, sg);
        auto separation = detail::max_flow_sep(make_tuple<std::set<Graph_vertex>, std::set<Graph_vertex>>({0, 1}, {2, 3}), sg, 1);
        BOOST_CHECK_EQUAL(2, (int)separation.size());
        BOOST_CHECK_EQUAL(1, (int)(separation.count(0) + separation.count(2) + separation.count(3)));
        BOOST_CHECK_EQUAL(1, (int)(separation.count(1) + separation.count(2) + separation.count(3)));
    }

    BOOST_AUTO_TEST_CASE(maxflow_g3_case3)
    {
        subgraph<Graph> sg;
        copy_graph(g3, sg);
        auto separation = detail::max_flow_sep(make_tuple<std::set<Graph_vertex>, std::set<Graph_vertex>>({0}, {2}), sg, 0);
        BOOST_CHECK_EQUAL(1, (int)separation.size());
        BOOST_CHECK_EQUAL(1, (int)(separation.count(0) + separation.count(1)));
    }

    BOOST_AUTO_TEST_CASE(maxflow_g6_case1)
    {
        subgraph<Graph> sg;
        copy_graph(g6, sg);
        auto separation = detail::max_flow_sep(make_tuple<std::set<Graph_vertex>, std::set<Graph_vertex>>({0}, {7}), sg, 0);
        BOOST_CHECK_EQUAL(1, (int)separation.size());
    }
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(tree_decomposition, Fixtures_d)
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
            if ((b[bag].size() - 1) > (3 * width + 4)) return false;
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

    //condition 5 - decomposition is connected graph
    bool td_condition_5(Graph & d) {
        if (num_vertices(d) - 1 != num_edges(d)) return false;
        subgraph<Graph> sd;
        copy_graph(d, sd);
        std::map<Graph_vertex, Graph_vertex_size_type> c;

        return (connected_components(sd, make_assoc_property_map(c)) == 1);
    }

    template <class Bags>
    void td_conditions_all(Graph & g, Graph & d, Bags & b, unsigned int width) {
        BOOST_CHECK_EQUAL(true, td_condition_1(d, b, width));
        BOOST_CHECK_EQUAL(true, td_condition_2(g, d, b));
        BOOST_CHECK_EQUAL(true, td_condition_3(g, d, b));
        BOOST_CHECK_EQUAL(true, td_condition_4(g, d, b));
        BOOST_CHECK_EQUAL(true, td_condition_5(d));
    }

    BOOST_AUTO_TEST_CASE(tree_decomposition_g1)
    {
        unsigned int width = 0;
        Graph d;
        Decomposition_bags_map bm;
        Decomposition_bags b(bm);
        BOOST_CHECK_EQUAL(true, boost::tree_decomposition(g1, d, b, width));
        td_conditions_all(g1, d, b, width);
    }

    BOOST_AUTO_TEST_CASE(tree_decomposition_g2)
    {
        unsigned int width = 1;
        Graph d;
        Decomposition_bags_map bm;
        Decomposition_bags b(bm);
        BOOST_CHECK_EQUAL(true, boost::tree_decomposition(g2, d, b, width));
        td_conditions_all(g2, d, b, width);
    }

    BOOST_AUTO_TEST_CASE(tree_decomposition_g3)
    {
        unsigned int width = 2;
        Graph d;
        Decomposition_bags_map bm;
        Decomposition_bags b(bm);
        BOOST_CHECK_EQUAL(true, boost::tree_decomposition(g3, d, b, width));
        td_conditions_all(g3, d, b, width);
    }

    BOOST_AUTO_TEST_CASE(tree_decomposition_g4)
    {
        unsigned int width = 2;
        Graph d;
        Decomposition_bags_map bm;
        Decomposition_bags b(bm);
        BOOST_CHECK_EQUAL(true, boost::tree_decomposition(g4, d, b, width));
        td_conditions_all(g4, d, b, width);
    }

    BOOST_AUTO_TEST_CASE(tree_decomposition_g4_vector_bags)
    {
        using DBM = std::map<Graph_vertex, std::vector<Graph_vertex>>;
        using DB = associative_property_map<DBM>;

        unsigned int width = 2;
        Graph d;
        DBM bm;
        DB b(bm);
        BOOST_CHECK_EQUAL(true, boost::tree_decomposition(g4, d, b, width));
        td_conditions_all(g4, d, b, width);
    }

    BOOST_AUTO_TEST_CASE(tree_decomposition_g4_list_bags)
    {
        using DBM = std::map<Graph_vertex, std::list<Graph_vertex>>;
        using DB = associative_property_map<DBM>;

        unsigned int width = 2;
        Graph d;
        DBM bm;
        DB b(bm);
        BOOST_CHECK_EQUAL(true, boost::tree_decomposition(g4, d, b, width));
        td_conditions_all(g4, d, b, width);
    }

    BOOST_AUTO_TEST_CASE(tree_decomposition_g4_uset_bags)
    {
        using DBM = std::map<Graph_vertex, std::unordered_set<Graph_vertex>>;
        using DB = associative_property_map<DBM>;

        unsigned int width = 2;
        Graph d;
        DBM bm;
        DB b(bm);
        BOOST_CHECK_EQUAL(true, boost::tree_decomposition(g4, d, b, width));
        td_conditions_all(g4, d, b, width);
    }

    BOOST_AUTO_TEST_CASE(tree_decomposition_g5)
    {
        unsigned int width = 4;
        Graph d;
        Decomposition_bags_map bm;
        Decomposition_bags b(bm);
        BOOST_CHECK_EQUAL(true, boost::tree_decomposition(g5, d, b, width));
        td_conditions_all(g5, d, b, width);
    }

    BOOST_AUTO_TEST_CASE(tree_decomposition_g6)
    {
        unsigned int width = 2;
        Graph d;
        Decomposition_bags_map bm;
        Decomposition_bags b(bm);
        BOOST_CHECK_EQUAL(true, boost::tree_decomposition(g6, d, b, width));
        td_conditions_all(g6, d, b, width);
    }

    BOOST_AUTO_TEST_CASE(tree_decomposition_g7)
    {
        unsigned int width = 2;
        Graph d;
        Decomposition_bags_map bm;
        Decomposition_bags b(bm);
        BOOST_CHECK_EQUAL(true, boost::tree_decomposition(g7, d, b, width));
        td_conditions_all(g7, d, b, width);
    }

    BOOST_AUTO_TEST_CASE(tree_decomposition_g8_case1)
    {
        unsigned int width = 2;
        Graph d;
        Decomposition_bags_map bm;
        Decomposition_bags b(bm);
        BOOST_CHECK_EQUAL(false, boost::tree_decomposition(g8, d, b, width));
    }

    BOOST_AUTO_TEST_CASE(tree_decomposition_g8_case2)
    {
        unsigned int width = 16;
        Graph d;
        Decomposition_bags_map bm;
        Decomposition_bags b(bm);
        BOOST_CHECK_EQUAL(true, boost::tree_decomposition(g8, d, b, width));
        td_conditions_all(g8, d, b, width);
    }

    BOOST_AUTO_TEST_CASE(tree_decomposition_g8_case3)
    {
        unsigned int width = 15;
        Graph d;
        Decomposition_bags_map bm;
        Decomposition_bags b(bm);
        BOOST_CHECK_EQUAL(false, boost::tree_decomposition(g8, d, b, width));
    }
BOOST_AUTO_TEST_SUITE_END()
