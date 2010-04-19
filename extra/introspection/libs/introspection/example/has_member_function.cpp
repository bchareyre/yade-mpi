#include <string>
#include <vector>
#include <boost/introspection/has_member_function.hpp>

using namespace boost::introspection;

struct foo
{
  void go();
  char& done(long**,long**);
};

struct bar
{
  void go() const;
  size_t size() const;
};

BOOST_HAS_MEMBER_FUNCTION(go,void());
BOOST_HAS_CONST_MEMBER_FUNCTION(size,size_t());
BOOST_HAS_NON_CONST_MEMBER_FUNCTION(done,char& (long**,long**));

int main()
{
  // BOOST_HAS_MEMBER_FUNCTION enables check for both
  // const and non-const member functions
  BOOST_MPL_ASSERT(( has_non_const_member_function_go<foo> ));
  BOOST_MPL_ASSERT_NOT(( has_non_const_member_function_go<bar> ));
  BOOST_MPL_ASSERT_NOT(( has_non_const_member_function_go<std::string> ));

  BOOST_MPL_ASSERT_NOT(( has_const_member_function_go<foo> ));
  BOOST_MPL_ASSERT(( has_const_member_function_go<bar> ));
  BOOST_MPL_ASSERT_NOT(( has_onst_member_function_go<std::string> ));

  // BOOST_HAS_CONST_MEMBER_FUNCTION enables check for const member functions only
  BOOST_MPL_ASSERT_NOT(( has_const_member_function_size<foo> ));
  BOOST_MPL_ASSERT(( has_const_member_function_size<bar> ));
  BOOST_MPL_ASSERT(( has_const_member_function_size<std::string> ));

  // BOOST_HAS_NON_CONST_MEMBER_FUNCTION enables check for non-const member functions only
  BOOST_MPL_ASSERT(( has_non_const_member_function_done<foo> ));
  BOOST_MPL_ASSERT_NOT(( has_non_const_member_function_done<bar> ));
  BOOST_MPL_ASSERT_NOT(( has_non_const_member_function_done<std::string> ));

  return 0;
}
