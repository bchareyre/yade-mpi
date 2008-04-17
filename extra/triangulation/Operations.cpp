//#pragma once

#include "Operations.h"


Plan PlanRadical	(const Sphere& S1, const Sphere& S2)
{
	Segment seg (S1.point(), S2.point());
	Real r = 0.5*(1.0 + (S1.weight() - S2.weight())/seg.squared_length());
    return Plan(S1.point() + seg.to_vector()*r, seg.to_vector());
}


Droite Intersect (Plan &P1, Plan &P2)
{
	CGAL::Object result;
    Droite D;
    result = CGAL::intersection(P1, P2);
    if (CGAL::assign(D, result)) return D;
	else {
		//std::cout << "2 plans sans droite d'intersection!!!!!!!!!";
		exit(0); }
}

Point Intersect (Plan &P1, Plan &P2, Plan &P3)
{
	//std::cout << "Point Intersect (Plan &P1, Plan &P2, Plan &P3)" << std::endl; 
	CGAL::Object result;
	Point P;
	//std::cout << "Point P;" << std::endl; 
	result = CGAL::intersection(P1, Intersect(P2, P3));
	//std::cout << "result = CGAL::intersectio" << std::endl;
    if (CGAL::assign(P, result)) return P;
	else {
		std::cout << "3 plans sans point d'intersection!!!!!!!!!";
		exit(0);}
}


