//data structure for storing calculate_weight() call results
std::map<
	Decomposition_vertex,
	std::unordered_map<
		std::set<Graph_vertex>,
		std::pair<Weight_type, std::list<Graph_vertex>>,
		hash<std::set<Graph_vertex>>
	>
> memory;

weight = memory[t][S].first;
backtrack_list = memory[t][S].second;