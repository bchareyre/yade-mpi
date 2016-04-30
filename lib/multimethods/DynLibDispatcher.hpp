/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <lib/multimethods/Indexable.hpp>
#include <lib/factory/ClassFactory.hpp>
#include <lib/serialization/Serializable.hpp>

#include <loki/Functor.h>
#include <loki/Typelist.h>
#include <loki/NullType.h>
// compat with former yade's local Loki
#define TYPELIST_1 LOKI_TYPELIST_1
#define TYPELIST_2 LOKI_TYPELIST_2
#define TYPELIST_3 LOKI_TYPELIST_3
#define TYPELIST_4 LOKI_TYPELIST_4
#define TYPELIST_5 LOKI_TYPELIST_5
#define TYPELIST_7 LOKI_TYPELIST_7

#include <vector>
#include <list>
#include <string>
#include <ostream>

struct DynLibDispatcher_Item2D {
	int ix1, ix2;
	std::string functorName;
	DynLibDispatcher_Item2D(int a, int b, std::string c):ix1(a),ix2(b),functorName(c){};
};
struct DynLibDispatcher_Item1D {
	int ix1;
	std::string functorName;
	DynLibDispatcher_Item1D(int a, std::string c):ix1(a), functorName(c){};
};
///
/// base classes involved in multiple dispatch must be derived from Indexable
///

/// base template for all dispatchers								///

template 
<
	class BaseClass,	//	a typelist with base classess involved in the dispatch (or single class, for 1D )
				// 		FIXME: should use shared_ptr references, like this: DynLibDispatcher< TYPELIST_2( shared_ptr<PhysicalAction>& , shared_ptr<Body>& ) , ....
	class Executor,		//	class which gives multivirtual function
	class ResultType,	//	type returned by multivirtual function
	class TList,
				//	typelist of arguments passed to multivirtual function
				//	WARNING: first arguments must be shared_ptr<BaseClass>, for details see FunctorWrapper
				
	bool autoSymmetry=true
				/*true - the function called is always the same,
						only order of arguments is rearranged
						to make correct function call, 
						only go() is called
				
					false - the function called is always different.
						arguments order is not rearranged
						go(), and goReverse() are called, respectively*/
>
class DynLibDispatcher
{
	// this template recursively defines a type for callBacks matrix, with required number of dimensions
	private:
		template<class T > struct Matrix {
				using ResultIterator = Loki::NullType;
				using ResultIteratorInt = Loki::NullType;
			};

	template<class Head > struct Matrix< Loki::Typelist< Head, Loki::NullType > > {
				using Result = vector< shared_ptr< Executor > >;
				using ResultInt = vector< int >;
				using ResultIterator = typename vector< shared_ptr< Executor > >::iterator;
				using ResultIteratorInt = vector< int >::iterator;
		  };

	template<class Head, class Tail >
		  struct Matrix< Loki::Typelist< Head, Tail > > {
				// recursive typedef to get matrix of required dimensions
				using InsideType = typename Matrix< Tail >::Result;
				using InsideTypeInt = typename Matrix< Tail >::ResultInt;
				using Result = vector< InsideType >;
				using ResultInt = vector< InsideTypeInt >;
				using ResultIterator = typename vector< InsideType >::iterator;
				using ResultIteratorInt = typename vector< InsideTypeInt >::iterator;
		  };

	typedef typename Loki::TL::Append<  Loki::NullType , BaseClass >::Result BaseClassList;
	typedef typename Loki::TL::TypeAtNonStrict<BaseClassList , 0>::Result    BaseClass1;  // 1D
	typedef typename Loki::TL::TypeAtNonStrict<BaseClassList , 1>::Result    BaseClass2;  // 2D
	
	
	typedef typename Matrix< BaseClassList >::ResultIterator     Iterator2; // outer iterator 2D
	typedef typename Matrix< BaseClassList >::ResultIteratorInt  IteratorInfo2;
	
	typedef typename Matrix< BaseClassList >::Result MatrixType;
	typedef typename Matrix< BaseClassList >::ResultInt MatrixIntType;
	MatrixType callBacks;        // multidimensional matrix that stores functors ( 1D, 2D, 3D, 4D, ....)
	MatrixIntType callBacksInfo; // multidimensional matrix for extra information about functors in the matrix
	                             // currently used to remember if it is reversed functor

	public:
		DynLibDispatcher()
		  {
			// FIXME - static_assert( typeid(BaseClass1) == typeid(Parm1) ); // 1D
			// FIXME - static_assert( typeid(BaseClass2) == typeid(Parm2) ); // 2D
			clearMatrix();
		};
		  
		void clearMatrix() {
			callBacks.clear();
			callBacksInfo.clear();
		}

		shared_ptr<Executor> getExecutor(shared_ptr<BaseClass1>& arg1){
			int ix1;
			if(arg1->getClassIndex()<0) throw runtime_error("No functor for type "+arg1->getClassName()+" (index "+boost::lexical_cast<string>(arg1->getClassIndex())+"), since the index is invalid (negative).");
			if(locateMultivirtualFunctor1D(ix1,arg1)) return callBacks[ix1];
			return shared_ptr<Executor>();
		}

