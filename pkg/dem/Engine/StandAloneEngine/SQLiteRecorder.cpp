// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#include"SQLiteRecorder.hpp"
#include<boost/algorithm/string.hpp>
#include<boost/filesystem/operations.hpp>
#include<boost/filesystem/convenience.hpp>
#include<boost/algorithm/string/join.hpp>
#include<yade/core/MetaBody.hpp>

YADE_REQUIRE_FEATURE(deprecated);

YADE_PLUGIN((SQLiteRecorder));
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
		shared_ptr<InteractionContainer> intrs;
		if(!saveInteractions){
			intrs=rootBody->interactions; rootBody->interactions=shared_ptr<InteractionContainer>();
		}
		LOG_DEBUG("Saving simulation to stream"); ostringstream out; Omega::instance().saveSimulationToStream(out);
		if(!saveInteractions){ rootBody->interactions=intrs; }
		con->executenonquery("create table meta (simulationXML STRING,maxIter INTEGER)");
		LOG_DEBUG("Inserting simulation XML into the table 'meta'");
		{ sqlite3x::sqlite3_command cmd(*con,"insert into meta values (?,-1)"); cmd.bind(1,out.str()); cmd.executenonquery(); }
		assert(con->executeint("select count(*) from sqlite_master where name='records';")==0);
		con->executenonquery("create table records (iter INTEGER, realTime FLOAT, virtTime FLOAT, wallClock FLOAT, bodyTable STRING, interactionTable STRING);");
	} else { /* db exists already! */
		/* if the last iteration saved is greater or equal to ours, we're not running the same simulation.
		 * The database will be renamed and init() called again.
		 */
		long maxIter=con->executeint("SELECT maxIter from meta;");
		LOG_DEBUG("maxIter in existing database is "<<maxIter);
		if(maxIter>=Omega::instance().getCurrentIteration()){
			LOG_DEBUG("This db is not continuation of current simulation (maxIter>currentIter), will be moved away.");
			assert(filesystem::exists(dbFile));
			int i=0;
			while(filesystem::exists(dbFile+"~"+lexical_cast<string>(i))) i++;
			string newDbFile=dbFile+"~"+lexical_cast<string>(i);
			filesystem::rename(dbFile,newDbFile);
			LOG_INFO("Renamed old database "<<dbFile<<" to "<<newDbFile<<" (rerun initialization).");
			init(rootBody);
		}
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
	char iterPadded[16]; snprintf(iterPadded,16,"%07ld",Omega::instance().getCurrentIteration());
	string perBodyTable="bodies_iter_"+string(iterPadded);
	// FIXME: handle this gracefully
	assert(0==con->executeint("select count(*) from sqlite_master where name='"+perBodyTable+"';")); // table shouldn't exist yet
	vector<string> columns; columns.push_back("id INTEGER");
	if(recActive[REC_SE3]){columns.push_back("se3_x FLOAT"); columns.push_back("se3_y FLOAT"); columns.push_back("se3_z FLOAT"); columns.push_back("se3_ori0 FLOAT"); columns.push_back("se3_ori1 FLOAT"); columns.push_back("se3_ori2 FLOAT"); columns.push_back("se3_ori3 FLOAT");}
	if(recActive[REC_RGB]){	columns.push_back("rgb_r FLOAT"); columns.push_back("rgb_g FLOAT"); columns.push_back("rgb_b FLOAT");}

	// create record table with the right number of columns
	con->executenonquery("create table "+perBodyTable+" ("+algorithm::join(columns,",")+");");
	LOG_DEBUG("Created table "<<perBodyTable<<" with columns "<<algorithm::join(columns,","));

	// add table to the records table
	{	sqlite3x::sqlite3_command cmd(*con,"insert into records values (?,?,?,?,?,?);"); cmd.bind(1,(sqlite3x::int64_t)Omega::instance().getCurrentIteration()); cmd.bind(2,(double)Omega::instance().getComputationTime()); cmd.bind(3,(double)Omega::instance().getSimulationTime()); cmd.bind(4,(double)PeriodicEngine::getClock()); cmd.bind(5,perBodyTable); cmd.bind(6,""); cmd.executenonquery(); }

	// loop over bodies
	sqlite3x::sqlite3_transaction transaction(*con);
	{
		sqlite3x::sqlite3_command cmd(*con,"insert into "+perBodyTable+" values ("+algorithm::join(vector<string>(columns.size(),"?"),",")+");");
		FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
			// placeholders are 1-based (!!!)
			size_t field=1; cmd.bind(field++,b->getId());
			if(recActive[REC_SE3]){
				const Se3r& se3=b->physicalParameters->se3;
				cmd.bind(field++,(double)se3.position[0]); cmd.bind(field++,(double)se3.position[1]); cmd.bind(field++,(double)se3.position[2]);
				cmd.bind(field++,(double)se3.orientation[0]); cmd.bind(field++,(double)se3.orientation[1]); cmd.bind(field++,(double)se3.orientation[2]); cmd.bind(field++,(double)se3.orientation[3]);
			}
			if(recActive[REC_RGB]){
				const Vector3r& color=b->geometricalModel->diffuseColor;
				cmd.bind(field++,(double)color[0]); cmd.bind(field++,(double)color[1]); cmd.bind(field++,(double)color[2]);
			}
			assert(field-1==columns.size());
			cmd.executenonquery();
		}
		con->executenonquery("UPDATE 'meta' SET maxIter="+lexical_cast<string>(Omega::instance().getCurrentIteration())+";");
	}
	transaction.commit();
}

