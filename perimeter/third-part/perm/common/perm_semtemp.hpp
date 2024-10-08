#ifndef PERM_SEMPRIVIMPL_HPP
#define PERM_SEMPRIVIMPL_HPP

#include <deque> // C++ 11
#include <condition_variable> // C++ 11
#include <mutex> // C++ 11
#include <atomic> // C++ 11
#include <climits>

namespace FcPerm {

// ///////////////////////////////////////////////////////////////////////
//
// private semaphore implement
//
// ///////////////////////////////////////////////////////////////////////
class  SemTemp {
private: 
    std::condition_variable  m_cv; 
    std::mutex   m_mtx; 
    std::atomic<int> m_count;
public :
    SemTemp( unsigned int c = 0 ) : m_count( c ) { }
    ~SemTemp( ) { } // std::unique_lock< std::mutex > lock( m_mtx ); lock.unlock(); }
    void notify( )
    {
        std::unique_lock< std::mutex > lock( m_mtx );
        if ( m_count.load() < INT_MAX ) { m_count.fetch_add(1); }
        m_cv.notify_one();
    }
    void  wait( )
    {
        std::unique_lock< std::mutex > lock( m_mtx );
        m_cv.wait( lock, [ this ]() { return m_count.load() > 0; } );
        m_count.fetch_sub(1);
    }
    template < class tClock, class tDuration >
    bool  waitUntil( const std::chrono::time_point< tClock, tDuration> &point )
    {
        std::unique_lock< std::mutex > lock( m_mtx );
        if ( ! m_cv.wait_until( lock, point, [this](){ return m_count.load() > 0; }) ) {
            return false;
        }
        m_count.fetch_sub(1);
        return true;
    }
};


}

#endif
