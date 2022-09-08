#pragma once
#include <windows.h>

// 把锁封装成基类
class InstanceLock;
class InstanceLockBase
{
	friend class InstanceLock;
	CRITICAL_SECTION cs;

	void Lock() {
		::EnterCriticalSection(&cs);
	}
	void Unlock() {
		::LeaveCriticalSection(&cs);
	}

protected:
	InstanceLockBase() {
		InitializeCriticalSection(&cs);
	}
	~InstanceLockBase() {
		DeleteCriticalSection(&cs);
	}
};

// 对资源进行加锁
class InstanceLock {
	InstanceLockBase* _pObj;
public:
	InstanceLock(InstanceLockBase* pObj)
	{
		_pObj = pObj;
		if (NULL != _pObj) {
			_pObj->Lock();
		}
	}
	~InstanceLock()
	{
		if (NULL != _pObj) {
			_pObj->Unlock();
		}
	}
};