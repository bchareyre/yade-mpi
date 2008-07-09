// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#pragma once
#include<yade/pkg-common/PeriodicEngines.hpp>
#include<yade/lib-sqlite3x/sqlite3x.hpp>

class SQLiteRecorder: public PeriodicEngine {
		shared_ptr<sqlite3x::sqlite3_connection> con;
	public:
		enum {REC_SE3=0,REC_RGB,REC_SENTINEL};
		vector<string> recorders;
		string dbFile;
		SQLiteRecorder() {};
		~SQLiteRecorder(){ if(con) con->close(); }
		void init(MetaBody*);
		virtual void registerAttributes(){
			PeriodicEngine::registerAttributes();
			REGISTER_ATTRIBUTE(recorders);
			REGISTER_ATTRIBUTE(dbFile);
		}
		virtual void action(MetaBody*);
	DECLARE_LOGGER;
	REGISTER_CLASS_NAME(SQLiteRecorder);
	REGISTER_BASE_CLASS_NAME(PeriodicEngine);
};
REGISTER_SERIALIZABLE(SQLiteRecorder,false);


