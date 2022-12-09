#include <QCoreApplication>
#include "QsLog.h"
#include "QsLogDest.h"
#include <QDateTime>
#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QNetworkReply>
#include <iostream>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QTimer>
#include <QThread>
#include "functionbindtestwgt.h"
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
using namespace QsLogging;
#define PUBLIC_KEY "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAqyHeiIAoTZ1I3Nb9KE8DoSMPWEMdA4ilGh0yGdUzqvbU2XfpK5aNIen9aWqDUb7hV5rjSc6ab/sw5Zp2EJtGKwZB/JzVaLIwlx7Dvijx9FojiQ1Ca+7qJF74b9CnDkIWrIsKBiHvFQQfgxKUACfXxwsLirfE8pcE309wcXlldBcGFY/UgyYi6MJZcP7Es9a44Fb3cH8iluEBw3x85BvGhAsFMp/1cx4FcUflgSr/L7qWIJbH90yuSaQEFJd8xMb2sJViZrogknD4KQzMd7gmdygaDtyxujh517U/vpgNGbxgpaCLt4u8EZDyP+Pbyxu4TI1LZlv3RJry95M7dn2mQIDAQAB"

void PromiseID(std::promise<std::thread::id> &po)
{
    try {
        po.set_value(std::this_thread::get_id());
    } catch (const std::exception &e) {
        po.set_exception(std::current_exception());
    }
}

int GetVectorMax(const vector<int>& vec)
{
    return *(std::max_element(vec.begin(),vec.end()));
}

void PrintIntValueOnShared(std::shared_future<int>& s_fu)
{
    s_fu.wait();
    std::stringstream ss;
    ss<<std::this_thread::get_id()<<" Value "<<s_fu.get()<<";\r\n";
    cout<<ss.str();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Logger& logger = Logger::instance();
    logger.setLoggingLevel(QsLogging::TraceLevel);

    QString ttl = QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss");
    QString tmpStr = "\Log\\log.txt";
    const QString sLogPath(QDir(QApplication::applicationDirPath()).filePath(tmpStr));
    DestinationPtr fileDestination(DestinationFactory::MakeFileDestination(sLogPath, EnableLogRotation, MaxSizeBytes(256*1024), MaxOldLogCount(5000)));
    logger.addDestination(fileDestination);

    //test4
    if(1)
    {
        FunctionBindTestWgt fWgt;
        fWgt.thread_function_test();
        fWgt.thread_pool_test();
        return -1;
    }
    //test4

    //test3
    if(1)
    {
        vector<int> vec = {1,2,3,4,5,6};
        std::packaged_task<int(const vector<int>&)> pl([](const vector<int>& vec)
        {
            return *(std::max_element(vec.begin(),vec.end()));
        });

        std::packaged_task<int(std::shared_future<int>&)> res([](std::shared_future<int>& sfu)
        {
            sfu.wait();
            std::stringstream ss;
            ss<<" Value "<<sfu.get()<<";\r\n";
            cout<<ss.str();
            return sfu.get();
         });

        std::shared_future<int> s_fu(pl.get_future());
        if(pl.valid())
        {
            std::thread t1(ref(res), ref(s_fu));
//            std::thread t2(&PrintIntValueOnShared, ref(s_fu));
//            std::thread t3(&PrintIntValueOnShared, ref(s_fu));

            QThread::msleep(500);

            std::thread(ref(pl), ref(vec)).join();

            t1.join();
//            t2.join();
//            t3.join();
        }
    }

    //test3

    //test2
    vector<int> vec = {1,2,3,4,5,6};
    std::packaged_task<int(const vector<int>&)> pt(GetVectorMax);
    std::shared_future<int> s_fu(pt.get_future());
    std::thread t1(&PrintIntValueOnShared, ref(s_fu));
    std::thread t2(&PrintIntValueOnShared, ref(s_fu));
    std::thread t3(&PrintIntValueOnShared, ref(s_fu));
    QThread::msleep(500);
    std::thread(ref(pt), ref(vec)).join();
    t1.join();
    t2.join();
    t3.join();
    //test2

    //test1
    std::promise<std::thread::id> p1;
    std::promise<std::thread::id> p2;
    std::future<std::thread::id> f1(p1.get_future());
    std::future<std::thread::id> f2(p2.get_future());
    std::thread th1(&PromiseID,ref(p1));
    std::thread th2(&PromiseID,ref(p2));
    cout<<"th1 id is:"<<f1.get();
    cout<<"th2 id is:"<<f2.get();
    th1.join();
    th2.join();
    //test1

    //std::bind¡¢std::function
    std::shared_ptr<FunctionBindTestWgt> testWgt = std::make_shared<FunctionBindTestWgt>();
    std::function<int(int,int)> addFunc = std::bind(&FunctionBindTestWgt::test_add,testWgt,std::placeholders::_1,std::placeholders::_2);
    qDebug()<<addFunc(1,2);
    //std::bind¡¢std::function
    while(1)
    {
        std::string s;
//        getline(cin,s);
//        cout<<s<<endl;

        QNetworkReply* reply;
        QEventLoop eventloop;
        QNetworkRequest* request = new QNetworkRequest();
        QString productsn = "TG21B275ED";
        QString tmpurl;
        tmpurl = "http://";
        tmpurl.append("172.16.123.202");
        tmpurl.append(":");
        tmpurl.append("8097");
        tmpurl.append("/ProductionForCenterControl/GetProduction?sn=");
        tmpurl.append(productsn);

        request->setUrl(QUrl(tmpurl));
        request->setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));
        request->setRawHeader("appId","CenterController");
        request->setRawHeader("publicKey",PUBLIC_KEY);

        QNetworkAccessManager* tmpaccessManager = new QNetworkAccessManager();
        reply = tmpaccessManager->get(*request);
        QObject::connect(reply,SIGNAL(finished()),&eventloop,SLOT(quit()));
        //set get request time out
        QTimer::singleShot(1000,&eventloop,&QEventLoop::quit);
        eventloop.exec();

        if(reply->isFinished())
        {
            QByteArray responseByte = reply->readAll();
            qDebug()<<"the response of MES is:" << QString(responseByte);
            if (reply->error() == QNetworkReply::NoError){
                ;
            }
            else
            {
                QLOG_INFO()<<"the error of MES is:"<<reply->errorString();
            }
        }
        else {
            QLOG_WARN()<<"the response of MES is TIMEOUT!";
        }
        QThread::msleep(100);
    }

    return a.exec();
}
