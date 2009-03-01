/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once


#include "Indexable.hpp"


#include<yade/lib-factory/ClassFactory.hpp>
#include<yade/lib-serialization/Serializable.hpp>
#include<yade/lib-multimethods/MultiMethodsExceptions.hpp>
#include<yade/lib-loki/Functor.hpp>
#include<yade/lib-loki/Typelist.hpp>
#include<yade/lib-loki/TypeManip.hpp>
#include<yade/lib-loki/NullType.hpp>


#include <vector>
#include <list>
#include <string>


using namespace std;
using namespace boost;


///
/// base classes involved in multiple dispatch must be derived from Indexable
///

/// base template for all dispatchers								///

template 
<
	class BaseClass,	//	a typelist with base classess involved in the dispatch (or single class, for 1D )
				// 		FIXME : should use shared_ptr references, like this: DynLibDispatcher< TYPELIST_2( shared_ptr<PhysicalAction>& , shared_ptr<Body>& ) , ....
	class Executor,		//	class which gives multivirtual function
	class ResultType,	//	type returned by multivirtual function
	class TList,		//	typelist of arguments passed to multivirtual function
				//	WARNING: first arguments must be shared_ptr<BaseClass>, for details see FunctorWrapper
				
	bool autoSymmetry=true	//	true -	the function called is always the same,
				//		only order of arguments is rearranged
				//		to make correct function call, 
				//		only go() is called
				//		
				//	false -	the function called is always different.
				//		arguments order is not rearranged
				//		go(), and goReverse() are called, respectively
				//
>
class DynLibDispatcher
{

		// this template recursively defines a type for callBacks matrix, with required number of dimensions
	private : template<class T > struct Matrix
		  {
			typedef Loki::NullType ResultIterator;
			typedef Loki::NullType ResultIteratorInt;
		  };

	private : template<class Head > struct Matrix< Loki::Typelist< Head, Loki::NullType > >
		  {
			typedef vector< shared_ptr< Executor > > 			Result;
			typedef vector< int > 						ResultInt;
			typedef typename vector< shared_ptr< Executor > >::iterator 	ResultIterator;
			typedef vector< int >::iterator 				ResultIteratorInt;
		  };

	private : template<class Head, class Tail >
		  struct Matrix< Loki::Typelist< Head, Tail > >
		  {
			// recursive typedef to get matrix of required dimensions
			typedef typename Matrix< Tail >::Result 		InsideType;
			typedef typename Matrix< Tail >::ResultInt 		InsideTypeInt;
			typedef vector< InsideType > 				Result;
			typedef vector< InsideTypeInt > 			ResultInt;
			typedef typename vector< InsideType >::iterator 	ResultIterator;
			typedef typename vector< InsideTypeInt >::iterator 	ResultIteratorInt;
		  };
		
		// this template helps declaring iterators for each dimension of callBacks matrix
	private : template<class T > struct GetTail
		  {
			typedef Loki::NullType Result;
		  };

	private : template<class Head, class Tail>
		  struct GetTail< Loki::Typelist< Head , Tail > >
		  {
			typedef Tail Result;
		  };

	private : template<class Head >
		  struct GetTail< Loki::Typelist< Head , Loki::NullType > >
		  {
			typedef Loki::NullType Result;
		  };

	private : template <typename G, typename T, typename U>
		  struct Select
		  {
			typedef T Result;
		  };
	
	private : template <typename T, typename U>
		  struct Select<Loki::NullType, T, U>
		  {
			typedef U Result;
		  };

	private : typedef typename Loki::TL::Append<  Loki::NullType , BaseClass >::Result BaseClassList;
	private : typedef typename Loki::TL::TypeAtNonStrict<BaseClassList , 0>::Result	BaseClass1;  // 1D
	private : typedef typename Loki::TL::TypeAtNonStrict<BaseClassList , 1>::Result	BaseClass2;  // 2D
	private : typedef typename Loki::TL::TypeAtNonStrict<BaseClassList , 2>::Result	BaseClass3;  // 3D
		
	private : typedef typename GetTail< BaseClassList >::Result			Tail2; // 2D
	private : typedef typename GetTail< Tail2 >::Result				Tail3; // 3D
	private : typedef typename GetTail< Tail3 >::Result				Tail4; // 4D ...
		
	private : typedef typename Matrix< BaseClassList >::ResultIterator 		Iterator2; // outer iterator 2D
	private : typedef typename Matrix< Tail2 >::ResultIterator			Iterator3; // inner iterator 3D
	private : typedef typename Matrix< Tail3 >::ResultIterator			Iterator4; // more inner iterator 4D
		
	private : typedef typename Matrix< BaseClassList >::ResultIteratorInt		IteratorInfo2;
	private : typedef typename Matrix< Tail2 >::ResultIteratorInt			IteratorInfo3;
	private : typedef typename Matrix< Tail3 >::ResultIteratorInt			IteratorInfo4;
		
	private : typedef typename Matrix< BaseClassList >::Result MatrixType;
	private : typedef typename Matrix< BaseClassList >::ResultInt MatrixIntType;
	private : MatrixType callBacks;		// multidimensional matrix that stores functors ( 1D, 2D, 3D, 4D, ....)
	private : MatrixIntType callBacksInfo;	// multidimensional matrix for extra information about functors in the matrix
						// currently used to remember if it is reversed functor
						
