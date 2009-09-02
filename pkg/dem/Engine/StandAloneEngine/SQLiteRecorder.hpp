// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#pragma once
#include<yade/pkg-common/PeriodicEngines.hpp>
#include<yade/lib-sqlite3x/sqlite3x.hpp>
/*
 * Extensible class for recording of structured data for later use with simulation player (uses sqlite).
 *
 * You need to set recorders and dbFile to use this engine. The resulting file is directly supported
 * by the simulation player (and can be browsed/processed using regular sqlite means) and contains
 * the whole simulation setup (the XML is embedded in the db and loaded by player) and saved states.
 * YOU NEED ONLY THIS SINGLE FILE to replay the simulation.
 *
 * Available recorders are: 'se3' (position and orientation) and 'rgb' (geometricalModel::diffuseColor).
 *
 * See scripts/simple-scene-player.py for example.
 *
 * Speed: the db is considerably faster for reading (replaying) and slightly slower for writing (with simple-scene).
 *
 * If the database file exists already, it is used iff the last iteration that was saved in this file is smaller
 * the current iteration. Otherwise the file is renamed to *~1 (or *~2 etc. if that exists already) and
 * new database is created. This permits to save, reload and rerun the simulation while writing into the same
 * database, but when you run it again, the results will be saved to different db and the old one backed up.
 *
 * Extensibility: if you want to add your own recorder, use this checklist:
 *
 * 1. add REC_NEWREC to the enum (keep REC_SENTINEL as last!)
 * 2. go through the action(MetaBody*) code and add appropriate conditionals (REC_SE3 and REC_RGB will guid you).
 * 2a. add the recorder activation condition
 * 2b. add appropriate columns to the database table. Name those newrec_*
 * 2c. insert your values into the INSERT as appropriate
 * 3. Edit GLSimulationPlayerViewer::loadNextRecordedData, test if your columns are present,
 * 	get their values and adjust simulation.
 *
 * Currently, only per-body records are supported (per-interaction records were in consideration, but the difficulty is
 * how to handle appearing/disappearing interactions).
 *
 */
class SQLiteRecorder: public PeriodicEngine {
		shared_ptr<sqlite3x::sqlite3_connection> con;
	public:
		enum {REC_SE3=0,REC_RGB,REC_SENTINEL};
		//! List of recorders that will be used
		vector<string> recorders;
		//! If false (default), interactions will not be saved to the initial XML inside the database
		bool saveInteractions;
		//! Filename of the database to save to
		string dbFile;
		SQLiteRecorder(): saveInteractions(false) { /* we always want to save the first state as well */ initRun=true; };
		~SQLiteRecorder(){ if(con) con->close(); }
		void init(MetaBody*);
		virtual void action(MetaBody*);
	REGISTER_ATTRIBUTES(PeriodicEngine,(recorders)(dbFile)(saveInteractions));
	REGISTER_CLASS_AND_BASE(SQLiteRecorder,PeriodicEngine);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(SQLiteRecorder);


