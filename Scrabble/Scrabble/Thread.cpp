// Library Includes
#include <process.h>

// Local Includes

// Slef Include
#include "Thread.h"

// Static Function Prototypes

// Implementation
CThread::CThread() : m_iThreadNumber(0), m_bAlive(false)
{

}

CThread::~CThread()
{

}

// This function should be used to create the Thread and then causes the Thread to begin execution by calling the run() method with the created thread
HANDLE CThread::Start(unsigned (__stdcall* functionPtr) (void *), void* _vpData)
{
	//m_bAlive = true;
	m_pThreadHandle = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, functionPtr, _vpData, THREAD_TERMINATE, &m_iThreadNumber)); //THREAD_SUSPEND_RESUME

	return(m_pThreadHandle);
}

bool CThread::Terminate()
{
	if(TerminateThread(m_pThreadHandle, 0) == 0)
	{
		return(false);
	}

	return(true);
}

// This method should force the calling thread to wait for the Thread on which it is invoked to complete.
bool CThread::Join(DWORD _ms)
{
	DWORD ret = WaitForSingleObject(m_pThreadHandle, _ms);

	if(ret != WAIT_FAILED && ret == WAIT_OBJECT_0)
	{
		return(true);
	}

	return(false);
}

// This function should stop a currently running thread
bool CThread::Stop()
{
	if(SuspendThread(m_pThreadHandle) == (DWORD) -1)
	{
		return(false);
	}

	return(true);
}

// This function should be used to resume a previously stopped thread
bool CThread::Resume()
{
	if(ResumeThread(m_pThreadHandle) == (DWORD) -1)
	{
		return(false);
	}

	return(true);
}
 
// Function to retrieve a thread's ID.
unsigned int CThread::GetThreadID() const
{
	return(m_iThreadNumber);
}