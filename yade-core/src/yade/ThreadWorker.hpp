/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef THREAD_WORKER_HPP
#define THREAD_WORKER_HPP

#include <boost/thread/mutex.hpp>
#include <boost/any.hpp>

class ThreadRunner;

/*! 
\brief	ThreadWorker contains information about tasks to be performed when
	the separate thread is executed.
 */

class ThreadWorker	//         perhaps simulation steps, or stage? as it is a single stage
			// of the simulation, that consists of several steps
			// Update: it is more general now. simulation stages perhaps will be derived from this class
{
	private:
		friend class ThreadRunner;
		bool		m_should_terminate;
		bool		m_done;
		boost::mutex	m_boolmutex;
		boost::any	m_val;
		void		setTerminate(bool);
		void		callSingleAction();

	protected:
		/// singleAction() can check whether someone asked for termination, and terminate if/when possible
		bool		shouldTerminate();
		void		setReturnValue(boost::any);
		/// derived classes must define this method
		virtual void	singleAction() = 0;

	public:
		ThreadWorker() : m_should_terminate(false), m_done(false) {};
		virtual		~ThreadWorker() {};

		/// Returns a value between 0 and 100 percent. Reimplement if necessary.
		virtual signed char progress();
		boost::any	getReturnValue();
		bool		done();
};

#endif

