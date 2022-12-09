#ifndef FUNCTIONBINDTESTWGT_H
#define FUNCTIONBINDTESTWGT_H

#include <QWidget>

class FunctionBindTestWgt{
public:
    explicit FunctionBindTestWgt();

    int test_add(int a,int b);

    void thread_function_test();
    void print_max_value(int v);
    void print_min_value(int v);

    void thread_pool_test();
};

#endif // FUNCTIONBINDTESTWGT_H
