#pragma once

#include "RuyiSDKDemo.h"
#include "AllowWindowsPlatformTypes.h"
//#include "windows.h" // any native windows header
#include "Windows/WindowsPlatformProcess.h"
#include "HideWindowsPlatformTypes.h"
#include "HAL/ThreadingBase.h"
//#include "DelegateBase.h"

/*
	A roughly-coded multithread class for executing ruyiSDK request, don't run ruyiSDK API in main render/logic thread 
*/

DECLARE_DELEGATE(FAsyncTaskWorkerDelegate);
DECLARE_DELEGATE_OneParam(FAsyncTaskWorkerDelegateOneStringParam, FString);
DECLARE_DELEGATE_TwoParams(FAsyncTaskWorkerDelegateTwoStringParam, FString, FString);
DECLARE_DELEGATE_ThreeParams(FAsyncTaskWorkerDelegateThreeIntParam, int, int, int);

DECLARE_DELEGATE_TwoParams(FAsyncTaskWorkerDelegateAuthentication, FString&, FString&);

class FAsyncTaskWorker : public FRunnable 
{
public:
	FAsyncTaskWorker();
	~FAsyncTaskWorker();

	virtual bool Init() override;
	virtual uint32 Run() override;
	//virtual void Stop() override;
	//virtual void Exit() override;

	void StartThread();
	void SetName(FString& name);
	void EndThread();
	void CleanAllBind();

	FAsyncTaskWorkerDelegate m_Delegate;
	FAsyncTaskWorkerDelegateOneStringParam m_DelegateOneStringParam;
	FAsyncTaskWorkerDelegateTwoStringParam m_DelegateTwoStringParam;
	FAsyncTaskWorkerDelegateThreeIntParam m_DelegateThreeIntParam;
	FString param1, param2;
	int intParam1, intParam2, intParam3;

	FString username, password;
	FAsyncTaskWorkerDelegateAuthentication m_DelegateAuthentication;

	bool m_End;
	bool m_StartDelegate;
private:
	FString m_Name;
	FRunnableThread* m_Thread;
	void* m_functionDelegate;

	FCriticalSection m_mutex;

	TArray<FRunnableThread*> m_ThreadPool;
};