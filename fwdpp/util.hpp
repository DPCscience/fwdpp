/// \file util.hpp
#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include <fwdpp/forward_types.hpp>
#include <fwdpp/fwd_functional.hpp>
#include <fwdpp/type_traits.hpp>
#include <fwdpp/internal/gsl_discrete.hpp>
#include <set>
#include <map>
#include <type_traits>
#include <algorithm>
#include <functional>
#include <cassert>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

namespace KTfwd
{
  /* \brief Sets mutation::checked to false
   */
  template<typename mutation_type,
  	   typename list_type_allocator,
  	   template <typename,typename> class list_type>
  void uncheck( list_type<mutation_type,list_type_allocator> * mutations )
  {
    static_assert( typename traits::is_mutation_t<mutation_type>::type(),
                   "mutation_type must be derived from KTfwd::mutation_base" );
    std::for_each(mutations->begin(),mutations->end(),[](mutation_type & __m){__m.checked=false;});
  }
  
  /*! \brief Remove mutations from population
    Removes mutations that are lost.

    \note This prevents object recycling!

    \deprecated
  */
  template<typename mutation_type,
  	   typename list_type_allocator,
  	   template <typename,typename> class list_type >
  void remove_lost( list_type<mutation_type,list_type_allocator> * mutations )
  {
    static_assert( typename traits::is_mutation_t<mutation_type>::type(),
                   "mutation_type must be derived from KTfwd::mutation_base" );
    for(auto i = mutations->begin(); i != mutations->end() ; )
      {
  	if(!i->checked)
  	  {
  	    i = mutations->erase(i);
  	  }
  	else
  	  {
  	    i->checked = false;
  	    ++i;
  	  }
      }
  }

  /*! \brief Remove mutations from population
    Removes mutations that are lost.

    This prevents object recycling!

    \deprecated

    \note: lookup must be compatible with lookup->erase(lookup->find(double))
  */
  template<typename mutation_type,
  	   typename list_type_allocator,
  	   template <typename,typename> class list_type,
  	   typename mutation_lookup_table>
  void remove_lost( list_type<mutation_type,list_type_allocator> * mutations, mutation_lookup_table * lookup )
  {
    static_assert( typename traits::is_mutation_t<mutation_type>::type(),
                   "mutation_type must be derived from KTfwd::mutation_base" );
    for(auto i = mutations->begin() ; i != mutations->end() ; )
      {
  	if(!i->checked)
  	  {
  	    lookup->erase(lookup->find(i->pos));
  	    i=mutations->erase(i);
  	  }
  	else
  	  {
  	    i->checked=false;
  	    ++i;
  	  }
      }
  }

  /*! \brief Remove mutations from population
    Removes mutations that are fixed or lost.

    This prevents object recycling!

    \deprecated
  */
  template<typename mutation_type,
  	   typename vector_type_allocator1,
  	   typename vector_type_allocator2,
  	   typename list_type_allocator,
  	   template <typename,typename> class vector_type,
  	   template <typename,typename> class list_type >
  void remove_fixed_lost( list_type<mutation_type,list_type_allocator> * mutations, 
  			  vector_type<mutation_type,vector_type_allocator1> * fixations, 
  			  vector_type<unsigned,vector_type_allocator2> * fixation_times,
  			  const unsigned & generation,const unsigned & twoN)
  {
    static_assert( typename traits::is_mutation_t<mutation_type>::type(),
  		   "mutation_type must be derived from KTfwd::mutation_base" );
    for(auto i = mutations->begin() ; i != mutations->end() ; )
      {
  	assert(i->n <= twoN);			
  	if(i->n==twoN )
  	  {
  	    fixations->push_back(*i);
  	    fixation_times->push_back(generation);
  	  }
  	if( !i->checked || i->n == twoN )
  	  {
  	    i=mutations->erase(i);
  	  }
  	else
  	  {
  	    i->checked=false;
  	    ++i;
  	  }
      }
  }

