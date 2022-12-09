#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <queue>
#include <atomic>
#include <future>
#include <stdexcept>
using namespace std;
#define  THREADPOOL_MAX_NUM 16
class ThreadPool
{
public:
    ThreadPool(unsigned short size = 4);
    template<class F,class... Args>
    auto commit_to_thread_pool(F&& f, Args&&... args) -> future<decltype(f(args...))>
    {
        if (!_run)
            throw runtime_error("commit on ThreadPool is stopped.");

        using RetType = decltype(f(args...));
        auto task = make_shared<packaged_task<RetType()>>(
            bind(forward<F>(f), forward<Args>(args)...)
        );
        future<RetType> future = task->get_future();
        {
            lock_guard<mutex> lock{ _lock };
            _tasks.emplace([task]() {
                (*task)();
            });
        }

        if (_idlThrNum < 1 && _pool.size() < THREADPOOL_MAX_NUM)
            addThread(1);

        _task_cv.notify_one();
        return future;
    }
    ~ThreadPool();
private:
    atomic<bool> _run{ true };
    void addThread(unsigned short size);

    unsigned short _initSize;
    using Task = function<void()>;
    vector<thread> _pool;
    queue<Task> _tasks;
    mutex _lock;

    condition_variable _task_cv;   //Ìõ¼þ×èÈû
    atomic<int>  _idlThrNum{ 0 };
};

#endif // THREADPOOL_H