	// ParmNReal is defined to avoid ambigious function call for different dimensions of multimethod
	private : typedef Loki::FunctorImpl<ResultType, TList > Impl;
	private : typedef TList ParmList;
	private : typedef Loki::NullType Parm1; 						// it's always at least 1D
	private : typedef typename Impl::Parm2 Parm2Real;
	private : typedef typename Select< Tail2 , Loki::NullType , Parm2Real >::Result Parm2; 	// 2D
	private : typedef typename Impl::Parm3 Parm3Real;
	private : typedef typename Select< Tail3 , Loki::NullType , Parm3Real >::Result Parm3; 	// 3D - to have 3D just working, without symmetry handling - change this line to be: typedef typename Impl::Parm3 Parm3;
	private : typedef typename Impl::Parm4 Parm4Real;
	private : typedef typename Select< Tail4 , Loki::NullType , Parm4Real >::Result Parm4;	// 4D - same as above
	private : typedef typename Impl::Parm5 Parm5;
	private : typedef typename Impl::Parm6 Parm6;
	private : typedef typename Impl::Parm7 Parm7;
	private : typedef typename Impl::Parm8 Parm8;
	private : typedef typename Impl::Parm9 Parm9;
	private : typedef typename Impl::Parm10 Parm10;
	private : typedef typename Impl::Parm11 Parm11;
	private : typedef typename Impl::Parm12 Parm12;
	private : typedef typename Impl::Parm13 Parm13;
	private : typedef typename Impl::Parm14 Parm14;
	private : typedef typename Impl::Parm15 Parm15;
	
	// Serialization stuff.. - FIXME - maybe this should be done in separate class...
//		bool deserializing;
// //	protected:
// // 		vector<vector<string> >	functorNames;
// // 		list<shared_ptr<Executor> > functorArguments;
// // 		typedef typename list<shared_ptr<Executor> >::iterator executorListIterator;
			
// 			virtual void registerAttributes()
// 			{
// 				REGISTER_ATTRIBUTE(functorNames);
// 				if(functors.size() != 0)
// 					REGISTER_ATTRIBUTE(functors);
// 			}
			
 	public  : DynLibDispatcher()
		  {
			// FIXME - static_assert( typeid(BaseClass1) == typeid(Parm1) ); // 1D
			// FIXME - static_assert( typeid(BaseClass2) == typeid(Parm2) ); // 2D
			callBacks.clear();
			callBacksInfo.clear();
		  };
		
 	public  : shared_ptr<Executor> getExecutor(const string& baseClassName1,const string& baseClassName2)
		  {

			shared_ptr<BaseClass1> baseClass1 = YADE_PTR_CAST<BaseClass1>(ClassFactory::instance().createShared(baseClassName1));
			shared_ptr<Indexable> base1	  = YADE_PTR_CAST<Indexable>(baseClass1);
			assert(base1);
			int& index1 = base1->getClassIndex();
 			assert (index1 != -1);

			shared_ptr<BaseClass2> baseClass2 = YADE_PTR_CAST<BaseClass2>(ClassFactory::instance().createShared(baseClassName2));
			shared_ptr<Indexable> base2	  = YADE_PTR_CAST<Indexable>(baseClass2);
			assert(base2);
			int& index2 = base2->getClassIndex();
 			assert (index2 != -1);

			assert(callBacks.size()>=(unsigned int)index1);
			assert(callBacks[index1].size()>=(unsigned int)index2);

			return callBacks[index1][index2];
		  }

 	public  : shared_ptr<Executor> getExecutor(const string& baseClassName)
		  {

			shared_ptr<BaseClass1> baseClass = YADE_PTR_CAST<BaseClass1>(ClassFactory::instance().createShared(baseClassName));
			shared_ptr<Indexable> base	  = YADE_PTR_CAST<Indexable>(baseClass);
			assert(base);
			int& index = base->getClassIndex();
 			assert (index != -1);

			assert(callBacks.size()>=(unsigned int)index);

			return callBacks[index];
		  }

 	public  : shared_ptr<Executor> makeExecutor(string libName)
		  {
			shared_ptr<Executor> executor;
			try
			{
				executor = dynamic_pointer_cast<Executor>(ClassFactory::instance().createShared(libName));
				if (!executor){ //dynamic_cast_failed for some reason so try with static_cast
					// cerr<<__FILE__<<":"<<__LINE__<<" ERROR dynamic_casting executor of type "<<libName<<", using static_cast. WHY?"<<endl;
					executor = static_pointer_cast<Executor>(ClassFactory::instance().createShared(libName));
				}
				else cerr<<"COOL! Dynamic cast of type "<<libName<<" successful!"<<endl;
			}
			catch (FactoryCantCreate& fe)
			{
				string error = string(fe.what()) + " -- " + MultiMethodsExceptions::NotExistingClass + libName;	
				throw MultiMethodsNotExistingClass(error.c_str());
			}

			assert(executor);
			
			return executor;
		  }
		
// // 		void storeFunctorArguments(shared_ptr<Executor>& ex)
// // 		{
// // 			if(! ex) return;
// // 			bool dupe = false;
// // 			executorListIterator it    = functorArguments.begin();
// // 			executorListIterator itEnd = functorArguments.end();
// // 			for( ; it != itEnd ; ++it )
// // 				if( (*it)->getClassName() == ex->getClassName() )
// // 					dupe = true;
// // 			
// // 			if(! dupe) functorArguments.push_back(ex);
// // 		}
// // 		
// // 		shared_ptr<Executor> findFunctorArguments(string libName)
// // 		{
// // 			executorListIterator it    = functorArguments.begin();
// // 			executorListIterator itEnd = functorArguments.end();
// // 			for( ; it != itEnd ; ++it )
// // 				if( (*it)->getClassName() == libName )
// // 					return *it;
// // 			
// // 			return shared_ptr<Executor>();
// // 		}

// add multivirtual function to 1D
// // 		void postProcessDispatcher1D(bool d)
// // 		{
// // 			if(d)
// // 			{
// // 				deserializing = true;
// // 				for(unsigned int i=0;i<functorNames.size();i++)
// // 					add(functorNames[i][0],functorNames[i][1],findFunctorArguments(functorNames[i][1]));
// // 				deserializing = false;
// // 			}
// // 		}
		
// // 		void storeFunctorName(	  const string& baseClassName
// // 					, const string& libName
// // 					, shared_ptr<Executor>& ex)
// // 		{
// // 			vector<string> v;
// // 			v.push_back(baseClassName);
// // 			v.push_back(libName);
// // 			functorNames.push_back(v);
// // 			storeFunctorArguments(ex);
// // 		}
		
