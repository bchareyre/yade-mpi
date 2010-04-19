#ifndef __BOOS_INTROSPECTION__TRAITS_HPP__INCLUDED
#define __BOOS_INTROSPECTION__TRAITS_HPP__INCLUDED

#include <boost/function_types/result_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/member_function_pointer.hpp>
#include <boost/function_types/property_tags.hpp>

namespace boost
{
  namespace introspection
  {
    ////////////////////////////////////////////////////////////////////////////
    // Build a MPL sequence correspondign to the components of a member function
    // type of class X with signature similar to Prototype.
    // E.g :
    // function_to_member<foo, void(int,long)>::type => <void,foo*,int,long>
    //
    ////////////////////////////////////////////////////////////////////////////
    template<class X, class Prototype> struct function_to_member
    {
      typedef typename function_types::result_type<Prototype>::type     result;
      typedef typename function_types::parameter_types<Prototype>::type args;
      typedef typename mpl::push_front<args,X*>::type                   base;
      typedef typename mpl::push_front<base,result>::type               type;
    };

    ////////////////////////////////////////////////////////////////////////////
    // Turn a class and a function type into a member function pointer with the
    // same signature.
    ////////////////////////////////////////////////////////////////////////////
    template<class X, class Prototype> struct build_member_type
    {
      typedef typename function_to_member<X,Prototype>::type                root;
      typedef typename function_types::member_function_pointer<root>::type  type;
    };

    ////////////////////////////////////////////////////////////////////////////
    // Turn a class and a function type into a const member function pointer
    // with the same signature.
    ////////////////////////////////////////////////////////////////////////////
    template<class X, class Prototype> struct build_const_member_type
    {
      typedef typename function_to_member<X,Prototype>::type               root;
      typedef typename function_types::member_function_pointer<root,function_types::const_qualified>::type type;
    };
  }
}

#endif
