#pragma once

#include<yade/core/GeometricalModel.hpp>
#include<Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>
#include<vector>
#include<boost/serialization/vector.hpp>
#include<boost/serialization/shared_ptr.hpp>
#include<boost/tuple/tuple.hpp>

typedef std::vector< std::vector<std::vector<unsigned char> > > T_DATA;

// delete this class after we migrate to boost::serialization
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
		Real layer_distance;

		int m_how_many_faces;
		std::vector<boost::tuple<Vector3r,Vector3r,Vector3r,Vector3r> > m_faces; // A,B,C,normal
		std::vector<float> m_depths; // depth for each face (allows faster checking of collision).
		// depths are negative numbers! positive number would be an altitude and means that point is _above_ the face

		std::vector<Grrrr> gr_gr;
	public: 
		BshSnowGrain():GeometricalModel(){createIndex(); m_how_many_faces=-1;};
		BshSnowGrain(const T_DATA& dat,Vector3r c_axis,int SELECTION,Vector3r col,Real one_voxel_in_meters_is);
		Vector3r search(const T_DATA& dat,Vector3r c,Vector3r dir);
		Vector3r search_plane(const T_DATA& dat,Vector3r c,Vector3r dir);

		bool is_point_inside_polyhedron(Vector3r point);
		int how_many_faces();
		bool face_is_valid(Vector3r&,Vector3r&,Vector3r&);
		Real depth(int i){return m_depths[i];};
		void push_face(Vector3r,Vector3r,Vector3r);
		const std::vector<boost::tuple<Vector3r,Vector3r,Vector3r,Vector3r> >& get_faces_const_ref(){how_many_faces(); return m_faces;};
	
	private:
		Real calc_depth(size_t);
		bool is_point_orthogonally_projected_on_triangle(Vector3r& a,Vector3r& b,Vector3r c,Vector3r& N,Vector3r& P,Real point_plane_distance = 0.0);

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, unsigned int version)
		{
			//boost::mutex::scoped_lock scoped_lock(m_voxel_envelope_mutex);
			ar & layer_distance;
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
				m_how_many_faces = -1;
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

