#ifndef RWLOCK__H
#define RWLOCK__H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation(use a .cpp suffix)
#endif

#include <mutex>
#include <condition_variable>

class RWLock {
  public:
    RWLock();
    virtual ~RWLock() = default;

    void lockWrite();
    void unlockWrite();
    void lockRead();
    void unlockRead();

  private:
    volatile int m_readCount;
    volatile int m_writeCount;
    volatile bool m_isWriting;
    std::mutex m_Lock;
    std::condition_variable m_readCond;
    std::condition_variable m_writeCond;
};

class ReadGuard {
  public:
    explicit ReadGuard(RWLock& lock);
    virtual ~ReadGuard();

  private:
    ReadGuard(const ReadGuard&);
    ReadGuard& operator=(const ReadGuard&);

  private:
    RWLock &m_lock;
};


class WriteGuard {
  public:
    explicit WriteGuard(RWLock& lock);
    virtual ~WriteGuard();

  private:
    WriteGuard(const WriteGuard&);
    WriteGuard& operator=(const WriteGuard&);

  private:
    RWLock& m_lock;
};

#endif  // RWLOCK__H