		shared_ptr<Executor> getExecutor(shared_ptr<BaseClass1>& arg1, shared_ptr<BaseClass2>& arg2){
			if(arg1->getClassIndex()<0 || arg2->getClassIndex()<0) throw runtime_error("No functor for types "+arg1->getClassName()+" (index "+boost::lexical_cast<string>(arg1->getClassIndex())+") + "+arg2->getClassName()+" (index "+boost::lexical_cast<string>(arg2->getClassIndex())+"), since some of the indices is invalid (negative).");
			int ix1,ix2;
			if(locateMultivirtualFunctor2D(ix1,ix2,arg1,arg2)) return callBacks[ix1][ix2];
			return shared_ptr<Executor>();
		 }

		shared_ptr<Executor> getFunctor1D(shared_ptr<BaseClass1>& base1){ return getExecutor(base1); }
		/* Return pointer to the functor for two base classes given. Swap is true if the dispatch objects should be swapped before calling Executor::go. */
		shared_ptr<Executor> getFunctor2D(shared_ptr<BaseClass1>& base1, shared_ptr<BaseClass2>& base2, bool& swap){
			int ix1, ix2;
			if(!locateMultivirtualFunctor2D(ix1,ix2,base1,base2)) return shared_ptr<Executor>();
			swap=(bool)(callBacksInfo[ix1][ix2]);
			return callBacks[ix1][ix2];
		}

		/*! Return representation of the dispatch matrix as vector of int,string (i.e. index,functor name) */
		vector<DynLibDispatcher_Item1D> dataDispatchMatrix1D(){
			vector<DynLibDispatcher_Item1D> ret;
			for(size_t i=0; i<callBacks.size(); i++) {
				if(callBacks[i]) ret.push_back(DynLibDispatcher_Item1D(i,callBacks[i]->getClassName()));
			}
			return ret;
		}
		/*! Return representation of the dispatch matrix as vector of int,int,string (i.e. index1,index2,functor name) */
		vector<DynLibDispatcher_Item2D> dataDispatchMatrix2D(){
			vector<DynLibDispatcher_Item2D> ret;
			for(size_t i=0; i<callBacks.size(); i++) {
				for(size_t j=0; j<callBacks[i].size(); j++) {
					if(callBacks[i][j]) {
						ret.push_back(DynLibDispatcher_Item2D(i,j,callBacks[i][j]->getClassName()));
					}
				}
			}
			return ret;
		}

		/*! Dump 1d dispatch matrix to given stream. */
		std::ostream& dumpDispatchMatrix1D(std::ostream& out, const string& prefix=""){
			for(size_t i=0; i<callBacks.size(); i++){
				if(callBacks[i]) out<<prefix<<i<<" -> "<<callBacks[i]->getClassName()<<std::endl;
			}
			return out;
		}
		/*! Dump 2d dispatch matrix to given stream. */
		std::ostream& dumpDispatchMatrix2D(std::ostream& out, const string& prefix=""){
			for(size_t i=0; i<callBacks.size(); i++) {
				for(size_t j=0; j<callBacks.size(); j++) {
					if(callBacks[i][j]) out<<prefix<<i<<"+"<<j<<" -> "<<callBacks[i][j]->getClassName()<<std::endl;
				}
			}
			return out;
		}

