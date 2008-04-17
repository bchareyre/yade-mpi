


#include "TesselationWrapper.h"
#include <iostream>

using namespace std;

int main ( int argc, char *argv[ ], char *envp[ ] )
{

	
	
	
	
	TesselationWrapper T1;
	cout << T1.insert(0   ,0   ,0   ,0.1,6) << endl;
	cout << T1.insert(0   ,0.2 ,0   ,0.1,7) << endl;
 	cout << T1.insert(0   ,0   ,0.2 ,0.1,8) << endl;
 	cout << T1.insert(0   ,0.2 ,0.2 ,0.1,9) << endl;
 	cout << T1.insert(0.2 ,0.01,0   ,0.1,10) << endl;
 	cout << T1.insert(0.2 ,0.2 ,0   ,0.1,11) << endl;
 	cout << T1.insert(0.2 ,0.2 ,0.2 ,0.1,12) << endl;
 	cout << T1.insert(0.2 ,0   ,0.2 ,0.1,13) << endl;
 	cout << T1.insert(0.1 ,0.1   ,0.1 ,0.01,14) << endl;
	
	T1.ComputeVolumes();
	
	for (int i =1; i<15; ++i) {
	cout << "sphere " << i << " v=" << T1.Volume(i) << endl;
	}
// 	cout << "sphere 1 : v=" << T1.Volume(2) << endl;
// 	cout << "sphere 1 : v=" << T1.Volume(3) << endl;
// 	cout << "sphere 1 : v=" << T1.Volume(4) << endl;
// 	cout << "sphere 1 : v=" << T1.Volume(5) << endl;
// 	cout << "sphere 1 : v=" << T1.Volume(6) << endl;
	
	
	//T1.Compute();
	//T1.ComputePorosity();
	
}
