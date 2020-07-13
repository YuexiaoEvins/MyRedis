#include "RWLock.h"

RWLock::RWLock()
    : m_readCount(0)
    , m_writeCount(0)
    , m_isWriting(false) {
    }

void RWLock::lockRead() {
    std::unique_lock<std::mutex> gurad(m_Lock);
    m_readCond.wait(gurad, [=] { return 0 == m_writeCount; });
    ++m_readCount;
}

void RWLock::unlockRead() {
    std::unique_lock<std::mutex> gurad(m_Lock);
    if (0 == (--m_readCount)
        && m_writeCount > 0) {
        // One write can go on
        m_writeCond.notify_one();
    }
}

void RWLock::lockWrite() {
    std::unique_lock<std::mutex> gurad(m_Lock);
    ++m_writeCount;
    m_writeCond.wait(gurad, [=] { return (0 == m_readCount) && !m_isWriting; });
    m_isWriting = true;
}

void RWLock::unlockWrite() {
    std::unique_lock<std::mutex> gurad(m_Lock);
    m_isWriting = false;
    if (0 == (--m_writeCount)) {
        // All read can go on
        m_readCond.notify_all();
    } else {
        // One write can go on
        m_writeCond.notify_one();
    }
}

ReadGuard::ReadGuard(RWLock &lock)
    : m_lock(lock) {
    m_lock.lockRead();
}

ReadGuard::~ReadGuard() {
    m_lock.unlockRead();
}

WriteGuard::WriteGuard(RWLock &lock)
    : m_lock(lock) {
    m_lock.lockWrite();
}

WriteGuard::~WriteGuard() {
    m_lock.unlockWrite();
}

