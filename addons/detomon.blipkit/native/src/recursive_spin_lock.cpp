#include "recursive_spin_lock.hpp"

using namespace BlipKit;

static thread_local int lock_thread_id;
static std::atomic<int> lock_thread_id_inc = 0;

void RecursiveSpinLock::lock() {
	int thread_id = lock_thread_id;

	// Create new ID for current thread.
	if (unlikely(thread_id == 0)) {
		thread_id = ++lock_thread_id_inc;
		lock_thread_id = thread_id;
	}

	// Not locked or locked by other thread.
	if (lock_owner.load() != thread_id) {
		// Wait for lock to be released.
		while (true) {
			int expect = 0;
			if (lock_owner.compare_exchange_weak(expect, thread_id, std::memory_order_acquire, std::memory_order_relaxed)) {
				break;
			}
		}
	}

	// Increment locks held by current thread.
	lock_count++;
}

void RecursiveSpinLock::unlock() {
	// Current thread is not lock owner.
	if (unlikely(lock_owner.load() != lock_thread_id)) {
		return;
	}

	// Unlock.
	if (--lock_count <= 0) {
		lock_count = 0;
		lock_owner.store(0, std::memory_order_release);
	}
}
