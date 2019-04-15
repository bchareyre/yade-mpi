#pragma once

#include <core/Body.hpp>
#include <core/Scene.hpp>
#include <core/BodyContainer.hpp>
#include <core/Omega.hpp>
#include <lib/base/Logging.hpp>
#include <lib/serialization/Serializable.hpp>
#include <vector>

class MPIBodyContainer :  public Serializable{

  public:

    int procRank; // MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    void insertBody(int id) {
      const shared_ptr<Scene>& scene = Omega::instance().getScene();
      shared_ptr<BodyContainer>& bodycontainer = scene-> bodies;
      shared_ptr<Body> b = (*bodycontainer)[id];
      // if empty put body
      if (bContainer.size() == 0 ){ bContainer.push_back(b); }
      //check if body already exists
      else {
         int  c = 0;
        for (std::vector<shared_ptr<Body> >::iterator bodyIter=bContainer.begin(); bodyIter != bContainer.end(); ++bodyIter ){
 	        const shared_ptr<Body>& inContainerBody = *(bodyIter);
 	        if (inContainerBody->id == b->id) {c += 1;}
       } if (c==0){bContainer.push_back(b); }
      }
    }

    void clearContainer();

    virtual ~MPIBodyContainer(){};

    void insertBodyList(boost::python::list&  idList) {
      // insert a list of bodies
      unsigned int listSize = boost::python::len(idList);
      for (unsigned int i=0; i != listSize; ++i) {
	      int  b_id = boost::python::extract <int> (idList[i]);
	      insertBody(b_id);
      }
    }

    unsigned int getCount() {
      return bContainer.size();
    }

    shared_ptr<Body> getBodyat(int id){
      return bContainer[id];
    }

    void setBodiesToBodyContainer() { // to be used when deserializing a recieved container.
	    const shared_ptr<Scene>& scene = Omega::instance().getScene();
	    shared_ptr<BodyContainer>& bodyContainer = scene->bodies;
	    for (std::vector<shared_ptr <Body>> ::iterator bIter = bContainer.begin(); bIter != bContainer.end(); ++ bIter) {
	      shared_ptr<Body> newBody = *(bIter);
	      bodyContainer->insertAtId(newBody, newBody->id);
	    }
    }
  
  YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(MPIBodyContainer,Serializable,"a dummy container to serialize and send. ",
  ((vector<shared_ptr<Body>>, bContainer,,,"a dummy body container to serialize"))
  ,
  ,
  .def("insertBody",&MPIBodyContainer::insertBody,(boost::python::arg("bodyId")),  "insert a body (by id) in this container")
  .def("insertBodyList", &MPIBodyContainer::insertBodyList, (boost::python::arg("listOfIds")), "inset a list of bodies (by ids)")
  .def("clearContainer", &MPIBodyContainer::clearContainer, "clear bodies in the container")
  .def("getCount", &MPIBodyContainer::getCount, "get container count")
  .def("getBodyat", &MPIBodyContainer::getBodyat, "get body at id")
  );
  DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(MPIBodyContainer);
