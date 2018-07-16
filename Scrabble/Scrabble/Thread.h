#pragma once

#ifndef __CTHREAD_H__
#define __CTHREAD_H__

// Library Includes:
#include <windows.h>

// Local Includes:

// Class declaration
class CThread
{
	// Class functions
	public:
		CThread(); // Ctor of the class
		virtual ~CThread(); // Virtual Dtor

		HANDLE Start(unsigned (__stdcall* functionPtr) (void *), void* _vpData); // This function should be used to create the Thread and then causes the Thread to begin execution by calling the run() method with the created thread
		bool Join(DWORD _ms); // This method should force the calling thread to wait for the Thread on which it is invoked to complete.
		bool Stop(); // This function should stop a currently running thread
		bool Resume(); // This function should be used to resume a previously stopped thread
		bool Terminate();
		unsigned int GetThreadID() const; // Function to retrieve a thread's ID.

	protected:	

	private:

	// class variables
	public:

	protected:

	private:
		unsigned int m_iThreadNumber;
		HANDLE m_pThreadHandle;
		bool m_bAlive;
};

#endif // __CTHREAD_H__
