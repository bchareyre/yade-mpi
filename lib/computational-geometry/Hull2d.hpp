// 2009 © Václav Šmilauer <eudoxos@arcig.cz> 


/*! Computing convex hull of a 2d cloud of points passed to the constructor,
	using Graham scan algorithm.

	Use the operator() to launch computation and get the hull as std::list<Vector2r>.
	
	The source http://marknelson.us/2007/08/22/convex/ is gratefully acknowledged.
	Look there for detailed description and more information.
*/
class ConvexHull2d{
	list<Vector2r> raw_points, lower_partition_points, upper_partition_points, hull;
	Vector2r left, right;
	static Real direction(const Vector2r& p0, const Vector2r& p1, const Vector2r& p2) {
		return ((p0[0]-p1[0])*(p2[1]-p1[1]))-((p2[0]-p1[0])*(p0[1]-p1[1]));
	}
	struct Vector2r_xComparator{
		bool operator()(const Vector2r& p1, const Vector2r& p2){return p1[0]<p2[0];}
	};

	void partition_points(){
		raw_points.sort(Vector2r_xComparator());
		left=raw_points.front(); raw_points.pop_front();
		right=raw_points.back(); raw_points.pop_back();
		FOREACH(const Vector2r& p, raw_points){
			if(direction(left,right,p)<0) upper_partition_points.push_back(p);
			else lower_partition_points.push_back(p);
		}
	}
	vector<Vector2r> build_half_hull(list<Vector2r>& in, int factor){
		vector<Vector2r> out;
		in.push_back(right); out.push_back(left);
		while(in.size()>0){
			out.push_back(in.front()); in.pop_front();
			while(out.size()>=3){
				size_t end=out.size()-1;
				if(factor*direction(out[end-2],out[end],out[end-1])<=0) out.erase(out.begin()+end-1);
				else break;
			}
		}
		return out;
	}
	public:
	ConvexHull2d(const list<Vector2r>& pts){raw_points.assign(pts.begin(),pts.end());};
	ConvexHull2d(const vector<Vector2r>& pts){raw_points.assign(pts.begin(),pts.end());};
	vector<Vector2r> operator()(void){
		partition_points();
		vector<Vector2r> lower_hull=build_half_hull(lower_partition_points,1);
		vector<Vector2r> upper_hull=build_half_hull(upper_partition_points,-1);
		vector<Vector2r> ret; ret.reserve(lower_hull.size()+upper_hull.size()-2);
		for(size_t i=upper_hull.size()-1; i>0; i--) ret.push_back(upper_hull[i]);
		size_t lsize=lower_hull.size();
		for(size_t i=0; i<lsize-1;  i++) ret.push_back(lower_hull[i]);
		return ret;
	}
};

/*! Compute area of a simple 2d polygon, using the Surveyor's formula.
	http://en.wikipedia.org/wiki/Polygon

	The first and last points shouldn't be given twice.
*/
Real simplePolygonArea2d(vector<Vector2r> P){
	Real ret=0.; size_t n=P.size();
	for(size_t i=0; i<n-1; i++) { ret+=P[i][0]*P[i+1][1]-P[i+1][0]*P[i][1];}
	ret+=P[n-1][0]*P[0][1]-P[0][0]*P[n-1][1];
	return abs(ret/2.);
}


