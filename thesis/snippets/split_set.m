Graph_vertex_set a, b;
unsigned long long set_size = s_copy.size();
for (unsigned long long i = 0; i < set_size; i++) {
    auto it = s_copy.begin();
    if (c_number % 2 == 0) a.insert(*it);
    else b.insert(*it);
    s_copy.erase(it);
    if (c_number != 0) c_number = c_number >> 1;
}
return make_tuple<Graph_vertex_set, Graph_vertex_set>(a, b);