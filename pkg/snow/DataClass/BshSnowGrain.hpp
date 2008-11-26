#pragma once

#include<yade/core/GeometricalModel.hpp>
#include<Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>
#include<vector>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>

typedef std::vector< std::vector<std::vector<unsigned char> > > T_DATA;

class Grrrr : public Serializable
{ // a workaround to stupid bug in yade::serialization
	public:
		std::vector<Vector3r> grr;
		Grrrr(){};
		Grrrr(std::vector<Vector3r>& g):grr(g){};
	protected :
		void registerAttributes()
		{
			REGISTER_ATTRIBUTE(grr);
		}
	REGISTER_CLASS_NAME(Grrrr);
	REGISTER_BASE_CLASS_NAME(Serializable);
};

REGISTER_SERIALIZABLE(Grrrr);

class BshSnowGrain : public GeometricalModel
{
	public:
		Vector3r center,c_axis;
		Vector3r start,end;
		Vector3r color;
		int selection;
		std::vector<std::vector<Vector3r> > slices;

		std::vector<Grrrr> gr_gr;
	public: 
		BshSnowGrain():GeometricalModel(){createIndex();};
		BshSnowGrain(const T_DATA& dat,Vector3r c_axis,int SELECTION,Vector3r col);
		Vector3r search(const T_DATA& dat,Vector3r c,Vector3r dir);
		Vector3r search_plane(const T_DATA& dat,Vector3r c,Vector3r dir);
	
	private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			//boost::mutex::scoped_lock scoped_lock(m_voxel_envelope_mutex);
			ar & center & c_axis;
			ar & start & end;
			ar & color;
			ar & selection;
			ar & slices;
		}
	
	protected :
		void registerAttributes();
		void preProcessAttributes(bool loading)
		{
			if(!loading)
			{
				gr_gr.clear();
				BOOST_FOREACH(std::vector<Vector3r>& g,slices)
					gr_gr.push_back(Grrrr(g));
			}
		};
		void postProcessAttributes(bool loading)
		{
			if(loading)
			{
				slices.clear();
				BOOST_FOREACH(Grrrr& g,gr_gr)
					slices.push_back(g.grr);
			}
		};
	REGISTER_CLASS_NAME(BshSnowGrain);
	REGISTER_BASE_CLASS_NAME(GeometricalModel);
	REGISTER_CLASS_INDEX(BshSnowGrain,GeometricalModel);
};

REGISTER_SERIALIZABLE(BshSnowGrain);

