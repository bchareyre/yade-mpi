#include"SQLiteRecorder.hpp"
#include<boost/algorithm/string.hpp>
#include<yade/core/MetaBody.hpp>
using namespace boost;
CREATE_LOGGER(SQLiteRecorder);

void SQLiteRecorder::init(MetaBody* rootBody){
	//sqlite3_open(dbFile.c_str(),&db);
	//LOG_DEBUG("Created db "<<dbFile);
	// create supertable: TODO
}


void SQLiteRecorder::action(MetaBody* rootBody){
	//if(!db) init(rootBody);
	vector<bool> recActive(REC_SENTINEL,false);
	FOREACH(string& rec, recorders){
		if(rec=="se3") recActive[REC_SE3]=true;
		else if(rec=="rgb") recActive[REC_RGB]=true;
		else LOG_ERROR("Unknown recorder named `"<<rec<<"' (supported are: se3, rgb). Ignored.");
	}
	// init db
	sqlite3x::sqlite3_connection con(dbFile);	
	con.executenonquery("PRAGMA synchronous = OFF");

	// create table
	string perBodyTable="bodies_iter_"+lexical_cast<string>(Omega::instance().getCurrentIteration());
	assert(0==con.executeint("select count(*) from sqlite_master where name='"+perBodyTable+"';")); // table shouldn't exist yet
	vector<string> columns; columns.push_back("id");
	if(recActive[REC_SE3]){columns.push_back("se3_x"); columns.push_back("se3_y"); columns.push_back("se3_z"); columns.push_back("se3_ori0"); columns.push_back("se3_ori1"); columns.push_back("se3_ori2"); columns.push_back("se3_ori3");}
	if(recActive[REC_RGB]){	columns.push_back("rgb_r"); columns.push_back("rgb_g"); columns.push_back("rgb_b");}
	con.executenonquery("create table "+perBodyTable+" ("+algorithm::join(columns,",")+");");
	LOG_DEBUG("Created table "<<perBodyTable<<" with columns "<<algorithm::join(columns,","));
	// add table metadata to supertable
	// TODO
	// loop over bodies
	sqlite3x::sqlite3_transaction transaction(con);
	{
		sqlite3x::sqlite3_command cmd(con,"insert into "+perBodyTable+" values("+algorithm::join(vector<string>(columns.size(),"?"),",")+");");
		FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
			// placeholders are 1-based (!!!)
			int field=1; cmd.bind(field++,b->getId());
			if(recActive[REC_SE3]){
				const Se3r& se3=b->physicalParameters->se3;
				cmd.bind(field++,se3.position[0]); cmd.bind(field++,se3.position[1]); cmd.bind(field++,se3.position[2]);
				cmd.bind(field++,se3.orientation[0]); cmd.bind(field++,se3.orientation[1]); cmd.bind(field++,se3.orientation[2]); cmd.bind(field++,se3.orientation[3]);
			}
			if(recActive[REC_RGB]){
				const Vector3r& color=b->geometricalModel->diffuseColor;
				cmd.bind(field++,color[0]); cmd.bind(field++,color[1]); cmd.bind(field++,color[2]);
			}
			assert(field-1==(int)columns.size());
			cmd.executenonquery();
		}
	}
	transaction.commit();
	con.close();
}

