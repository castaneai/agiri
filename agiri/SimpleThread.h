#pragma once
#include <Windows.h>
#include <tchar.h>

typedef void (*THREAD_FUNC)(void);

class SimpleThread
{
public:
	SimpleThread(THREAD_FUNC threadFunc)
		: func(nullptr), eventContext(nullptr)
	{
		this->func = threadFunc;
		this->eventContext = CreateEvent(nullptr, true, false, _T("threadContext"));
	}

	void Start()
	{
		QueueUserWorkItem(this->_queueThreadFunc, this, 0);
	}

	void Join(int timeoutMilliseconds = INFINITE)
	{
		WaitForSingleObject(this->eventContext, timeoutMilliseconds);
	}

private:
	THREAD_FUNC func;
	HANDLE eventContext;

	static DWORD WINAPI _queueThreadFunc(LPVOID context)
	{
		auto thread = reinterpret_cast<SimpleThread*>(context);
		thread->func();
		SetEvent(thread->eventContext);
		return 0;
	}
};