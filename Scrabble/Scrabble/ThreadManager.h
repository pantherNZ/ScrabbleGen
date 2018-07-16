#pragma once

#ifndef __CTHREADMANAGER_H__
#define __CTHREADMANAGER_H__

// Library Includes:
#include <windows.h>
#include <vector>

// Local Includes:
#include "Thread.h"

// Class declaration
class CThreadManager
{
	// Class functions
	public:
		CThreadManager(); // Ctor
		~CThreadManager(); // Dtor
		void Initialise();
		HANDLE CreateThread(unsigned (__stdcall* functionPtr) (void *), void* _vpData);
		void Update(DWORD _ms);

		bool StopThreads();
		bool ResumeThreads();
		bool TerminateThreads();

		int GetActiveThreadCount() const;

	protected:	

	private:

	// class variables
	public:

	protected:

	private:
		std::vector<CThread*> m_pThreads;
		int m_iProcessors;
		int m_iThreadCount;
};

#endif // __CTHREADMANAGER_H__
