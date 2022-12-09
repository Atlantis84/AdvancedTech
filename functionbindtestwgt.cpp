#include "functionbindtestwgt.h"
#include <QDebug>
#include <functional>
#include <thread>
#include <future>
#include <iostream>
using std::cout;
using std::endl;
#include <vector>
using std::vector;
#include <sstream>
#include <string>
using std::string;
#include <algorithm>
using namespace std;
#include <QThread>
#include "threadpool.h"
FunctionBindTestWgt::FunctionBindTestWgt()
{

}

int FunctionBindTestWgt::test_add(int a, int b)
{
    return a+b;
}

void FunctionBindTestWgt::thread_function_test()
{
    vector<int> vecT = {1,2,3,4,5,6};
    std::packaged_task<int(const vector<int>&)> pl([](const vector<int>& vec)
    {
        return *(std::max_element(vec.begin(),vec.end()));
    });

    std::packaged_task<int(const vector<int>&)> pm([](const vector<int>& vec)
    {
        return *(std::min_element(vec.begin(),vec.end()));
    });

    std::packaged_task<void(std::shared_future<int>&)> res([](std::shared_future<int>& sfu)
    {
        std::shared_ptr<FunctionBindTestWgt> testWgt = std::make_shared<FunctionBindTestWgt>();
        std::function<void(int)> printMax = std::bind(&FunctionBindTestWgt::print_max_value,testWgt,std::placeholders::_1);
        sfu.wait();
        printMax(sfu.get());
     });

    std::packaged_task<void(std::shared_future<int>&)> res1([](std::shared_future<int>& sfu)
    {
        std::shared_ptr<FunctionBindTestWgt> testWgt = std::make_shared<FunctionBindTestWgt>();
        std::function<void(int)> printMin = std::bind(&FunctionBindTestWgt::print_min_value,testWgt,std::placeholders::_1);
        sfu.wait();
        printMin(sfu.get());
     });

    std::shared_future<int> s_fu(pl.get_future());
    std::shared_future<int> s_fu1(pm.get_future());
    if(pl.valid())
    {
        std::thread t1(ref(res), ref(s_fu));
        std::thread(ref(pl), ref(vecT)).join();
        t1.join();
    }
    if(pm.valid())
    {
        std::thread t1(ref(res1),ref(s_fu1));
        std::thread(ref(pm),ref(vecT)).join();
        t1.join();
    }
}

void FunctionBindTestWgt::print_max_value(int v)
{
    if(v%2 == 0)
        QThread::msleep(v);
    qDebug()<<"Max value of vector is:"<<v;
}

void FunctionBindTestWgt::print_min_value(int v)
{
    qDebug()<<"Min value of vector is:"<<v;
}

void FunctionBindTestWgt::thread_pool_test()
{
    std::shared_ptr<FunctionBindTestWgt> test = std::make_shared<FunctionBindTestWgt>();
    std::function<void(int)> print = std::bind(&FunctionBindTestWgt::print_max_value,test,std::placeholders::_1);
    ThreadPool tPool(64);
    for(int i=0;i<1000;i++)
        tPool.commit_to_thread_pool(print,i);
}