 	public  : void add1DEntry( string baseClassName, string libName, shared_ptr<Executor> ex = shared_ptr<Executor>())
		  {

			// create base class, to access its index. (we can't access static variable, because
			// the class might not exist in memory at all, and we have to load dynamic library,
			// so that a static variable is created and accessible)
			shared_ptr<BaseClass1> baseClass = 
				YADE_PTR_CAST<BaseClass1>(ClassFactory::instance().createShared(baseClassName));
			// this is a strange tweak without which it won't work.
			shared_ptr<Indexable> base = YADE_PTR_CAST<Indexable>(baseClass);
		
			assert(base);
			int& index = base->getClassIndex();
			if(index == -1)
				std::cerr << "--------> Did you forget to call createIndex(); in constructor?\n";
 			assert (index != -1);
			
			int maxCurrentIndex = base->getMaxCurrentlyUsedClassIndex();
			callBacks.resize( maxCurrentIndex+1 );	// make sure that there is a place for new Functor

			shared_ptr<Executor> executor = ex ? ex : makeExecutor(libName);	// create the requested functor
			callBacks[index] = executor;
						
			#if 0
				cerr <<" New class added to DynLibDispatcher 1D: " << libName << endl;
			#endif
		  };
		
 	public  : bool locateMultivirtualFunctor1D(int& index, shared_ptr<BaseClass1>& base)
		  {
			index = base->getClassIndex();
			assert( index >= 0 && (unsigned int)( index ) < callBacks.size());
			if( callBacks[index] )
				return true;
			
			int depth=1;
			int index_tmp = base->getBaseClassIndex(depth);
			while(1)
				if(index_tmp == -1)
					return false;
				else
					if(callBacks[index_tmp])
					{
						// BEGIN FIXME - this should be a separate function to resize stuff
						//cerr << index << " " << index_tmp << endl;
						if( callBacksInfo.size() <= (unsigned int)index )
							callBacksInfo.resize(index+1);
						if( callBacks.size() <= (unsigned int)index )
							callBacks.resize(index+1);
						// END
						callBacksInfo[index] = callBacksInfo[index_tmp];
						callBacks    [index] = callBacks    [index_tmp];
						return true;
					}
					else
						index_tmp = base->getBaseClassIndex(++depth);
		
			return false; // FIXME - this line should be not needed
		  }

		
// add multivirtual function to 2D
// // 		void postProcessDispatcher2D(bool d)
// // 		{
// // 			if(d)
// // 			{
// // 				deserializing = true;
// // 				for(unsigned int i=0;i<functorNames.size();i++)
// // 					add(functorNames[i][0],functorNames[i][1],functorNames[i][2],findFunctorArguments(functorNames[i][2]));
// // 				deserializing = false;
// // 			}
// // 		}
		
// // 		void storeFunctorName(	  const string& baseClassName1
// // 					, const string& baseClassName2
// // 					, const string& libName
// // 					, shared_ptr<Executor>& ex) // 2D
// // 		{
// // 			vector<string> v;
// // 			v.push_back(baseClassName1);
// // 			v.push_back(baseClassName2);
// // 			v.push_back(libName);
// // 			functorNames.push_back(v);
// // 			storeFunctorArguments(ex);
// // 		}
		
	public  : void add2DEntry( string baseClassName1, string baseClassName2, string libName, shared_ptr<Executor> ex = shared_ptr<Executor>())
		  {
			shared_ptr<BaseClass1> baseClass1 = YADE_PTR_CAST<BaseClass1>(ClassFactory::instance().createShared(baseClassName1));
			shared_ptr<BaseClass2> baseClass2 = YADE_PTR_CAST<BaseClass2>(ClassFactory::instance().createShared(baseClassName2));
			shared_ptr<Indexable> base1 = YADE_PTR_CAST<Indexable>(baseClass1);
			shared_ptr<Indexable> base2 = YADE_PTR_CAST<Indexable>(baseClass2);
			
			assert(base1);
			assert(base2);

 			int& index1 = base1->getClassIndex();
			if(index1 == -1)
				std::cerr << "--------> Did you forget to call createIndex(); in constructor?\n";
			assert (index1 != -1);
 			
			int& index2 = base2->getClassIndex();
			if(index2 == -1)
				std::cerr << "--------> Did you forget to call createIndex(); in constructor?\n";
 			assert(index2 != -1);
	
			if( typeid(BaseClass1) == typeid(BaseClass2) )
				assert(base1->getMaxCurrentlyUsedClassIndex() == base2->getMaxCurrentlyUsedClassIndex());
	
			int maxCurrentIndex1 = base1->getMaxCurrentlyUsedClassIndex();
			int maxCurrentIndex2 = base2->getMaxCurrentlyUsedClassIndex();

			callBacks.resize( maxCurrentIndex1+1 );		// resizing callBacks table
			callBacksInfo.resize( maxCurrentIndex1+1 );
			for( Iterator2 ci = callBacks.begin() ; ci != callBacks.end() ; ++ci )
				ci->resize(maxCurrentIndex2+1);
			for( IteratorInfo2 cii = callBacksInfo.begin() ; cii != callBacksInfo.end() ; ++cii )
				cii->resize(maxCurrentIndex2+1);
	
			shared_ptr<Executor> executor = ex ? ex : makeExecutor(libName);	// create the requested functor
		
			if( typeid(BaseClass1) == typeid(BaseClass2) ) // both base classes are the same
			{
				callBacks	[index2][index1] = executor;
				callBacks	[index1][index2] = executor;
				
				string order		= baseClassName1 + " " + baseClassName2;
				string reverseOrder	= baseClassName2 + " " + baseClassName1;
				
				if( autoSymmetry || executor->checkOrder() == order ) // if you want autoSymmetry, you don't have to DEFINE_FUNCTOR_ORDER_2D
				{
					callBacksInfo	[index2][index1] = 1; // this is reversed call
					callBacksInfo	[index1][index2] = 0;
				}
				else if( executor->checkOrder() == reverseOrder )
				{
					callBacksInfo	[index2][index1] = 0;
					callBacksInfo	[index1][index2] = 1; // this is reversed call
				} else
				{
					string err = MultiMethodsExceptions::UndefinedOrder + libName;
					throw MultiMethodsUndefinedOrder(err.c_str());
				}
			}
			else // classes are different, no symmetry possible
			{
				callBacks	[index1][index2] = executor;
				callBacksInfo	[index1][index2] = 0;
			}

			#if 0
				cerr <<" New class added to MultiMethodsManager 2D: " << libName << endl;
			#endif
		  }
		
