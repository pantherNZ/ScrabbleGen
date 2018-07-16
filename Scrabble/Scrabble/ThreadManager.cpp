// Self Include
#include "ThreadManager.h"

// Local Includes
#include "Utility.h"

// Library Includes

// Static Function Prototypes

// Implementation
CThreadManager::CThreadManager() : m_iProcessors(0), m_iThreadCount(0)
{

}

CThreadManager::~CThreadManager()
{
	for(unsigned int i = 0; i < m_pThreads.size(); ++i)
	{
		DeleteOBJ(m_pThreads[i]);
	}
}

void CThreadManager::Initialise()
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);

	m_iProcessors = sysinfo.dwNumberOfProcessors;
}

HANDLE CThreadManager::CreateThread(unsigned (__stdcall* functionPtr) (void *), void* _vpData)
{
	// check for a spare processor slot
	while(m_iThreadCount >= m_iProcessors) 
	{ 
		Update(100);
	}

	++m_iThreadCount;
	CThread* pThread = new CThread;
	m_pThreads.push_back(pThread);
	return(pThread->Start(functionPtr, _vpData));
}

void CThreadManager::Update(DWORD _ms)
{
	for(unsigned int i = 0; i < m_pThreads.size(); ++i)
	{
		// if thread has successfully joined with main thread
		if(m_pThreads.at(i)->Join(_ms))
		{	
			delete m_pThreads.at(i);
			m_pThreads.at(i) = nullptr;
			m_pThreads.erase(m_pThreads.begin() + i);
			--m_iThreadCount;
		}
	}
}

bool CThreadManager::StopThreads()
{
	bool bStopped = false;

	for(unsigned int i = 0; i < m_pThreads.size(); ++i)
	{
		if(m_pThreads.at(i)->Stop())
		{
			bStopped = true;
		}
	}

	return(bStopped);
}

bool CThreadManager::ResumeThreads()
{
	bool bResumed = false;

	for(unsigned int i = 0; i < m_pThreads.size(); ++i)
	{
		if(m_pThreads.at(i)->Resume())
		{
			bResumed = true;
		}
	}

	return(bResumed);
}

bool CThreadManager::TerminateThreads()
{
	bool bTerminated = false;

	for(unsigned int i = 0; i < m_pThreads.size(); ++i)
	{
		if(m_pThreads.at(i)->Terminate())
		{
			bTerminated = true;
			delete m_pThreads.at(i);
			m_pThreads.at(i) = nullptr;
			m_pThreads.erase(m_pThreads.begin() + i);
			--m_iThreadCount;
		}
	}

	return(bTerminated);
}

int CThreadManager::GetActiveThreadCount() const
{
	return(m_iThreadCount);
}
