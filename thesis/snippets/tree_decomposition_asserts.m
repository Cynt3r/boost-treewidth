//== TYPEDEFS ==========================
//vertex descriptor that is used to address vertices in graph
using Graph_vertex = typename graph_traits<Graph>::vertex_descriptor;
//vertex descriptor that is used to address vertices in decomposition graph
using Decomposition_vertex = typename graph_traits<Decomposition>::vertex_descriptor;
//type of value (container) of PropertyMap
using Bag_value_type = typename property_traits<Bags>::value_type;
//type of container value, that is stored as value in PropertyMap
using Bag_inside_type = typename Bag_value_type::value_type;

//== ASSERTS ===========================
BOOST_CONCEPT_ASSERT((VertexListGraphConcept<Graph>)); //A1
BOOST_CONCEPT_ASSERT((InsertCollection<Bag_value_type>)); //A2
BOOST_CONCEPT_ASSERT((ReadWritePropertyMapConcept<Bags, Decomposition_vertex>)); //A3
BOOST_STATIC_ASSERT((is_same<Graph_vertex, Bag_inside_type>::value)); //A4