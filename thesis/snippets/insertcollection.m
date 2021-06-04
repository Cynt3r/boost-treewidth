#ifndef BOOST_GRAPH_INSERT_COLLECTION
#define BOOST_GRAPH_INSERT_COLLECTION
template <class C>
struct InsertCollection : Collection<C>
{
 public:
    using Container_value_type = typename C::value_type;

    BOOST_CONCEPT_USAGE(InsertCollection)
    {
        x.insert(x.begin(), e); // require insert method
    }
 private:
    C x;
    Container_value_type e;
};
#endif // BOOST_GRAPH_INSERT_COLLECTION