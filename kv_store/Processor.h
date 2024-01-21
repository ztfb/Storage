#ifndef PROCESSOR
#define PROCESSOR

#include <memory>
#include <string>

class Processor{
public:
    static std::shared_ptr<Processor> instance(); // 获取Processor的单例对象

    void init(); // 初始化方法
    std::string process(std::string& method, std::string& url, std::string& body); // 处理http请求并返回结果
    ~Processor(); // 析构函数
    
    Processor(const Processor&) = delete; // 禁用拷贝构造函数
    Processor& operator=(const Processor&) = delete; // 禁用赋值运算符
private:
    Processor() = default; // 禁用外部构造
};

#endif