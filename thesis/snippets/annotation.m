/**
 * @tparam Graph Type of the graph.
 * @tparam Decomposition Type of the decomposition.
 * @tparam Bags Type of the property map representing bags ...
 * @param[in] g An undirected graph. The graph type must be ...
 * @param[out] d An undirected graph in which will be ...
 * @param[out] bags The bags property map. The type must be ...
 * @param[in] k Parameter, which defines the maximum width ...
 * @return True if tree decomposition of graph g exists ...
 */
template <class Graph, class Decomposition, class Bags>
bool tree_decomposition(Graph & g, Decomposition & d, Bags & bags, unsigned long k)