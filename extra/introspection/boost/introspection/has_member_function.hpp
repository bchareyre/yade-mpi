#ifndef __BOOS_INTROSPECTION__HAS_MEMBER_FUNCTION_HPP__INCLUDED
#define __BOOS_INTROSPECTION__HAS_MEMBER_FUNCTION_HPP__INCLUDED

#include <boost/mpl/bool.hpp>
#include <boost/mpl/or.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/introspection/traits.hpp>

////////////////////////////////////////////////////////////////////////////////
// Generate a traits class that detect if a given type X has a non-const member
// function named Name with a given signature Sig.
////////////////////////////////////////////////////////////////////////////////
#define BOOST_HAS_NON_CONST_MEMBER_FUNCTION(Name,Sig)                   \
namespace boost                                                         \
{                                                                       \
  namespace introspection                                               \
  {                                                                     \
    template<class T>                                                   \
    struct BOOST_PP_CAT(has_non_const_member_function_,Name)            \
    {                                                                   \
      typedef char NotFound;                                            \
      struct Found { char x[2]; };                                      \
                                                                        \
      template< class X                                                 \
              , typename build_member_type<X,Sig>::type                 \
              > struct member {};                                       \
                                                                        \
      template<class X> static Found test(member<X,&X::Name>*);         \
      template<class X> static NotFound test( ... );                    \
                                                                        \
      static const bool value  = (sizeof(Found) == sizeof(test<T>(0))); \
      typedef mpl::bool_<value> type;                                   \
    };                                                                  \
  }                                                                     \
}                                                                       \
/**/

////////////////////////////////////////////////////////////////////////////////
// Generate a traits class that detect if a given type X has a const member
// function named Name with a given signature Sig.
////////////////////////////////////////////////////////////////////////////////
#define BOOST_HAS_CONST_MEMBER_FUNCTION(Name,Sig)                       \
namespace boost                                                         \
{                                                                       \
  namespace introspection                                               \
  {                                                                     \
    template<class T>                                                   \
    struct BOOST_PP_CAT(has_const_member_function_,Name)                \
    {                                                                   \
      typedef char NotFound;                                            \
      struct Found { char x[2]; };                                      \
                                                                        \
      template< class X                                                 \
              , typename build_const_member_type<X,Sig>::type           \
              > struct member {};                                       \
                                                                        \
      template<class X> static Found test(member<X,&X::Name>*);         \
      template<class X> static NotFound test( ... );                    \
                                                                        \
      static const bool value  = (sizeof(Found) == sizeof(test<T>(0))); \
      typedef mpl::bool_<value> type;                                   \
    };                                                                  \
  }                                                                     \
}
/**/

////////////////////////////////////////////////////////////////////////////////
// Generate a traits class that detect if a given type X has a member function
// named Name with a given signature Sig which is either const or non-const
////////////////////////////////////////////////////////////////////////////////
#define BOOST_HAS_MEMBER_FUNCTION(Name,Sig)                                 \
BOOST_HAS_CONST_MEMBER_FUNCTION(Name,Sig)                                   \
BOOST_HAS_NON_CONST_MEMBER_FUNCTION(Name,Sig)                               \
                                                                            \
namespace boost                                                             \
{                                                                           \
  namespace introspection                                                   \
  {                                                                         \
    template<class T>                                                       \
    struct BOOST_PP_CAT(has_member_function_,Name) :                        \
            mpl::or_< BOOST_PP_CAT(has_const_member_function_,Name)<T>      \
                    , BOOST_PP_CAT(has_non_const_member_function_,Name)<T>  \
                    > {};                                                   \
  }                                                                         \
}                                                                           \
/**/
#endif
