#pragma once
#include<boost/python/object.hpp>
#include<boost/version.hpp>
#if BOOST_VERSION<103400
	// taken from boost-1.34/python/object.hpp

	// Copyright David Abrahams 2002.
	// Distributed under the Boost Software License, Version 1.0. (See
	// accompanying file LICENSE_1_0.txt or copy at
	// http://www.boost.org/LICENSE_1_0.txt)

	namespace boost { namespace python {

		 inline ssize_t len(object const& obj)
		 {
			  ssize_t result = PyObject_Length(obj.ptr());
			  if (PyErr_Occurred()) throw_error_already_set();
			  return result;
		 }

	}} // namespace boost::python
#endif // BOOST_VERSION<103400