	public:
		/* Return pointer to the functor for two base classes given. Swap is true if the dispatch objects
		 * should be swapped before calling Executor::go.
		 */
		shared_ptr<Executor> getFunctor2D(shared_ptr<BaseClass1>& base1, shared_ptr<BaseClass2>& base2, bool& swap){
			int ix1, ix2;
			if(!locateMultivirtualFunctor2D(ix1,ix2,base1,base2)){
				return shared_ptr<Executor>();
			}
			swap=(bool)(callBacksInfo[ix1][ix2]);
			return callBacks[ix1][ix2];
		}
	
	
		bool locateMultivirtualFunctor2D(int& index1, int& index2, shared_ptr<BaseClass1>& base1,shared_ptr<BaseClass2>& base2)
		  {
			index1 = base1->getClassIndex();
			index2 = base2->getClassIndex();
			assert( index1 >= 0 && (unsigned int)( index1 ) < callBacks.size() &&
				index2 >= 0 && (unsigned int)( index2 ) < callBacks[index1].size() );
				
			if(callBacks[index1][index2])
				return true;

			int depth1=1;
			int depth2=1;
			int index1_tmp = base1->getBaseClassIndex(depth1);
			int index2_tmp = base2->getBaseClassIndex(depth2);
			
			if( index1_tmp == -1 )
			{
				while(1)
				{
					if(index2_tmp == -1)
						return false;
					else
					{	if(callBacks[index1][index2_tmp]) // FIXME - this is not working, when index1 or index2 is out-of-boundary. I have to resize callBacks and callBacksInfo tables.  - this should be a separate function to resize stuff
						{ 
							callBacksInfo[index1][index2] = callBacksInfo[index1][index2_tmp];
							callBacks    [index1][index2] = callBacks    [index1][index2_tmp];
//							index2 = index2_tmp;
							return true;
						}
						else
							index2_tmp = base2->getBaseClassIndex(++depth2);
					}
				}
			}
			else if( index2_tmp == -1 )
			{
				while(1)
				{
					if(index1_tmp == -1)
						return false;
					else
					{
						if(callBacks[index1_tmp][index2    ])
						{
							callBacksInfo[index1][index2] = callBacksInfo[index1_tmp][index2];
							callBacks    [index1][index2] = callBacks    [index1_tmp][index2];
//							index1 = index1_tmp;
							return true;
						}
						else
							index1_tmp = base1->getBaseClassIndex(++depth1);
					}
				}
			}
			else if( index1_tmp != -1 && index2_tmp != -1 )
				throw std::runtime_error("DynLibDispatcher: ambiguous or undefined dispatch for 2d multivirtual function, classes: "+base1->getClassName()+" "+base2->getClassName());
			
			return false;
		  };

		
// add multivirtual function to 3D

//  to be written when needed.... - not too difficult
// // 		void postProcessDispatcher3D(bool d)
// // 		{
// // 		}
// // 		
// // 		void storeFunctorName(const string& str1,const string& str2,const string& str3,const string& str4,shared_ptr<Executor>& ex)
// // 		{
// // 		}
// 		
// 		void add(string baseClassName1 , string baseClassName2 , string baseClassName3 , string libName)
// 		{
// 		}
// 		
// 		bool locateMultivirtualFunctor3D(int& index1, int& index2, int& index3,
// 			shared_ptr<BaseClass1>& base1, shared_ptr<BaseClass2>& base2, shared_ptr<BaseClass3>& base3 )
// 		{
// 			return false;
// 		}
		
// calling multivirtual function, 1D

