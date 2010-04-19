#ifndef __BOOS_INTROSPECTION__HAS_MEMBER_DATA_HPP__INCLUDED
#define __BOOS_INTROSPECTION__HAS_MEMBER_DATA_HPP__INCLUDED

#include <boost/mpl/bool.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/introspection/traits.hpp>

////////////////////////////////////////////////////////////////////////////////
// Generate a traits class that detect if a given type X has a member data
// named Name of type Type. This class can only detects public member data or
// member data that have been explicitly registered as 'traits visible' by the
// BOOST_INTROSPECTION_SUPPORT macro.
////////////////////////////////////////////////////////////////////////////////
#define BOOST_HAS_MEMBER_DATA(Type,Name)                                \
namespace boost                                                         \
{                                                                       \
  namespace introspection                                               \
  {                                                                     \
    template<class T> struct BOOST_PP_CAT(has_member_data_,Name)        \
    {                                                                   \
      typedef char NotFound;                                            \
      struct Found { char x[2]; };                                      \
                                                                        \
      template< class X, Type X::*> struct member {};                   \
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
// Used in a class, make the member data Name visible by the boost::introspection
// traits class.
////////////////////////////////////////////////////////////////////////////////
#define BOOST_INTROSPECTION_SUPPORT(Class,Name)                              \
friend class boost::introspection::BOOST_PP_CAT(has_member_data_,Name)<Class>\
/**/

#endif
