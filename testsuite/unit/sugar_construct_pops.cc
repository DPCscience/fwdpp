// Test the construction of populations
// from user input data

#include <cassert>
#include <boost/test/unit_test.hpp>
#include <fwdpp/sugar/slocuspop.hpp>
#include <fwdpp/sugar/popgenmut.hpp>
#include <testsuite/fixtures/sugar_fixtures.hpp>

// The following structs are test fixtures

struct slocuspop_objects
{
    using poptype = slocuspop_popgenmut_fixture::poptype;

    poptype::dipvector_t diploids;
    poptype::mcont_t mutations;
    poptype::gcont_t gametes;

    using mutation_container
        = poptype::gcont_t::value_type::mutation_container;

    slocuspop_objects() : diploids{}, mutations{}, gametes{}
    {
        // Add some mutations
        for (unsigned i = 0; i < 3; ++i)
            {
                // position = i, effect size = i
                // meaning muts 1 and 2 not neutral
                mutations.emplace_back(i, i, 1, 0);
            }

        // Add two gametes
        gametes.emplace_back(1, mutation_container{ 0 },
                             mutation_container{ 1 });
        gametes.emplace_back(1, mutation_container{}, mutation_container{ 2 });

        // Add a diploid
        diploids.emplace_back(0, 1);
        assert(gametes.size() == 2);
        assert(mutations.size() == 3);
    }
};

struct mlocuspop_objects
{
    using poptype = mlocuspop_popgenmut_fixture::poptype;

    poptype::dipvector_t diploids;
    poptype::mcont_t mutations;
    poptype::gcont_t gametes;

    using mutation_container
        = poptype::gcont_t::value_type::mutation_container;

    mlocuspop_objects() : diploids{}, mutations{}, gametes{}
    {
        // Add some mutations
        for (unsigned i = 0; i < 3; ++i)
            {
                // position = i, effect size = i
                // meaning muts 1 and 2 not neutral
                mutations.emplace_back(i, i, 1, 0);
            }

        // Add two gametes
        gametes.emplace_back(1, mutation_container{ 0 },
                             mutation_container{ 1 });
        gametes.emplace_back(3, mutation_container{}, mutation_container{ 2 });

        diploids.resize(2); // two demes
        // Add a diploid
        diploids[0].emplace_back(0, 1);
        diploids[1].emplace_back(1, 1);
        assert(gametes.size() == 2);
        assert(mutations.size() == 3);
    }
};

BOOST_AUTO_TEST_SUITE(test_construct_pops)

BOOST_FIXTURE_TEST_CASE(test_slocuspop, slocuspop_objects)
{
    BOOST_REQUIRE_EQUAL(mutations[0].neutral, true);
    BOOST_REQUIRE_EQUAL(mutations[1].neutral, false);
    BOOST_REQUIRE_EQUAL(mutations[2].neutral, false);

    BOOST_REQUIRE_NO_THROW(poptype p(diploids, gametes, mutations););
}

BOOST_FIXTURE_TEST_CASE(test_slocuspop_N, slocuspop_objects)
{
    poptype p(diploids, gametes, mutations);
    BOOST_REQUIRE_EQUAL(p.N, p.diploids.size());
}

BOOST_FIXTURE_TEST_CASE(test_slocuspop_bad_gamete_counts, slocuspop_objects)
{
    // Now, make incorrect data to trigger exceptions
    diploids[0].second = 0;
    BOOST_REQUIRE_THROW(poptype p(diploids, gametes, mutations),
                        std::runtime_error);
}

BOOST_FIXTURE_TEST_CASE(test_slocuspop_bad_gamete_counts2, slocuspop_objects)
{
    // make the gamete data incorrect
    gametes[0].n++;
    BOOST_REQUIRE_THROW(poptype p(diploids, gametes, mutations),
                        std::runtime_error);
}

BOOST_FIXTURE_TEST_CASE(test_slocuspop_incorrect_mutation_storage,
                        slocuspop_objects)
{
    std::swap(gametes[0].mutations, gametes[0].smutations);
    BOOST_REQUIRE_THROW(poptype p(diploids, gametes, mutations),
                        std::logic_error);
}

BOOST_FIXTURE_TEST_CASE(test_mlocuspop, mlocuspop_objects)
{
    BOOST_REQUIRE_EQUAL(mutations[0].neutral, true);
    BOOST_REQUIRE_EQUAL(mutations[1].neutral, false);
    BOOST_REQUIRE_EQUAL(mutations[2].neutral, false);

    BOOST_REQUIRE_NO_THROW(poptype p(diploids, gametes, mutations););
}

BOOST_FIXTURE_TEST_CASE(test_mlocuspop_deme_sizes, mlocuspop_objects)
{
    poptype p(diploids, gametes, mutations);

    BOOST_REQUIRE_EQUAL(p.N, p.diploids.size());
}

BOOST_FIXTURE_TEST_CASE(test_mlocuspop_bad_gamete_counts, mlocuspop_objects)
{
    // Now, make incorrect data to trigger exceptions
    diploids[0][0].second = 0;
    BOOST_REQUIRE_THROW(poptype p(diploids, gametes, mutations),
                        std::runtime_error);
}

BOOST_FIXTURE_TEST_CASE(test_mlocuspop_bad_gamete_counts2, mlocuspop_objects)
{
    // make the gamete data incorrect
    gametes[0].n++;
    BOOST_REQUIRE_THROW(poptype p(diploids, gametes, mutations),
                        std::runtime_error);
}

BOOST_FIXTURE_TEST_CASE(test_mlocuspop_incorrect_mutation_storage,
                        mlocuspop_objects)
{
    std::swap(gametes[0].mutations, gametes[0].smutations);
    BOOST_REQUIRE_THROW(poptype p(diploids, gametes, mutations),
                        std::logic_error);
}

BOOST_AUTO_TEST_SUITE_END()
