//== TYPEDEFS ===========================================
//vertex descriptor that is used to address vertices in decomposition graph
using Decomposition_vertex = typename graph_traits<Decomposition>::vertex_descriptor;
//type of value (container) of PropertyMap
using Bag_value_type = typename property_traits<Bags>::value_type;
//=========================================== TYPEDEFS ==

//== ASSERTS ============================================
BOOST_CONCEPT_ASSERT((VertexListGraphConcept<Decomposition>)); //A1
BOOST_CONCEPT_ASSERT((InsertCollection<Bag_value_type>)); //A2
BOOST_CONCEPT_ASSERT((ReadWritePropertyMapConcept<Bags, Decomposition_vertex>)); //A3
//============================================ ASSERTS ==