//for every component we will recursively call decompose
for (auto component : d_components) {
	tuple<Decomposition_vertex, bool> res = detail::decompose(
		parent,
		decomposition,
		bags,
		detail::get_neighbour_vertices(*component, true),
		detail::get_neighbour_vertices(*component, false),
		k);
		
	if (!get<1>(res)) return res;
	//connect bag with current root
	add_edge(get<0>(res), root_bag, decomposition);
}