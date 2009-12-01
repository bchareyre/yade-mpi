// © 2007 Vaclav Smilauer <eudoxos@arcig.cz>

#pragma once

#include<yade/core/World.hpp>
#include<yade/core/FileGenerator.hpp>
#include<yade/core/DeusExMachina.hpp>
#include<yade/pkg-dem/Clump.hpp>


/*! \brief Test some basic clump functionality; show how to use clumps as well. */
class ClumpTestGen : public FileGenerator {
		void createOneClump(shared_ptr<World>& rootBody, Vector3r clumpPos, vector<Vector3r> relPos, vector<Real> radii);
		shared_ptr<ClumpMemberMover> clumpMover;
	public :
		bool generate();
	DECLARE_LOGGER;
	REGISTER_CLASS_AND_BASE(ClumpTestGen,FileGenerator);
	REGISTER_ATTRIBUTES(FileGenerator,/*nothing here*/);
};
REGISTER_SERIALIZABLE(ClumpTestGen);


