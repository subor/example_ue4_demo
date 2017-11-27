
#include "AsyncTaskWorker.h"

FAsyncTaskWorker::FAsyncTaskWorker() 
{
	m_Thread = nullptr;
	param1 = TEXT("");
	param2 = TEXT("");
	m_End = false;
	m_StartDelegate = false;
}
FAsyncTaskWorker::~FAsyncTaskWorker()
{
	//delete m_Thread;
	//m_Thread = nullptr;
}

bool FAsyncTaskWorker::Init() 
{
	return true;
}

uint32 FAsyncTaskWorker::Run() 
{
	//FPlatformProcess::Sleep(0.03);
	while (!m_End)
	{
		if (!m_StartDelegate) 
		{
			m_StartDelegate = true;

			if (m_DelegateAuthentication.IsBound())
			{
				//m_DelegateAuthentication.ExecuteIfBound(username, password);
			}
		}
	}
	return 0;
}

void FAsyncTaskWorker::SetName(FString& name)
{
	m_Name = name;
}

void FAsyncTaskWorker::StartThread() 
{
	m_End = false;
	m_StartDelegate = false;

	m_Thread = FRunnableThread::Create(this, *m_Name, 0, TPri_BelowNormal);
	//m_ThreadPool.Add(m_Thread);
}

void FAsyncTaskWorker::EndThread() 
{
	//FPlatformProcess::Sleep(0.5);
	m_End = true;
	if (nullptr != m_Thread) 
	{
		m_Thread->Kill();
		//delete m_Thread;
		m_Thread = nullptr;
	}
}

void FAsyncTaskWorker::CleanAllBind() 
{
	m_DelegateAuthentication.Unbind();
}