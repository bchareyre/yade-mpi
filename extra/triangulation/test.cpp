


#include "TesselationWrapper.h"
#include <iostream>

using namespace std;

int main ( int argc, char *argv[ ], char *envp[ ] )
{

	
	
/*0.2071 0.2071 0.2071 0.2071
0.2071 0.7929 0.7929 0.2071
0.7929 0.2071 0.7929 0.2071
0.7929 0.7929 0.2071 0.2071
0.7929 0.2071 0.2071 0.2071
0.2071 0.2071 0.7929 0.2071
0.7929 0.7929 0.7929 0.2071
0.2071 0.7929 0.2071 0.2071
0.5000 0.5000 0.2071 0.2071
0.5000 0.5000 0.7929 0.2071
0.5000 0.2071 0.5000 0.2071

0.2071 0.5000 0.5000 0.2071
0.7929 0.5000 0.5000 0.2071
0.5000 0.7929 0.5000 0.2071*/	
	
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

	cout << T1.insert(0.2071, 0.2071, 0.2071, 0.2071,6) << endl;
	cout << T1.insert(0.7929, 0.2071, 0.7929, 0.2071,7) << endl;
	cout << T1.insert(0.7929, 0.7929, 0.2071, 0.2071,8) << endl;
	cout << T1.insert(0.7929, 0.2071, 0.2071, 0.2071,9) << endl;
	cout << T1.insert(0.2071, 0.2071, 0.7929, 0.2071,10) << endl;
	cout << T1.insert(0.2071, 0.7929, 0.2071, 0.2071,11) << endl;
	cout << T1.insert(0.5000, 0.5000, 0.2071, 0.2071,12) << endl;
	cout << T1.insert(0.5000, 0.5000, 0.7929, 0.2071,13) << endl;
	cout << T1.insert(0.5000, 0.2071, 0.5000, 0.2071,14) << endl;

	cout << T1.insert(0.2071, 0.5000, 0.5000, 0.2071,15) << endl;

	cout << T1.insert(0.7929, 0.5000, 0.5000, 0.2071,16) << endl;

	cout << T1.insert(0.5000, 0.7929, 0.5000, 0.2071,17) << endl;
	
//  	T1.ComputeVolumes();

	T1.ComputeVolumes(0, 1.0, 0, 1.0, 0, 1.0, 0.00001);

	
	cout << "sphere icicicicici     =    " << T1.NumberOfFacets() << endl ;


	
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
	return 0;
	
}
