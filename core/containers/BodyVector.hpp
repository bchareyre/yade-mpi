// © 2009 Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/BodyContainer.hpp>
#include<vector>

class Body;

class BodyVectorIterator: public BodyContainerIterator {
	public:
	std::vector<shared_ptr<Body> >::iterator i;
	BodyVectorIterator(): BodyContainerIterator(){}
	virtual bool isDifferent(const BodyContainerIterator& ii){ return i!=static_cast<const BodyVectorIterator&>(ii).i; }
	virtual void increment(){ ++i; }
	virtual void affect(const BodyContainerIterator& ii){ i=static_cast<const BodyVectorIterator&>(ii).i; }
	virtual shared_ptr<Body> getValue(){ return *i; }
	virtual shared_ptr<BodyContainerIterator> createPtr(){ return shared_ptr<BodyContainerIterator>(new BodyVectorIterator()); }
};

class BodyVector: public BodyContainer{
	private:
		std::vector<shared_ptr<Body> > bodies;
		unsigned int lowestFree;
	public:
		BodyVector(): lowestFree(0){}
		// non-trivial methods
		virtual unsigned int insert(shared_ptr<Body>&);
		virtual unsigned int insert(shared_ptr<Body>& b, unsigned int id);
		unsigned int findFreeId();
		// trivial methods
		virtual bool erase(unsigned int id){ if(!exists(id)) return false; lowestFree=min(lowestFree,id); bodies[id]=shared_ptr<Body>(); return true; }
		virtual void clear(){ bodies.clear(); lowestFree=0; }
		virtual bool find(unsigned int id, shared_ptr<Body>& b) const { if(!exists(id)) return false; b=bodies[id]; return true; }
		virtual bool exists(unsigned int id) const { return (id>=0) && ((size_t)id<bodies.size()) && ((bool)bodies[id]); }
		virtual shared_ptr<Body>& operator[](unsigned int id){ return bodies[id];}
		virtual const shared_ptr<Body>& operator[](unsigned int id) const { return bodies[id]; }
		virtual BodyContainer::iterator begin(){ shared_ptr<BodyVectorIterator> it(new BodyVectorIterator); it->i=bodies.begin(); return BodyContainer::iterator(it); }
		virtual BodyContainer::iterator end()  { shared_ptr<BodyVectorIterator> it(new BodyVectorIterator); it->i=bodies.end();   return BodyContainer::iterator(it); }
		virtual unsigned int size() const { return bodies.size(); }
	REGISTER_CLASS_AND_BASE(BodyVector,BodyContainer);
	REGISTER_ATTRIBUTES(BodyContainer,/* no attrs*/);
};
REGISTER_SERIALIZABLE(BodyVector);

