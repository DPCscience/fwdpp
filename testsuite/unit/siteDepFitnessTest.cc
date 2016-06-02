/*!
  \file siteDepFitness.cc
  \brief Unit tests of KTfwd::site_dependent_fitness
  \ingroup unit
*/

#include <config.h>
#include <boost/test/unit_test.hpp>
#include <testsuite/fixtures/fwdpp_fixtures.hpp>
#include <testsuite/util/custom_dip.hpp>
#include <fwdpp/fitness_models.hpp>

using mut = KTfwd::mutation;

BOOST_FIXTURE_TEST_SUITE(test_site_dependent_fitness,standard_empty_single_deme_fixture)
/*
  This test creates a situation
  where gamete1 has a selected mutation and gamete2 does not.
  If issue #8 were to cause fitness to be mis-calculated,
  then this test will fail.

  However, it does not.  Even with the bug, the remaining bit of the function
  gets the calculation right.  Yay!
*/
BOOST_AUTO_TEST_CASE( simple_multiplicative1 )
{
  KTfwd::gamete g1(1),g2(1);
  

  //add mutation at position 0.1, s=0.1,n=1,dominance=0.5 (but we won't use the dominance...)
  mutations.emplace_back(0.1,0.1,0.5);
  g1.smutations.emplace_back(0);
  BOOST_CHECK_EQUAL(g1.smutations.size(),1);
  
  gcont_t g{g1,g2};
  double w = KTfwd::site_dependent_fitness()(g[0],g[1],mutations,
					     [&](double & fitness,const mut & __mut)
					     {
					       fitness *= std::pow(1. + __mut.s,2.);
					     },
					     [&](double & fitness,const mut & __mut)
					     {
					       fitness *= (1. + __mut.s);
					     },
					     1.);
  
  BOOST_CHECK_EQUAL(w,1.1);
}

/*
  g2 has it, g1 does not
*/
BOOST_AUTO_TEST_CASE( simple_multiplicative2 )
{
  KTfwd::gamete g1(1),g2(1);
  

  //add mutation at position 0.1, s=0.1,n=1,dominance=0.5 (but we won't use the dominance...)
  mutations.emplace_back( 0.1,0.1,0.5 );
  g2.smutations.emplace_back(0);
  BOOST_CHECK_EQUAL(g1.smutations.size(),0);
  BOOST_CHECK_EQUAL(g2.smutations.size(),1);

  gcont_t g{g1,g2};

  double w = KTfwd::site_dependent_fitness()(g[0],g[1],mutations,
					     [&](double & fitness,const mut & __mut)
					     {
					       fitness *= std::pow(1. + __mut.s,2.);
					     },
					     [&](double & fitness,const mut & __mut)
					     {
					       fitness *= (1. + __mut.s);
					     },
					     1.);
  BOOST_CHECK_EQUAL(w,1.1);
}

/*
  Both have it
*/
BOOST_AUTO_TEST_CASE( simple_multiplicative3 )
{
  KTfwd::gamete g1(1),g2(1);
  

  //add mutation at position 0.1, s=0.1,n=1,dominance=0.5 (but we won't use the dominance...)
  mutations.emplace_back(0.1,0.1,0.5 );
  g1.smutations.emplace_back(0);
  g2.smutations.emplace_back(0);

  BOOST_CHECK_EQUAL(g1.smutations.size(),1);
  BOOST_CHECK_EQUAL(g2.smutations.size(),1);

  gcont_t g{g1,g2};

  double w = KTfwd::site_dependent_fitness()(g[0],g[1],mutations,
					     [&](double & fitness,const mut & __mut)
					     {
					       fitness *= std::pow(1. + __mut.s,2.);
					     },
					     [&](double & fitness,const mut & __mut)
					     {
					       fitness *= (1. + __mut.s);
					     },
					     1.);
  BOOST_CHECK_EQUAL(w,1.1*1.1);
}

/*
  Now, g1 has 2 mutations
*/
BOOST_AUTO_TEST_CASE( simple_multiplicative4 )
{
  KTfwd::gamete g1(1),g2(1);
  

  //add mutation at position 0.1, s=0.1,n=1,dominance=0.5 (but we won't use the dominance...)
  mutations.emplace_back(0.1,0.1,0.5 );
  mutations.emplace_back(0.2,0.1,0.5);
  g1.smutations.emplace_back(0);
  g1.smutations.emplace_back(1);
  BOOST_CHECK_EQUAL(g1.smutations.size(),2);

  gcont_t g{g1,g2};

  double w = KTfwd::site_dependent_fitness()(g[0],g[1],mutations,
					     [&](double & fitness,const mut & __mut)
					     {
					       fitness *= std::pow(1. + __mut.s,2.);
					     },
					     [&](double & fitness,const mut & __mut)
					     {
					       fitness *= (1. + __mut.s);
					     },
					     1.);
  BOOST_CHECK_EQUAL(w,1.1*1.1);
}

