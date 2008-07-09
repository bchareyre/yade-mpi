#include"SQLiteRecorder.hpp"
#include<boost/algorithm/string.hpp>
#include<yade/core/MetaBody.hpp>
using namespace boost;
CREATE_LOGGER(SQLiteRecorder);

/*

Database structure:
	meta: saved simulation, perhaps some metadata
	records:	one row per saved state
	bodies_iter_#: per-body rows
	interactions_iter_#: per-interaction rows [NOT IMPLEMENTED]

TODO: set types on columns (doubles, strings etc)

*/

void SQLiteRecorder::init(MetaBody* rootBody){
	con=shared_ptr<sqlite3x::sqlite3_connection>(new sqlite3x::sqlite3_connection(dbFile));
	LOG_DEBUG("Opened db "<<dbFile);
	con->executenonquery("PRAGMA synchronous = OFF");
	// create supertable (only if the db is empty)
	if(0==con->executeint("select count(*) from sqlite_master where name='meta';")){
		ostringstream out;
		LOG_DEBUG("Saving simulation to stream");
		Omega::instance().saveSimulationToStream(out);
		con->executenonquery("create table meta (simulationXML)");
		LOG_DEBUG("Inserting simulation XML into the table 'meta'");
		{ sqlite3x::sqlite3_command cmd(*con,"insert into meta values (?)"); cmd.bind(1,out.str()); cmd.executenonquery(); }
		assert(con->executeint("select count(*) from sqlite_master where name='records';")==0);
		con->executenonquery("create table records (iter INTEGER, realTime FLOAT, virtTime FLOAT, wallClock FLOAT, bodyTable STRING, interactionTable STRING);");
	}
}

void SQLiteRecorder::action(MetaBody* rootBody){
	if(!con) init(rootBody);
	vector<bool> recActive(REC_SENTINEL,false);
	FOREACH(string& rec, recorders){
		if(rec=="se3") recActive[REC_SE3]=true;
		else if(rec=="rgb") recActive[REC_RGB]=true;
		else LOG_ERROR("Unknown recorder named `"<<rec<<"' (supported are: se3, rgb). Ignored.");
	}

	// create table
	string perBodyTable="bodies_iter_"+lexical_cast<string>(Omega::instance().getCurrentIteration());
	assert(0==con->executeint("select count(*) from sqlite_master where name='"+perBodyTable+"';")); // table shouldn't exist yet
	vector<string> columns; columns.push_back("id INTEGER");
	if(recActive[REC_SE3]){columns.push_back("se3_x FLOAT"); columns.push_back("se3_y FLOAT"); columns.push_back("se3_z FLOAT"); columns.push_back("se3_ori0 FLOAT"); columns.push_back("se3_ori1 FLOAT"); columns.push_back("se3_ori2 FLOAT"); columns.push_back("se3_ori3 FLOAT");}
	if(recActive[REC_RGB]){	columns.push_back("rgb_r FLOAT"); columns.push_back("rgb_g FLOAT"); columns.push_back("rgb_b FLOAT");}

	// create record table with the right number of columns
	con->executenonquery("create table "+perBodyTable+" ("+algorithm::join(columns,",")+");");
	LOG_DEBUG("Created table "<<perBodyTable<<" with columns "<<algorithm::join(columns,","));

	// add table to the records table
	{	sqlite3x::sqlite3_command cmd(*con,"insert into records values (?,?,?,?,?,?);"); cmd.bind(1,(sqlite3x::int64_t)Omega::instance().getCurrentIteration()); cmd.bind(2,Omega::instance().getComputationTime()); cmd.bind(3,Omega::instance().getSimulationTime()); cmd.bind(4,PeriodicEngine::getClock()); cmd.bind(5,perBodyTable); cmd.bind(6,""); cmd.executenonquery(); }

	// loop over bodies
	sqlite3x::sqlite3_transaction transaction(*con);
	{
		sqlite3x::sqlite3_command cmd(*con,"insert into "+perBodyTable+" values ("+algorithm::join(vector<string>(columns.size(),"?"),",")+");");
		FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
			// placeholders are 1-based (!!!)
			size_t field=1; cmd.bind(field++,b->getId());
			if(recActive[REC_SE3]){
				const Se3r& se3=b->physicalParameters->se3;
				cmd.bind(field++,se3.position[0]); cmd.bind(field++,se3.position[1]); cmd.bind(field++,se3.position[2]);
				cmd.bind(field++,se3.orientation[0]); cmd.bind(field++,se3.orientation[1]); cmd.bind(field++,se3.orientation[2]); cmd.bind(field++,se3.orientation[3]);
			}
			if(recActive[REC_RGB]){
				const Vector3r& color=b->geometricalModel->diffuseColor;
				cmd.bind(field++,color[0]); cmd.bind(field++,color[1]); cmd.bind(field++,color[2]);
			}
			assert(field-1==columns.size());
			cmd.executenonquery();
		}
	}
	transaction.commit();
}