  /*! \brief Remove mutations from population
    Removes mutations that are fixed or lost.

    This prevents object recycling!

    \deprecated

    \note: lookup must be compatible with lookup->erase(lookup->find(double))
  */
  template<typename mutation_type,
  	   typename vector_type_allocator1,
  	   typename vector_type_allocator2,
  	   typename list_type_allocator,
  	   template <typename,typename> class vector_type,
  	   template <typename,typename> class list_type,
  	   typename mutation_lookup_table>
  void remove_fixed_lost( list_type<mutation_type,list_type_allocator> * mutations, 
  			  vector_type<mutation_type,vector_type_allocator1> * fixations, 
  			  vector_type<unsigned,vector_type_allocator2> * fixation_times,
  			  mutation_lookup_table * lookup,
  			  const unsigned & generation,const unsigned & twoN )
  {
    static_assert( typename traits::is_mutation_t<mutation_type>::type(),
  		   "mutation_type must be derived from KTfwd::mutation_base" );
    for(auto i=mutations->begin();i!=mutations->end();)
      {
  	assert(i->n <= twoN);
  	if(i->n==twoN )
  	  {
  	    fixations->push_back(*i);
  	    fixation_times->push_back(generation);
  	  }
  	if(!i->checked ||  i->n == twoN )
  	  {
  	    lookup->erase(lookup->find(i->pos));
  	    i = mutations->erase(i);
  	  }
  	else
  	  {
  	    i->checked=false;
  	    ++i;
  	  }
      }    
  }

  /*
    Label all extinct variants for recycling

    \note: lookup must be compatible with lookup->erase(lookup->find(double))
  */
  template<typename mutation_list_type,
	   typename mutation_lookup_table>
  void update_mutations( mutation_list_type * mutations, 
			 mutation_lookup_table * lookup )
  {
    static_assert( typename traits::is_mutation_t<typename mutation_list_type::value_type>::type(),
		   "mutation_type must be derived from KTfwd::mutation_base" );
    for(auto i=mutations->begin();i!=mutations->end();++i)
      {
	if(!i->checked)
	  {
	    i->n=0;
	    auto I = lookup->find(i->pos);
	    if(I!=lookup->end())
	      lookup->erase(I);
	  }
	i->checked=false;
      }
  }

  /*!
    Label all extinct and fixed variants for recycling

    \note: lookup must be compatible with lookup->erase(lookup->find(double))
  */
  template<typename mutation_list_type,
	   typename mutation_lookup_table>
  void update_mutations( mutation_list_type * mutations, 
			 mutation_lookup_table * lookup,
			 const unsigned twoN)
  {
    static_assert( typename traits::is_mutation_t<typename mutation_list_type::value_type>::type(),
		   "mutation_type must be derived from KTfwd::mutation_base" );
    for(auto i=mutations->begin();i!=mutations->end();++i)
      {
	assert(i->n <= twoN);
	if(i->n==twoN )
	  {
	    lookup->erase(lookup->find(i->pos));
	    i->n=i->checked=0;
	  }
	else
	  {
	    if(!i->checked)
	      {
		i->n=0;
		auto I = lookup->find(i->pos);
		if(I!=lookup->end())
		  lookup->erase(I);
	      }
	  }
	i->checked=false;
      }
  }

  /*!
    Label all extinct variants for recycling, copy fixations and fixation times
    into containers.

    \note: lookup must be compatible with lookup->erase(lookup->find(double))
  */
  template<//typename mutation_type,
	   typename mutation_list_type,
	   typename fixation_container_t,
	   typename fixation_time_container_t,
	   //typename vector_type_allocator1,
	   //typename vector_type_allocator2,
	   //typename list_type_allocator,
	   //template <typename,typename> class vector_type,
	   //template <typename,typename> class list_type,
	   typename mutation_lookup_table>
  void update_mutations( mutation_list_type * mutations, 
			 fixation_container_t * fixations, 
			 fixation_time_container_t * fixation_times,
			 mutation_lookup_table * lookup,
			 const unsigned & generation,const unsigned & twoN )
  {
    static_assert( typename traits::is_mutation_t<typename mutation_list_type::value_type>::type(),
		   "mutation_type must be derived from KTfwd::mutation_base" );
    for(auto i=mutations->begin();i!=mutations->end();++i)
      {
	assert(i->n <= twoN);
	if(i->n==twoN )
	  {
	    fixations->push_back(*i);
	    fixation_times->push_back(generation);
	    lookup->erase(lookup->find(i->pos));
	    i->n=i->checked=0;
	  }
	else
	  {
	    if(!i->checked)
	      {
		i->n=0;
		auto I = lookup->find(i->pos);
		if(I!=lookup->end())
		  lookup->erase(I);
	      };
	    i->checked=false;
	  }
      }
  }
  
  /*!
    Add elements to a container via emplace_back
    
    \param c The container
    \param N final desired size of the container
    \param args Arguments for the constructor of container_type::value_type

    \note if N <= c.size(), nothing happens.
  */
  template<typename container_type,
	   class... Args>
  void add_elements( container_type & c,
		     size_t N,
		     Args&&... args )
  {
    for( size_t i = c.size() ; i < N ; ++i )
      {
	c.emplace_back(std::forward<Args>(args)...);
      }
  }
}
#endif /* _UTIL_HPP_ */
