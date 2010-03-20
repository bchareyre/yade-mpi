/*************************************************************************
*  Copyright (C) 2009 by Vincent Richefeu                                *
*  Vincent.Richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/DataRecorder.hpp>
#include <string>
#include <fstream>

class HistoryRecorder : public DataRecorder
{
public :
	
	string outputBase; 
    int interval;
    int stateId;
	
	HistoryRecorder ();
	~HistoryRecorder ();
	virtual void action();
	virtual bool isActivated(Scene*);

protected :

	virtual void postProcessAttributes(bool deserializing);
	REGISTER_CLASS_NAME(HistoryRecorder);
	REGISTER_BASE_CLASS_NAME(DataRecorder);
	REGISTER_ATTRIBUTES(DataRecorder,(outputBase)(interval)(stateId));
};

REGISTER_SERIALIZABLE(HistoryRecorder);


