/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "ThreadWorker.hpp"

void ThreadWorker::setTerminate(bool b)
{
	boost::mutex::scoped_lock lock(m_boolmutex);
	m_should_terminate=b;
};

bool ThreadWorker::shouldTerminate()
{
	boost::mutex::scoped_lock lock(m_boolmutex);
	return m_should_terminate;
};
		
signed char ThreadWorker::progress()
{
	return 0;
};

void ThreadWorker::setReturnValue(boost::any a)
{
	m_val = a;
};

boost::any ThreadWorker::getReturnValue()
{
	return m_val;
};

bool ThreadWorker::done()
{
	boost::mutex::scoped_lock lock(m_boolmutex);
	return m_done;
};

void ThreadWorker::callSingleAction()
{
	{
		boost::mutex::scoped_lock lock(m_boolmutex);
		m_done = false;
	}
	this->singleAction();
	{
		boost::mutex::scoped_lock lock(m_boolmutex);
		m_done = true;
	}
};

