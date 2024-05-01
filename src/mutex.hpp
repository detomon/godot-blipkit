#pragma once

#include <godot_cpp/core/defs.hpp>
#include <mutex>

namespace BlipKit {

class RecursiveMutex {
private:
	std::recursive_mutex mutex;

public:
	_ALWAYS_INLINE_ void lock() {
		mutex.lock();
	}

	_ALWAYS_INLINE_ void unlock() {
		mutex.unlock();
	}
};

template <typename MutexT>
class MutexLock {
private:
	MutexT &mutex;

public:
	_ALWAYS_INLINE_ MutexLock(MutexT &p_mutex) :
			mutex(p_mutex) {
		mutex.lock();
	}

	_ALWAYS_INLINE_ ~MutexLock() {
		mutex.unlock();
	}
};

} //namespace BlipKit
