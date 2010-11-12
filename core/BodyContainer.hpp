// 2004 © Olivier Galizzi <olivier.galizzi@imag.fr>
// 2004 © Janek Kozicki <cosurgi@berlios.de>
// 2010 © Václav Šmilauer <eudoxos@arcig.cz>

#pragma once

#include<yade/lib/serialization/Serializable.hpp>

#include<boost/foreach.hpp>
#ifndef FOREACH
#  define FOREACH BOOST_FOREACH
#endif

class Body;

/*
Container of bodies implemented as flat std::vector. It handles body removal and
intelligently reallocates free ids for newly added ones.

Any alternative implementation should use the same API.
*/
class BodyContainer: public Serializable{
	private:
		typedef std::vector<shared_ptr<Body> > ContainerT;
		std::vector<shared_ptr<Body> > body;
		unsigned int lowestFree;
		unsigned int findFreeId();
	public:
		typedef ContainerT::iterator iterator;
		typedef ContainerT::const_iterator const_iterator;

		BodyContainer(): lowestFree(0){}
		virtual ~BodyContainer();
		unsigned int insert(shared_ptr<Body>&);
		unsigned int insert(shared_ptr<Body>& b, unsigned int id);
	
		// mimick some STL api
		void clear(){ body.clear(); lowestFree=0; }
		iterator begin() { return body.begin(); }
		iterator end() { return body.end(); }
		const_iterator begin() const { return body.begin(); }
		const_iterator end() const { return body.end(); }
		unsigned int size() const { return body.size(); }
		shared_ptr<Body>& operator[](unsigned int id){ return body[id];}
		const shared_ptr<Body>& operator[](unsigned int id) const { return body[id]; }

		bool exists(unsigned int id) const { return (id>=0) && ((size_t)id<body.size()) && ((bool)body[id]); }
		bool erase(unsigned int id){ if(!exists(id)) return false; lowestFree=min(lowestFree,id); body[id]=shared_ptr<Body>(); return true; }

		REGISTER_CLASS_AND_BASE(BodyContainer,Serializable);
		REGISTER_ATTRIBUTES(Serializable,(body));
};
REGISTER_SERIALIZABLE(BodyContainer);
