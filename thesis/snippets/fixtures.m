struct Fixtures_d {
	Fixtures_d() {
		construct_g1();
		construct_g2();
	}
	~Fixtures_d() = default;

	void construct_g1() {...}
	void construct_g2() {...}
};

BOOST_FIXTURE_TEST_SUITE(split_set, Fixtures_d)
...
BOOST_AUTO_TEST_SUITE_END();