#pragma once

#include <assert.h>
#include <chrono>

#include <mutex>

namespace common {
class Mutex {
public:
    using Timeout = std::chrono::milliseconds;

public:
    Mutex() = default;

    ~Mutex() = default;

    Mutex( const Mutex& other ) = delete;
    Mutex( const Mutex&& other ) = delete;
    Mutex& operator=( const Mutex& other ) = delete;

    //
    // Note: take() by default will block while getting the semaphore for infinite time
    //
    inline bool take( Timeout timeout = Timeout( 0 ) ) {
        if( timeout == Timeout( 0 ) ) {
        	m_mutex.lock();
            return true;
        }
        else {
        	m_mutex.lock();
			return true;
        }
    }

    inline bool release() {
    	m_mutex.unlock();
        return true;
    }

private:
    std::mutex m_mutex;
};
};  // namespace common
