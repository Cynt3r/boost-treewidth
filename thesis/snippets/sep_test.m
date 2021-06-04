BOOST_AUTO_TEST_CASE(maxflow_g2_case2)
{
	subgraph<Graph> sg;
	copy_graph(g2, sg);
	auto separation = detail::max_flow_sep(...);
	BOOST_CHECK_EQUAL(1, separation.size());
	BOOST_CHECK_EQUAL(1, separation.count(2) + separation.count(3));
}