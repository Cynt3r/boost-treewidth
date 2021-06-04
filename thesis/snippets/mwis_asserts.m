//== TYPEDEFS ===========================================
//type of value (container) of PropertyMap
using Bag_value_type = typename property_traits<Bags>::value_type;
//type of container value, that is stored as value in PropertyMap
using Bag_inside_type = typename Bag_value_type::value_type;
//type representing weight
using Weight_type = typename property_traits<Weights>::value_type;
//type representing color
using Colors_type = typename property_traits<Colors>::value_type;
//== ASSERTS ============================================
BOOST_CONCEPT_ASSERT((VertexListGraphConcept<Graph>)); //A1
BOOST_CONCEPT_ASSERT((VertexListGraphConcept<Decomposition>)); //A2
BOOST_CONCEPT_ASSERT((ReadablePropertyMapConcept<Bags, Decomposition_vertex>)); //A3
BOOST_CONCEPT_ASSERT((Container<Bag_value_type>)); //A4
BOOST_STATIC_ASSERT((is_same<Graph_vertex, Bag_inside_type>::value)); //A5
BOOST_CONCEPT_ASSERT((ReadablePropertyMapConcept<Weights, Graph_vertex>)); //A6
BOOST_CONCEPT_ASSERT((UnsignedIntegerConcept<Weight_type>)); //A7
BOOST_CONCEPT_ASSERT((ReadWritePropertyMapConcept<Colors, Graph_vertex>)); //A8
BOOST_CONCEPT_ASSERT((IntegerConcept<Colors_type>)); //A9