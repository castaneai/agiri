#pragma once
#include <Windows.h>
#include <tchar.h>

typedef void(*THREAD_FUNC)(void);

class SimpleThread
{
public:
	SimpleThread(THREAD_FUNC threadFunc)
		: func(threadFunc), eventContext(CreateEvent(nullptr, true, false, _T("threadContext")))
	{
	}

	void Start() const
	{
		QueueUserWorkItem(this->_queueThreadFunc, const_cast<SimpleThread*>(this), 0);
	}

	void Join(int timeoutMilliseconds = INFINITE)
	{
		WaitForSingleObject(this->eventContext, timeoutMilliseconds);
	}

private:
	const THREAD_FUNC func;
	const HANDLE eventContext;

	static DWORD WINAPI _queueThreadFunc(LPVOID context)
	{
		auto thread = reinterpret_cast<SimpleThread*>(context);
		thread->func();
		SetEvent(thread->eventContext);
		return 0;
	}
};
