#include <iostream>
#include <set>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graphviz.hpp>

//to print content of set, operator << must be defined for std::set
namespace std {
	template <class T>
	std::ostream& operator<< (std::ostream& os, const std::set<T> & in) {
		os << "[";
		for (auto it = in.begin(); it != in.end(); it++) {
            if (it != in.begin()) os << ",";
            os << *it;
        }
        os << "]";

		return os;
	}
}

using namespace boost;

//save graph with property map into a file
template <class G, class PM>
void save_graph(std::string filename, G & g, PM & pm) {
	std::ofstream file;
  	file.open("example/"+filename);
  	boost::write_graphviz(file, g, make_label_writer(pm));
  	file.close();
}

//save graph into file
template <class G>
void save_graph(std::string filename, G & g) {
	std::ofstream file;
  	file.open("example/"+filename);
  	boost::write_graphviz(file, g);
  	file.close();
}

//output graph into stdin
template <class G>
void stdin_graph(G & g, std::string name) {
	std::cout << "\n\n======== Graph " << name << " ========\n";
	std::cout << "Edges of the graph:\n";
	boost::print_graph(g);
}

//output graph and property map into stdin
template <class G, class PM>
void stdin_graph(G & g, std::string name, PM & pm) {
	stdin_graph(g, name);
	std::cout << "Bags for each vertex:\n";
	for (auto v : make_iterator_range(vertices(g))) std::cout << v << ": " << pm[v] << "\n";
}

//output graphs independent set into stdin
template <class G, class C>
void print_weight(G & g, C & c, unsigned int total) {
	std::cout << "\n\nMaximum independent set of g is [";
	bool first = true;
	for (auto v : make_iterator_range(vertices(g))) {
		if (c[v] == boost::color_traits<int>::white()) {
			if (!first) std::cout << ",";
			first = false;
			std::cout << v;
		}
	}
	std::cout << "] and its total weight is " << total << ".\n";
}
