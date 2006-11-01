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
		boost::mutex	m_mutex;
		boost::any	m_val;
		float		m_progress;
		std::string	m_message;
		void		setTerminate(bool);
		void		callSingleAction();

	protected:
		/// singleAction() can check whether someone asked for termination, and terminate if/when possible
		bool		shouldTerminate();
		void		setReturnValue(boost::any);
		void		setProgress(float);
		void		setMessage(std::string);
		/// derived classes must define this method
		virtual void	singleAction() = 0;

	public:
		ThreadWorker() : m_should_terminate(false), m_done(false), m_progress(0) {};
		virtual		~ThreadWorker() {};

		/// Returns a value between 0.0 and 1.0. Call setProgress() to set it.
		float		progress();
		std::string	message();
		boost::any	getReturnValue();
		bool		done();
};

#endif

