template <class Graph, class Decomposition, class Bags, class Weights, class Colors>
typename property_traits<Weights>::value_type
max_weighted_independent_set(
	Graph & g,
	Decomposition & d,
	Bags & bags,
	typename graph_traits<Decomposition>::vertex_descriptor root,
	Weights & weights,
	Colors & colors)