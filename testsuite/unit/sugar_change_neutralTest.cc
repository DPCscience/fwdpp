/*!
  \file sugar_change_neutralTest.cc

  \brief test fwdpp::change_neutral
*/

#include <config.h>
#include <boost/test/unit_test.hpp>
#include <fwdpp/debug.hpp>
#include <fwdpp/sugar/add_mutation.hpp>
#include <fwdpp/sugar/change_neutral.hpp>
#include <testsuite/fixtures/sugar_fixtures.hpp>

BOOST_FIXTURE_TEST_SUITE(test_change_neutral, slocuspop_popgenmut_fixture)

BOOST_AUTO_TEST_CASE(test_change_neutral_slocuspop)
{

    fwdpp::add_mutation(pop,
                        // individuals where we want to place the mutation
                        { 0, 1, 3, 5, 7, 9 },
                        /*
                          gametes in each individual: 0 = .first, 1 = .second,
                          2 = .first and .second
                          Thus, there should be 1+1+1+2+2+1=8 copies of the
                          mutation in the population
                        */
                        { 0, 1, 0, 2, 2, 0 },
                        // Parameters to pass on to create a new mutation
                        0.1, -0.1, 1, 0);
    BOOST_REQUIRE_EQUAL(fwdpp::check_sum(pop.gametes, 2000), true);
    BOOST_REQUIRE_EQUAL(pop.gametes.size(), 2);
    BOOST_REQUIRE_EQUAL(pop.mutations.size(), 1);
    BOOST_REQUIRE_EQUAL(pop.mcounts.size(), 1);
    BOOST_REQUIRE_EQUAL(pop.mcounts[0], 8);
    BOOST_REQUIRE_EQUAL(pop.mutations[0].neutral, false);
    // Change the mutation from selected to neutral
    fwdpp::change_neutral(pop, 0);
    // Change it back
    fwdpp::change_neutral(pop, 0);
}

BOOST_AUTO_TEST_SUITE_END()
