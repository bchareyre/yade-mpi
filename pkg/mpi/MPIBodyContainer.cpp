#include "MPIBodyContainer.hpp"
CREATE_LOGGER(MPIBodyContainer);
YADE_PLUGIN((MPIBodyContainer));


void MPIBodyContainer::clearContainer() {
      bContainer.clear(); 
}


//string MPIBodyContainertoString(MPIBodyContainer mpiContainer) { 
//  
//    std::ostringstream oss; 
//    yade::ObjectIO::save<decltype(mpiContainer), boost::archive::binary_oarchive>(oss, "mpiContainer", mpiContainer); 
//    oss.flush(); 
//	return oss.str(); 
//  } 
//	
//
//void stringtoMPIBodyContainer(string mpiContainerString){ 	  
//  
//  MPIBodyContainer b; 	  
//  std::istringstream istr(mpiContainerString);  	
//  yade::ObjectIO::load<decltype(b),boost::archive::binary_iarchive>(istr,"mpiContainer",b);
//  std::cout << "size of b = "  << b.bContainer.size() << std::endl; 
//	
//} 
//	