		ResultType operator() (shared_ptr<BaseClass1>& base)
		{
			int index;
			if( locateMultivirtualFunctor1D(index,base) )
				return (callBacks[index])->go(base);
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base, Parm2 p2)
		{
			int index;
			if( locateMultivirtualFunctor1D(index,base) )
				return (callBacks[index])->go(base, p2);
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base, Parm2 p2, Parm3 p3)
		{
			int index;
			if( locateMultivirtualFunctor1D(index,base) )
				return (callBacks[index])->go(base, p2, p3);
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base, Parm2 p2, Parm3 p3, Parm4 p4)
		{
			int index;
			if( locateMultivirtualFunctor1D(index,base) )
				return (callBacks[index])->go(base, p2, p3, p4);
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5)
		{
			int index;
			if( locateMultivirtualFunctor1D(index,base) )
				return (callBacks[index])->go(base, p2, p3, p4, p5);
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6)
		{
			int index;
			if( locateMultivirtualFunctor1D(index,base) )
				return (callBacks[index])->go(base, p2, p3, p4, p5, p6);
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6, Parm7 p7)
		{
			int index;
			if( locateMultivirtualFunctor1D(index,base) )
				return (callBacks[index])->go(base, p2, p3, p4, p5, p6, p7);
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6, Parm7 p7, Parm8 p8)
		{
			int index;
			if( locateMultivirtualFunctor1D(index,base) )
				return (callBacks[index])->go(base, p2, p3, p4, p5, p6, p7, p8);
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9)
		{
			int index;
			if( locateMultivirtualFunctor1D(index,base) )
				return (callBacks[index])->go(base, p2, p3, p4, p5, p6, p7, p8, p9);
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9,
									Parm10 p10)
		{
			int index;
			if( locateMultivirtualFunctor1D(index,base) )
				return (callBacks[index])->go(base, p2, p3, p4, p5, p6, p7, p8, p9, p10);
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9,
									Parm10 p10, Parm11 p11)
		{
			int index;
			if( locateMultivirtualFunctor1D(index,base) )
				return (callBacks[index])->go(base, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9,
									Parm10 p10, Parm11 p11, Parm12 p12)
		{
			int index;
			if( locateMultivirtualFunctor1D(index,base) )
				return (callBacks[index])->go(base, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9,
									Parm10 p10, Parm11 p11, Parm12 p12, Parm13 p13)
		{
			int index;
			if( locateMultivirtualFunctor1D(index,base) )
				return (callBacks[index])->go(base, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9,
									Parm10 p10, Parm11 p11, Parm12 p12, Parm13 p13, Parm14 p14)
		{
			int index;
			if( locateMultivirtualFunctor1D(index,base) )
				return (callBacks[index])->go(base, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
			else	return ResultType();
		}
	
		ResultType operator() (shared_ptr<BaseClass1>& base, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9,
									Parm10 p10, Parm11 p11, Parm12 p12, Parm13 p13, Parm14 p14, Parm15 p15)
		{
			int index;
			if( locateMultivirtualFunctor1D(index,base) )
				return (callBacks[index])->go(base, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15);
			else	return ResultType();
		}
	
// calling multivirtual function, 2D, 
// symmetry handling in private struct

/// @cond 
	private:
		template< bool useSymmetry, class BaseClassTrait1, class BaseClassTrait2, class ParmTrait3, class ParmTrait4, class ParmTrait5, class ParmTrait6,
				class ParmTrait7, class ParmTrait8, class ParmTrait9, class ParmTrait10, class ParmTrait11, class ParmTrait12, class ParmTrait13,
				class ParmTrait14, class ParmTrait15 >
		struct InvocationTraits
		{
			static ResultType doDispatch( shared_ptr<Executor>& ex, shared_ptr<BaseClassTrait1> base1, shared_ptr<BaseClassTrait2> base2 )
			{	
				return ex->goReverse	(base1, base2);	
			}	
			static ResultType doDispatch( shared_ptr<Executor>& ex, shared_ptr<BaseClassTrait1> base1, shared_ptr<BaseClassTrait2> base2, ParmTrait3 p3)
			{
				return ex->goReverse	(base1, base2, p3);
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex, shared_ptr<BaseClassTrait1> base1, shared_ptr<BaseClassTrait2> base2, ParmTrait3 p3,
						ParmTrait4 p4)
			{
				return ex->goReverse	(base1, base2, p3, p4);
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex, shared_ptr<BaseClassTrait1> base1, shared_ptr<BaseClassTrait2> base2, ParmTrait3 p3,
						ParmTrait4 p4, ParmTrait5 p5)
			{
				return ex->goReverse	(base1, base2, p3, p4, p5);
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex, shared_ptr<BaseClassTrait1> base1, shared_ptr<BaseClassTrait2> base2, ParmTrait3 p3,
						ParmTrait4 p4, ParmTrait5 p5, ParmTrait6 p6)
			{	
				return ex->goReverse	(base1, base2, p3, p4, p5, p6);
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex, shared_ptr<BaseClassTrait1> base1, shared_ptr<BaseClassTrait2> base2, ParmTrait3 p3,
						ParmTrait4 p4, ParmTrait5 p5, ParmTrait6 p6, ParmTrait7 p7)
			{	
				return ex->goReverse	(base1, base2, p3, p4, p5, p6, p7);
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex, shared_ptr<BaseClassTrait1> base1, shared_ptr<BaseClassTrait2> base2, ParmTrait3 p3,
						ParmTrait4 p4, ParmTrait5 p5, ParmTrait6 p6, ParmTrait7 p7, ParmTrait8 p8)
			{	
				return ex->goReverse	(base1, base2, p3, p4, p5, p6, p7, p8);
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex, shared_ptr<BaseClassTrait1> base1, shared_ptr<BaseClassTrait2> base2, ParmTrait3 p3,
						ParmTrait4 p4, ParmTrait5 p5, ParmTrait6 p6, ParmTrait7 p7, ParmTrait8 p8, ParmTrait9 p9)
			{	
				return ex->goReverse	(base1, base2, p3, p4, p5, p6, p7, p8, p9);
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex, shared_ptr<BaseClassTrait1> base1, shared_ptr<BaseClassTrait2> base2, ParmTrait3 p3,
						ParmTrait4 p4, ParmTrait5 p5, ParmTrait6 p6, ParmTrait7 p7, ParmTrait8 p8, ParmTrait9 p9, ParmTrait10 p10)
			{	
				return ex->goReverse	(base1, base2, p3, p4, p5, p6, p7, p8, p9, p10);
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex, shared_ptr<BaseClassTrait1> base1, shared_ptr<BaseClassTrait2> base2, ParmTrait3 p3,
						ParmTrait4 p4, ParmTrait5 p5, ParmTrait6 p6, ParmTrait7 p7, ParmTrait8 p8, ParmTrait9 p9, ParmTrait10 p10, ParmTrait11 p11)
			{	
				return ex->goReverse	(base1, base2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex, shared_ptr<BaseClassTrait1> base1, shared_ptr<BaseClassTrait2> base2, ParmTrait3 p3,
						ParmTrait4 p4, ParmTrait5 p5, ParmTrait6 p6, ParmTrait7 p7, ParmTrait8 p8, ParmTrait9 p9, ParmTrait10 p10, ParmTrait11 p11,
						ParmTrait12 p12)
			{	
				return ex->goReverse	(base1, base2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex, shared_ptr<BaseClassTrait1> base1, shared_ptr<BaseClassTrait2> base2, ParmTrait3 p3,
						ParmTrait4 p4, ParmTrait5 p5, ParmTrait6 p6, ParmTrait7 p7, ParmTrait8 p8, ParmTrait9 p9, ParmTrait10 p10, ParmTrait11 p11,
						ParmTrait12 p12, ParmTrait13 p13)
			{	
				return ex->goReverse	(base1, base2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex, shared_ptr<BaseClassTrait1> base1, shared_ptr<BaseClassTrait2> base2, ParmTrait3 p3,
						ParmTrait4 p4, ParmTrait5 p5, ParmTrait6 p6, ParmTrait7 p7, ParmTrait8 p8, ParmTrait9 p9, ParmTrait10 p10, ParmTrait11 p11,
						ParmTrait12 p12, ParmTrait13 p13, ParmTrait14 p14)
			{	
				return ex->goReverse	(base1, base2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex, shared_ptr<BaseClassTrait1> base1, shared_ptr<BaseClassTrait2> base2, ParmTrait3 p3,
						ParmTrait4 p4, ParmTrait5 p5, ParmTrait6 p6, ParmTrait7 p7, ParmTrait8 p8, ParmTrait9 p9, ParmTrait10 p10, ParmTrait11 p11,
						ParmTrait12 p12, ParmTrait13 p13, ParmTrait14 p14, ParmTrait15 p15)
			{	
				return ex->goReverse	(base1, base2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15);
			}
		};
		template< class BaseClassTrait, class ParmTrait3, class ParmTrait4, class ParmTrait5, class ParmTrait6
					, class ParmTrait7, class ParmTrait8, class ParmTrait9, class ParmTrait10, class ParmTrait11, class ParmTrait12
					, class ParmTrait13, class ParmTrait14, class ParmTrait15 >
		struct InvocationTraits< true , BaseClassTrait, BaseClassTrait, ParmTrait3, ParmTrait4, ParmTrait5, ParmTrait6
					, ParmTrait7, ParmTrait8, ParmTrait9, ParmTrait10, ParmTrait11, ParmTrait12
					, ParmTrait13, ParmTrait14, ParmTrait15 >
		{
			static ResultType doDispatch( shared_ptr<Executor>& ex , shared_ptr<BaseClassTrait> base1, shared_ptr<BaseClassTrait> base2 )
			{
				return ex->go		(base2, base1 );
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex , shared_ptr<BaseClassTrait> base1, shared_ptr<BaseClassTrait> base2, ParmTrait3 p3)
			{
				return ex->go		(base2, base1, p3);
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex , shared_ptr<BaseClassTrait> base1, shared_ptr<BaseClassTrait> base2, ParmTrait3 p3,
						ParmTrait4 p4)
			{
				return ex->go		(base2, base1, p3, p4);
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex , shared_ptr<BaseClassTrait> base1, shared_ptr<BaseClassTrait> base2, ParmTrait3 p3,
						ParmTrait4 p4, ParmTrait5 p5)
			{	
				return ex->go		(base2, base1, p3, p4, p5);
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex , shared_ptr<BaseClassTrait> base1, shared_ptr<BaseClassTrait> base2, ParmTrait3 p3,
						ParmTrait4 p4, ParmTrait5 p5, ParmTrait6 p6)
			{	
				return ex->go		(base2, base1, p3, p4, p5, p6);
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex , shared_ptr<BaseClassTrait> base1, shared_ptr<BaseClassTrait> base2, ParmTrait3 p3,
						ParmTrait4 p4, ParmTrait5 p5, ParmTrait6 p6, ParmTrait7 p7)
			{	
				return ex->go		(base2, base1, p3, p4, p5, p6, p7);
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex , shared_ptr<BaseClassTrait> base1, shared_ptr<BaseClassTrait> base2, ParmTrait3 p3,
						ParmTrait4 p4, ParmTrait5 p5, ParmTrait6 p6, ParmTrait7 p7, ParmTrait8 p8)
			{	
				return ex->go		(base2, base1, p3, p4, p5, p6, p7, p8);
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex , shared_ptr<BaseClassTrait> base1, shared_ptr<BaseClassTrait> base2, ParmTrait3 p3,
						ParmTrait4 p4, ParmTrait5 p5, ParmTrait6 p6, ParmTrait7 p7, ParmTrait8 p8, ParmTrait9 p9)
			{	
				return ex->go		(base2, base1, p3, p4, p5, p6, p7, p8, p9);
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex , shared_ptr<BaseClassTrait> base1, shared_ptr<BaseClassTrait> base2, ParmTrait3 p3,
						ParmTrait4 p4, ParmTrait5 p5, ParmTrait6 p6, ParmTrait7 p7, ParmTrait8 p8, ParmTrait9 p9, ParmTrait10 p10)
			{	
				return ex->go		(base2, base1, p3, p4, p5, p6, p7, p8, p9, p10);
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex , shared_ptr<BaseClassTrait> base1, shared_ptr<BaseClassTrait> base2, ParmTrait3 p3,
						ParmTrait4 p4, ParmTrait5 p5, ParmTrait6 p6, ParmTrait7 p7, ParmTrait8 p8, ParmTrait9 p9, ParmTrait10 p10, ParmTrait11 p11)
			{	
				return ex->go		(base2, base1, p3, p4, p5, p6, p7, p8, p9, p10, p11);
			}	
			static ResultType doDispatch( shared_ptr<Executor>& ex , shared_ptr<BaseClassTrait> base1, shared_ptr<BaseClassTrait> base2, ParmTrait3 p3,
						ParmTrait4 p4, ParmTrait5 p5, ParmTrait6 p6, ParmTrait7 p7, ParmTrait8 p8, ParmTrait9 p9, ParmTrait10 p10, ParmTrait11 p11,
						ParmTrait12 p12)
			{	
				return ex->go		(base2, base1, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex , shared_ptr<BaseClassTrait> base1, shared_ptr<BaseClassTrait> base2, ParmTrait3 p3,
						ParmTrait4 p4, ParmTrait5 p5, ParmTrait6 p6, ParmTrait7 p7, ParmTrait8 p8, ParmTrait9 p9, ParmTrait10 p10, ParmTrait11 p11,
						ParmTrait12 p12, ParmTrait13 p13)
			{	
				return ex->go		(base2, base1, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex , shared_ptr<BaseClassTrait> base1, shared_ptr<BaseClassTrait> base2, ParmTrait3 p3,
						ParmTrait4 p4, ParmTrait5 p5, ParmTrait6 p6, ParmTrait7 p7, ParmTrait8 p8, ParmTrait9 p9, ParmTrait10 p10, ParmTrait11 p11,
						ParmTrait12 p12, ParmTrait13 p13, ParmTrait14 p14)
			{	
				return ex->go		(base2, base1, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
			}
			static ResultType doDispatch( shared_ptr<Executor>& ex , shared_ptr<BaseClassTrait> base1, shared_ptr<BaseClassTrait> base2, ParmTrait3 p3,
						ParmTrait4 p4, ParmTrait5 p5, ParmTrait6 p6, ParmTrait7 p7, ParmTrait8 p8, ParmTrait9 p9, ParmTrait10 p10, ParmTrait11 p11,
						ParmTrait12 p12, ParmTrait13 p13, ParmTrait14 p14, ParmTrait15 p15)
			{	
				return ex->go		(base2, base1, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15);
			}
		};
/// @endcond 

// calling multivirtual function, 2D, public interface
	public:
		ResultType operator() (shared_ptr<BaseClass1>& base1,shared_ptr<BaseClass2>& base2)
		{
			int index1, index2;
			if( locateMultivirtualFunctor2D(index1,index2,base1,base2) )
			{
				if(callBacksInfo[index1][index2])	// reversed
				{
					typedef InvocationTraits<autoSymmetry, BaseClass1, BaseClass2, Parm3, Parm4, Parm5, Parm6,
						Parm7, Parm8, Parm9, Parm10, Parm11, Parm12, Parm13, Parm14, Parm15 > CallTraits;
					return CallTraits::doDispatch( callBacks[index1][index2] , base1, base2 );
				}
				else
					return (callBacks[index1][index2] )->go			(base1, base2 );
			}
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base1,shared_ptr<BaseClass2>& base2, Parm3 p3)
		{
			int index1, index2;
			if( locateMultivirtualFunctor2D(index1,index2,base1,base2) )
			{
				if(callBacksInfo[index1][index2])	// reversed
				{
					typedef InvocationTraits<autoSymmetry, BaseClass1, BaseClass2, Parm3, Parm4, Parm5, Parm6,
						Parm7, Parm8, Parm9, Parm10, Parm11, Parm12, Parm13, Parm14, Parm15 > CallTraits;
					return CallTraits::doDispatch( callBacks[index1][index2] , base1, base2, p3);
				}
				else
					return (callBacks[index1][index2] )->go			(base1, base2, p3 );
			}
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base1,shared_ptr<BaseClass2>& base2, Parm3 p3, Parm4 p4)
		{
			int index1, index2;
			if( locateMultivirtualFunctor2D(index1,index2,base1,base2) )
			{
				if(callBacksInfo[index1][index2])	// reversed
				{
					typedef InvocationTraits<autoSymmetry, BaseClass1, BaseClass2, Parm3, Parm4, Parm5, Parm6,
						Parm7, Parm8, Parm9, Parm10, Parm11, Parm12, Parm13, Parm14, Parm15 > CallTraits;
					return CallTraits::doDispatch( callBacks[index1][index2] , base1, base2, p3, p4 );
				}
				else
					return (callBacks[index1][index2] )->go			(base1, base2, p3, p4 );
			}
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base1,shared_ptr<BaseClass2>& base2, Parm3 p3, Parm4 p4, Parm5 p5)
		{
			int index1, index2;
			if( locateMultivirtualFunctor2D(index1,index2,base1,base2) )
			{
				if(callBacksInfo[index1][index2])	// reversed
				{
					typedef InvocationTraits<autoSymmetry, BaseClass1, BaseClass2, Parm3, Parm4, Parm5, Parm6,
						Parm7, Parm8, Parm9, Parm10, Parm11, Parm12, Parm13, Parm14, Parm15 > CallTraits;
					return CallTraits::doDispatch( callBacks[index1][index2] , base1, base2, p3, p4, p5 );
				}
				else
					return (callBacks[index1][index2] )->go			(base1, base2, p3, p4, p5 );
			}
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base1,shared_ptr<BaseClass2>& base2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6)
		{
			int index1, index2;
			if( locateMultivirtualFunctor2D(index1,index2,base1,base2) )
			{
				if(callBacksInfo[index1][index2])	// reversed
				{
					typedef InvocationTraits<autoSymmetry, BaseClass1, BaseClass2, Parm3, Parm4, Parm5, Parm6,
						Parm7, Parm8, Parm9, Parm10, Parm11, Parm12, Parm13, Parm14, Parm15 > CallTraits;
					return CallTraits::doDispatch( callBacks[index1][index2] , base1, base2, p3, p4, p5, p6 );
				}
				else
					return (callBacks[index1][index2] )->go			(base1, base2, p3, p4, p5, p6 );
			}
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base1,shared_ptr<BaseClass2>& base2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6,
						Parm7 p7)
		{
			int index1, index2;
			if( locateMultivirtualFunctor2D(index1,index2,base1,base2) )
			{
				if(callBacksInfo[index1][index2])	// reversed
				{
					typedef InvocationTraits<autoSymmetry, BaseClass1, BaseClass2, Parm3, Parm4, Parm5, Parm6,
						Parm7, Parm8, Parm9, Parm10, Parm11, Parm12, Parm13, Parm14, Parm15 > CallTraits;
					return CallTraits::doDispatch( callBacks[index1][index2] , base1, base2, p3, p4, p5, p6, p7 );
				}
				else
					return (callBacks[index1][index2] )->go			(base1, base2, p3, p4, p5, p6, p7 );
			}
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base1,shared_ptr<BaseClass2>& base2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6,
						Parm7 p7, Parm8 p8)
		{
			int index1, index2;
			if( locateMultivirtualFunctor2D(index1,index2,base1,base2) )
			{
				if(callBacksInfo[index1][index2])	// reversed
				{
					typedef InvocationTraits<autoSymmetry, BaseClass1, BaseClass2, Parm3, Parm4, Parm5, Parm6,
						Parm7, Parm8, Parm9, Parm10, Parm11, Parm12, Parm13, Parm14, Parm15 > CallTraits;
					return CallTraits::doDispatch( callBacks[index1][index2] , base1, base2, p3, p4, p5, p6, p7, p8);
				}
				else
					return (callBacks[index1][index2] )->go			(base1, base2, p3, p4, p5, p6, p7, p8 );
			}
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base1,shared_ptr<BaseClass2>& base2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6,
						Parm7 p7, Parm8 p8, Parm9 p9)
		{
			int index1, index2;
			if( locateMultivirtualFunctor2D(index1,index2,base1,base2) )
			{
				if(callBacksInfo[index1][index2])	// reversed
				{
					typedef InvocationTraits<autoSymmetry, BaseClass1, BaseClass2, Parm3, Parm4, Parm5, Parm6,
						Parm7, Parm8, Parm9, Parm10, Parm11, Parm12, Parm13, Parm14, Parm15 > CallTraits;
					return CallTraits::doDispatch( callBacks[index1][index2] , base1, base2, p3, p4, p5, p6, p7, p8, p9 );
				}
				else
					return (callBacks[index1][index2] )->go			(base1, base2, p3, p4, p5, p6, p7, p8, p9 );
			}
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base1,shared_ptr<BaseClass2>& base2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6,
						Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10)
		{
			int index1, index2;
			if( locateMultivirtualFunctor2D(index1,index2,base1,base2) )
			{
				if(callBacksInfo[index1][index2])	// reversed
				{
					typedef InvocationTraits<autoSymmetry, BaseClass1, BaseClass2, Parm3, Parm4, Parm5, Parm6,
						Parm7, Parm8, Parm9, Parm10, Parm11, Parm12, Parm13, Parm14, Parm15 > CallTraits;
					return CallTraits::doDispatch( callBacks[index1][index2] , base1, base2, p3, p4, p5, p6, p7, p8, p9, p10);
				}
				else
					return (callBacks[index1][index2] )->go			(base1, base2, p3, p4, p5, p6, p7, p8, p9, p10 );
			}
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base1,shared_ptr<BaseClass2>& base2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6,
						Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11)
		{
			int index1, index2;
			if( locateMultivirtualFunctor2D(index1,index2,base1,base2) )
			{
				if(callBacksInfo[index1][index2])	// reversed
				{
					typedef InvocationTraits<autoSymmetry, BaseClass1, BaseClass2, Parm3, Parm4, Parm5, Parm6,
						Parm7, Parm8, Parm9, Parm10, Parm11, Parm12, Parm13, Parm14, Parm15 > CallTraits;
					return CallTraits::doDispatch( callBacks[index1][index2] , base1, base2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
				}
				else
					return (callBacks[index1][index2] )->go			(base1, base2, p3, p4, p5, p6, p7, p8, p9, p10, p11 );
			}
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base1,shared_ptr<BaseClass2>& base2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6,
						Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11, Parm12 p12)
		{
			int index1, index2;
			if( locateMultivirtualFunctor2D(index1,index2,base1,base2) )
			{
				if(callBacksInfo[index1][index2])	// reversed
				{
					typedef InvocationTraits<autoSymmetry, BaseClass1, BaseClass2, Parm3, Parm4, Parm5, Parm6,
						Parm7, Parm8, Parm9, Parm10, Parm11, Parm12, Parm13, Parm14, Parm15 > CallTraits;
					return CallTraits::doDispatch( callBacks[index1][index2] , base1, base2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12 );
				}
				else
					return (callBacks[index1][index2] )->go			(base1, base2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12 );
			}
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base1,shared_ptr<BaseClass2>& base2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6,
						Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11, Parm12 p12, Parm13 p13)
		{
			int index1, index2;
			if( locateMultivirtualFunctor2D(index1,index2,base1,base2) )
			{
				if(callBacksInfo[index1][index2])	// reversed
				{
					typedef InvocationTraits<autoSymmetry, BaseClass1, BaseClass2, Parm3, Parm4, Parm5, Parm6,
						Parm7, Parm8, Parm9, Parm10, Parm11, Parm12, Parm13, Parm14, Parm15 > CallTraits;
					return CallTraits::doDispatch( callBacks[index1][index2] , base1, base2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
				}
				else
					return (callBacks[index1][index2] )->go			(base1, base2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13 );
			}
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base1,shared_ptr<BaseClass2>& base2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6,
						Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11, Parm12 p12, Parm13 p13, Parm14 p14)
		{
			int index1, index2;
			if( locateMultivirtualFunctor2D(index1,index2,base1,base2) )
			{
				if(callBacksInfo[index1][index2])	// reversed
				{
					typedef InvocationTraits<autoSymmetry, BaseClass1, BaseClass2, Parm3, Parm4, Parm5, Parm6,
						Parm7, Parm8, Parm9, Parm10, Parm11, Parm12, Parm13, Parm14, Parm15 > CallTraits;
					return CallTraits::doDispatch( callBacks[index1][index2] , base1, base2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
				}
				else
					return (callBacks[index1][index2] )->go			(base1, base2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14 );
			}
			else	return ResultType();
		}
		
		ResultType operator() (shared_ptr<BaseClass1>& base1,shared_ptr<BaseClass2>& base2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6,
						Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11, Parm12 p12, Parm13 p13, Parm14 p14, Parm15 p15)
		{
			int index1, index2;
			if( locateMultivirtualFunctor2D(index1,index2,base1,base2) )
			{
				if(callBacksInfo[index1][index2])	// reversed
				{
					typedef InvocationTraits<autoSymmetry, BaseClass1, BaseClass2, Parm3, Parm4, Parm5, Parm6,
						Parm7, Parm8, Parm9, Parm10, Parm11, Parm12, Parm13, Parm14, Parm15 > CallTraits;
					return CallTraits::doDispatch( callBacks[index1][index2] , base1, base2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15);
				}
				else
					return (callBacks[index1][index2] )->go			(base1, base2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15 );
			}
			else	return ResultType();
		}
		
// calling multivirtual function, 3D

// to be continued ...

};


