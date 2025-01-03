#pragma once

#include <atomic>
#include <godot_cpp/core/defs.hpp>

namespace BlipKit {

struct RecursiveSpinLock {
private:
	std::atomic<int> lock_owner = 0;
	int lock_count = 0;

public:
	struct Autolock {
		friend class RecursiveSpinLock;

	private:
		RecursiveSpinLock &spin_lock;

		Autolock(RecursiveSpinLock &p_spin_lock) :
				spin_lock(p_spin_lock) {}

	public:
		_FORCE_INLINE_ void unlock() { spin_lock.unlock(); }
		_FORCE_INLINE_ ~Autolock() { unlock(); }
	};

	void lock();
	void unlock();
	_FORCE_INLINE_ Autolock autolock() { return Autolock(*this); }
};

} //namespace BlipKit
