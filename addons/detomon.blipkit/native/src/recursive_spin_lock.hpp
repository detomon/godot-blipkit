#pragma once

#include <atomic>
#include <godot_cpp/core/defs.hpp>

namespace BlipKit {

struct RecursiveSpinLock {
private:
	std::atomic<uint32_t> lock_owner = 0;
	uint32_t lock_count = 0;

public:
	struct Autolock {
		friend struct RecursiveSpinLock;

	private:
		RecursiveSpinLock &spin_lock;

		Autolock(RecursiveSpinLock &p_spin_lock) :
				spin_lock(p_spin_lock) {}

	public:
		_ALWAYS_INLINE_ void unlock() { spin_lock.unlock(); }
		_ALWAYS_INLINE_ ~Autolock() { unlock(); }
	};

	void lock();
	void unlock();
	_ALWAYS_INLINE_ Autolock autolock() { return Autolock(*this); }
};

} //namespace BlipKit
