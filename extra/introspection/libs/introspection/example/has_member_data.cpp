
#include <string>
#include <boost/introspection/has_member_data.hpp>

using namespace boost::introspection;

BOOST_HAS_MEMBER_DATA(volatile int,value)

struct foo
{
  int value;
};

struct bar
{
  BOOST_INTROSPECTION_SUPPORT(bar,value);

  private:
  volatile int value;
};


int main()
{
  BOOST_MPL_ASSERT(( has_member_data_value<foo> ));
  BOOST_MPL_ASSERT(( has_member_data_value<bar> ));
  BOOST_MPL_ASSERT_NOT(( has_member_data_value<std::string> ));
  return 0;
}
