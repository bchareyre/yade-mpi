#include"BshSnowGrain.hpp"
#include<Wm3Quaternion.h>

// a pixel is 20.4 microns (2.04 × 10-5 meters)
// the sample was 10.4mm hight


void BshSnowGrain::registerAttributes()
{
	GeometricalModel::registerAttributes();
	REGISTER_ATTRIBUTE(center);
	REGISTER_ATTRIBUTE(c_axis);
	REGISTER_ATTRIBUTE(start);
	REGISTER_ATTRIBUTE(end);
	REGISTER_ATTRIBUTE(color);
	REGISTER_ATTRIBUTE(selection);
	REGISTER_ATTRIBUTE(layer_distance);
	REGISTER_ATTRIBUTE(gr_gr);
}

BshSnowGrain::BshSnowGrain(const T_DATA& dat,Vector3r c_ax,int SELECTION,Vector3r col, Real one_voxel_in_meters_is) : GeometricalModel()
{
	createIndex();

	if(SELECTION!=0)
	{
		color=col;

		c_axis=c_ax;
		c_axis.Normalize();
		selection=SELECTION;
		std::cout << "creating grain " << SELECTION << " c_axis " << c_axis << "\n";

		Vector3r sum(0,0,0);
		float count=0;

		int FRAMES=dat.size();
		int sx=dat[0].size();
		int sy=dat[0][0].size();
		for(int i=0;i<FRAMES;++i)
		{
			for(int x=0;x<sx;++x)
				for(int y=0;y<sy;++y)
					if( dat[i][x][y]==SELECTION )
					{
						sum+=Vector3r((float)x,(float)y,(float)i);
						count+=1.0;
					}
		}
		center=sum/count;

		std::cout << "finding start/end\n";

		start=search_plane(dat,center,c_axis);
		end  =search_plane(dat,center,-1.0*c_axis);
		
		std::cout << "(start-end).length() " << ((start-end).Length()) << "\n";

		Quaternionr q;
		q.Align(Vector3r(0,0,1),c_axis);
//////////////////////////////////////////////////////////
int II=0;
float prevL=(start-end).SquaredLength();
float tmpL;
Vector3r moving_center(0,0,0),moving_sum(0,0,0);
count=0;
layer_distance=3.0;
for(Vector3r S=start; (tmpL=(S-end).SquaredLength())<=prevL ; S-=c_axis*layer_distance, ++II)
{
	slices.resize(II+1);
	float vectorY1=5.0;
	float vectorX1=0.0;
	for(float angle=0.0f ; angle <= (2.0f*3.14159) ; angle+=0.2f)
	{		
		float vectorX=(5.0*sin(angle));
		float vectorY=(5.0*cos(angle));		
		Vector3r tmp(vectorX1,vectorY1,0);
		tmp=search(dat,S+moving_center,q*tmp);
		slices[II].push_back(tmp-center);
		vectorY1=vectorY;
		vectorX1=vectorX;

		moving_sum+=tmp;
		count+=1.0;
	}
	prevL=tmpL;

	moving_center=moving_sum/count-S;
	moving_sum=Vector3r(0,0,0);
	count=0;

	std::cout << "slices " << II << " S-end " << (S-end).SquaredLength() << "\n";
}
//////////////////////////////////////////////////////////
// we're done. Now scale everything.
BOOST_FOREACH(std::vector<Vector3r>& g,slices)
	BOOST_FOREACH(Vector3r& v,g)
		v *= one_voxel_in_meters_is;

c_axis.Normalize();
start = (start-center)*one_voxel_in_meters_is;
end = (end-center)*one_voxel_in_meters_is;
center *= one_voxel_in_meters_is;


		
		std::cout << "grain created, center " << center << " start " << start << " end " << end << "\n";
	} 
	else
		std::cout << "nothing selected!\n";
};
	
Vector3r BshSnowGrain::search(const T_DATA& dat,Vector3r c,Vector3r dir)
{
	dir.Normalize();
	dir=dir*0.3;

	Vector3r the_farthest(c);

	int FRAMES=dat.size();
	int sx=dat[0].size();
	int sy=dat[0][0].size();
	for( Vector3r current(c); 
		   ((int)(current[0])<sx    ) && ((int)(current[0])>=0) 
		&& ((int)(current[1])<sy    ) && ((int)(current[1])>=0) 
		&& ((int)(current[2])<FRAMES) && ((int)(current[2])>=0) 
		; current+=dir )

			if( dat[(int)(current[2])][(int)(current[0])][(int)(current[1])]==selection )
				the_farthest=current;

	return the_farthest;
};

Vector3r BshSnowGrain::search_plane(const T_DATA& dat,Vector3r c,Vector3r dir)
{
	Vector3r result=search(dat,c,dir);
	
	dir.Normalize();
	
	Quaternionr q;
	q.Align(Vector3r(0,0,1),dir);

	dir=dir*0.3;
	bool anything_found=true;
		
	for( ; anything_found==true ; result+=dir)
	{
		anything_found=false;

		float vectorY1=5.0;
		float vectorX1=0.0;
		for(float angle=0.0f ; (angle <= (2.0f*3.14159)) && (anything_found==false) ; angle+=0.2f)
		{		
			float vectorX=(5.0*sin(angle));
			float vectorY=(5.0*cos(angle));		
			Vector3r tmp(vectorX1,vectorY1,0);
			tmp=search(dat,result,q*tmp);

			if((tmp-result).SquaredLength()>1.0)
				anything_found=true;
			else
				anything_found=false;

			vectorY1=vectorY;
			vectorX1=vectorX;			
		}

	}

	return result;
};

YADE_PLUGIN("BshSnowGrain","Grrrr");