 	public:
		void add1DEntry(string baseClassName, shared_ptr<Executor> executor){
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

			callBacks[index] = executor;
		};

		
	public:
		void add2DEntry(string baseClassName1, string baseClassName2, shared_ptr<Executor> executor){
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
					throw std::runtime_error(("Multimethods: checkOrder: undefined dispatch order for "+executor->getClassName()).c_str());
				}
			}
			else // classes are different, no symmetry possible
			{
				callBacks	[index1][index2] = executor;
				callBacksInfo	[index1][index2] = 0;
			}
		}
		

		bool locateMultivirtualFunctor1D(int& index, shared_ptr<BaseClass1>& base) {
			if(callBacks.empty()) return false;
			index = base->getClassIndex();
			assert( index >= 0 && (unsigned int)( index ) < callBacks.size());
			if(callBacks[index]) return true;
			
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

		bool locateMultivirtualFunctor2D(int& index1, int& index2, shared_ptr<BaseClass1>& base1, shared_ptr<BaseClass2>& base2) {
			//#define _DISP_TRACE(msg) cerr<<"@DT@"<<__LINE__<<" "<<msg<<endl;
			#define _DISP_TRACE(msg)
			if(callBacks.empty()) return false;
			index1=base1->getClassIndex(); index2 = base2->getClassIndex();
			assert(index1>=0); assert(index2>=0); 
			assert((unsigned int)(index1)<callBacks.size()); assert((unsigned int)(index2)<callBacks[index1].size());
			_DISP_TRACE("arg1: "<<base1->getClassName()<<"="<<index1<<"; arg2: "<<base2->getClassName()<<"="<<index2)
			/* This is python code for the algorithm:

				def ff(x,sum): print x,sum-x,sum
				for dist in range(0,5):
					for ix1 in range(0,dist+1): ff(ix1,dist)

				Increase depth sum from 0 up and look for possible matches, of which sum of distances beween the argument and the declared functor arg type equals depth.
				
				Two matches are considered euqally good (ambiguous) if they have the same depth. That raises exception.

				If both indices are negative (reached the top of hierarchy for that indexable type) and nothing has been found for given depth, raise exception (undefined dispatch).

				FIXME: by the original design, callBacks don't distinguish between dispatch that was already looked for,
				but is undefined and dispatch that was never looked for before. This means that there can be lot of useless lookups;
				e.g. if MetaInteractingGeometry2AABB is not in BoundingVoumeMetaEngine, it is looked up at every step.

			*/
			if(callBacks[index1][index2]) {
				_DISP_TRACE("Direct hit at ["<<index1<<"]["<<index2<<"] → "<<callBacks[index1][index2]->getClassName());
				return true;
			}
			int foundIx1,foundIx2; int maxDp1=-1, maxDp2=-1;
			
			for(int dist=1; ; dist++){
				bool distTooBig=true;
				foundIx1=foundIx2=-1; // found no dispatch at this depth yet
				for(int dp1=0; dp1<=dist; dp1++){
					int dp2=dist-dp1;
					if((maxDp1>=0 && dp1>maxDp1) || (maxDp2>=0 && dp2>maxDp2)) continue;
					_DISP_TRACE(" Trying indices with depths "<<dp1<<" and "<<dp2<<", dist="<<dist);
					int ix1=dp1>0?base1->getBaseClassIndex(dp1):index1, ix2=dp2>0?base2->getBaseClassIndex(dp2):index2;
					if(ix1<0) maxDp1=dp1;
					if(ix2<0) maxDp2=dp2;
					if(ix1<0 || ix2<0) continue; // hierarchy height exceeded in either dimension
					distTooBig=false;
					if(callBacks[ix1][ix2]){
						if(foundIx1!=-1 && callBacks[foundIx1][foundIx2]!=callBacks[ix1][ix2]){ // we found a callback, but there already was one at this distance and it was different from the current one
							cerr<<__FILE__<<":"<<__LINE__<<": ambiguous 2d dispatch ("<<"arg1="<<base1->getClassName()<<", arg2="<<base2->getClassName()<<", distance="<<dist<<"), dispatch matrix:"<<endl;
							dumpDispatchMatrix2D(cerr,"AMBIGUOUS: "); throw runtime_error("Ambiguous dispatch.");
						}
						foundIx1=ix1; foundIx2=ix2;
						callBacks[index1][index2]=callBacks[ix1][ix2];
						callBacksInfo[index1][index2]=callBacksInfo[ix1][ix2];
						_DISP_TRACE("Found callback ["<<ix1<<"]["<<ix2<<"] → "<<callBacks[ix1][ix2]->getClassName());
					}
				}
				if(foundIx1!=-1) return true;
				if(distTooBig){
					_DISP_TRACE("Undefined dispatch, dist="<<dist);
					return false; /* undefined dispatch */
				}
			}
		};
		
// calling multivirtual function, 1D
		template<typename... Args>
		ResultType operator() (shared_ptr<BaseClass1>& base, Args... args)
		{
			int index;
			if( locateMultivirtualFunctor1D(index,base) )
				return (callBacks[index])->go(base, args...);
			else	return ResultType();
		}
		

// calling multivirtual function, 2D, 
// symmetry handling in private struct
	private:
		template< bool useSymmetry, class BaseClassTrait1, class BaseClassTrait2, typename... Args>
		struct InvocationTraits
		{
			static ResultType doDispatch(shared_ptr<Executor>& ex, shared_ptr<BaseClassTrait1> base1, shared_ptr<BaseClassTrait2> base2, Args... args)
			{
				return ex->goReverse (base1, base2, args...);
			}
		};
		template< class BaseClassTrait, typename... Args>
		struct InvocationTraits< true , BaseClassTrait, BaseClassTrait, Args... >
		{
			static ResultType doDispatch( shared_ptr<Executor>& ex , shared_ptr<BaseClassTrait> base1, shared_ptr<BaseClassTrait> base2, Args... args)
			{
				return ex->go (base2, base1, args...);
			}
		};

// calling multivirtual function, 2D, public interface
	public:
		template<typename... Args>
		ResultType operator() (shared_ptr<BaseClass1>& base1, shared_ptr<BaseClass2>& base2, Args... args)
		{
			int index1, index2;
			if( locateMultivirtualFunctor2D(index1,index2,base1,base2) )
			{
				if(callBacksInfo[index1][index2])// reversed
				{
					using CallTraits=InvocationTraits<autoSymmetry, BaseClass1, BaseClass2, Args...> ;
					return CallTraits::doDispatch( callBacks[index1][index2] , base1, base2, args...);
				}
				else
					return (callBacks[index1][index2] )->go(base1, base2, args...);
			}
			else	return ResultType();
		}
};