BOOST_AUTO_TEST_CASE( simple_additive_1 )
{
  KTfwd::gamete g1(1),g2(1);
 
  //add mutation at position 0.1, s=0.1,n=1,dominance=1.0
  mutations.emplace_back(0.1,0.1,1 );
  mutations.emplace_back(0.2,0.1,1);
  g1.smutations.emplace_back(0);
  g1.smutations.emplace_back(1);
  BOOST_CHECK_EQUAL(g1.smutations.size(),2);

  gcont_t g{g1,g2};

  double w = KTfwd::additive_diploid()(g[0],g[1],mutations);
  BOOST_CHECK_EQUAL(w,1.2);  
}

/*
  API checks on fitness policies.

  Below, we test the ability to assign bound fitness models to variables
  and then reassign them.

  We make use of types defined by population objects from the "sugar" layer.
*/

BOOST_AUTO_TEST_CASE( reassign_test_1 )
{
  //This is the expected type of a fitness policy for non-custom diploids.
  //We use fwdpp's type_traits header to pull this out.
  using fitness_model_t = KTfwd::traits::fitness_fxn_type<dipvector_t,gcont_t,mcont_t>::type;
  
  {
    //Test reassignment of the SAME fitness model type
    //Multiplicative model first
    
    //assign a fitness model with default scaling = 1.
    fitness_model_t dipfit = std::bind(KTfwd::multiplicative_diploid(),
				       std::placeholders::_1,
				       std::placeholders::_2,
				       std::placeholders::_3);
    
    //Now, reassign it with scaling = 2.
    dipfit = std::bind(KTfwd::multiplicative_diploid(),
		       std::placeholders::_1,
		       std::placeholders::_2,
		       std::placeholders::_3,
		       2.);
  }

  {
    //Test reassignment of the SAME fitness model type
    //Now the additive model
    
    //assign a fitness model with default scaling = 1.
    fitness_model_t dipfit = std::bind(KTfwd::additive_diploid(),
				       std::placeholders::_1,
				       std::placeholders::_2,
				       std::placeholders::_3);
    //Now, reassign it with scaling = 2.
    dipfit = std::bind(KTfwd::additive_diploid(),
		       std::placeholders::_1,
		       std::placeholders::_2,
		       std::placeholders::_3,
		       2.);
  }

  {
    //Convert a multiplicative model to an additive model
    //The application of this is a program using the library
    //that wants to decide which model to use based on parameters
    //passed in by a user.
    
    fitness_model_t dipfit = std::bind(KTfwd::multiplicative_diploid(),
				       std::placeholders::_1,
				       std::placeholders::_2,
				       std::placeholders::_3);
    
    //Now, reassign it to addtive model with scaling = 2.
    dipfit = std::bind(KTfwd::additive_diploid(),
		       std::placeholders::_1,
		       std::placeholders::_2,
		       std::placeholders::_3,
		       2.);
  }
}


BOOST_AUTO_TEST_CASE( reassign_test_2 )
{
  //Expected signature for fitness policies involving custom diploids
  //We use fwdpp's type_traits header to pull this out.
  using fitness_model_t = KTfwd::traits::fitness_fxn_type<std::vector<custom_diploid_testing_t>,gcont_t,mcont_t>::type;

  {
    //Test reassignment of the SAME fitness model type
    //Multiplicative model first

    //assign a fitness model with default scaling = 1.
    fitness_model_t dipfit = std::bind(KTfwd::multiplicative_diploid(),
					  std::placeholders::_1,
					  std::placeholders::_2,
					  std::placeholders::_3);

    //Now, reassign it with scaling = 2.
    dipfit = std::bind(KTfwd::multiplicative_diploid(),
		       std::placeholders::_1,
		       std::placeholders::_2,
		       std::placeholders::_3,
		       2.);
  }

  {
    //Test reassignment of the SAME fitness model type
    //Now the additive model
    
    //assign a fitness model with default scaling = 1.
    fitness_model_t dipfit = std::bind(KTfwd::additive_diploid(),
					  std::placeholders::_1,
					  std::placeholders::_2,
					  std::placeholders::_3);
    //Now, reassign it with scaling = 2.
    dipfit = std::bind(KTfwd::additive_diploid(),
		       std::placeholders::_1,
		       std::placeholders::_2,
		       std::placeholders::_3,
		       2.);
  }

  {
    //Convert a multiplicative model to an additive model
    //The application of this is a program using the library
    //that wants to decide which model to use based on parameters
    //passed in by a user.
    
    fitness_model_t dipfit = std::bind(KTfwd::multiplicative_diploid(),
					  std::placeholders::_1,
					  std::placeholders::_2,
					  std::placeholders::_3);

    //Now, reassign it to addtive model with scaling = 2.
    dipfit = std::bind(KTfwd::additive_diploid(),
		       std::placeholders::_1,
		       std::placeholders::_2,
		       std::placeholders::_3,
		       2.);
  }
}


BOOST_AUTO_TEST_SUITE_END()
