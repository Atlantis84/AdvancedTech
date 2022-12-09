#include "threadpool.h"

ThreadPool::ThreadPool(unsigned short size)
{
    _initSize = size; addThread(size);
}

ThreadPool::~ThreadPool()
{
    _run = false;
    _task_cv.notify_all(); // 唤醒所有线程执行
    for (thread& thread : _pool) {
        if (thread.joinable())
            thread.join();
    }
}

void ThreadPool::addThread(unsigned short size)
{
    for (; _pool.size() < THREADPOOL_MAX_NUM && size > 0; --size)
    {
        _pool.emplace_back([this] {
            while (true)
            {
                Task task;
                {
                    unique_lock<mutex> lock{ _lock };
                    _task_cv.wait(lock, [this] { // wait 直到有 task, 或需要停止
                        return !_run || !_tasks.empty();
                    });
                    if (!_run && _tasks.empty())
                        return;

                    _idlThrNum--;
                    task = move(_tasks.front()); // 按先进先出从队列取一个 task
                    _tasks.pop();
                }
                task();//执行任务
                {
                    unique_lock<mutex> lock{ _lock };
                    _idlThrNum++;
                }
            }
        });
        {
            unique_lock<mutex> lock{ _lock };
            _idlThrNum++;
        }
    }
}

//template<class F, class... Args>
//auto ThreadPool::commit_to_thread_pool(F&& f, Args&&... args) -> future<decltype(f(args...))>
//{
//    if (!_run)
//        throw runtime_error("commit on ThreadPool is stopped.");

//    using RetType = decltype(f(args...));
//    auto task = make_shared<packaged_task<RetType()>>(
//        bind(forward<F>(f), forward<Args>(args)...)
//    );
//    future<RetType> future = task->get_future();
//    {
//        lock_guard<mutex> lock{ _lock };
//        _tasks.emplace([task]() {
//            (*task)();
//        });
//    }

//    if (_idlThrNum < 1 && _pool.size() < THREADPOOL_MAX_NUM)
//        addThread(1);

//    _task_cv.notify_one();
//    return future;
//}
