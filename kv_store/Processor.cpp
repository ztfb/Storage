#include "Processor.h"
#include "SkipList.h"
#include <memory>
#include <mutex>
#include <iostream>
#include <string>

static std::shared_ptr<Processor> processor=nullptr;
static std::mutex mutex;
static SkipList* skipList;

std::shared_ptr<Processor> Processor::instance(){
    // 懒汉模式
    // 使用双重检查保证线程安全
    if(processor==nullptr){
        std::unique_lock<std::mutex> lock(mutex); // 访问临界区之前需要加锁
        if(processor==nullptr){
            processor=std::shared_ptr<Processor>(new Processor());
        }
    }
    return processor;
}

void Processor::init() {
    skipList = new SkipList(6);
    skipList->load("dump_file");
}

std::string Processor::process(std::string& method, std::string& url, std::string& body) {
    if(method!="POST" || url!="/kv_store") return "404";
    else {
        // 解析json格式的命令：{"cmd": "xxx"}，并得到命令序列
        int pos = body.find(':');
        std::vector<std::string> tokens;
        std::string token;
        bool start=false;
        for(int i=pos+1;i<body.size();i++){
            if(body[i]=='\"') {
                if(!start){
                    // 遇到开始的"
                    start=true;
                }else{
                    // 遇到结束的"
                    tokens.push_back(token);
                    break;
                }
            }else if(body[i]==' '){
                if(start){
                    tokens.push_back(token);
                    token="";
                }// 如果暂时未遇到第一个"，则忽略空格符
            }else{
                if(start){
                    token.push_back(body[i]);
                }
            }
        }
        // 如果解析出的tokens错误，则返回空字符，表示服务器内部错误
        if(tokens.empty()) return "";
        else{
            if(tokens[0]=="insert") {
                if(tokens.size()!=3)return "";
                else{
                    try{
                        std::string result = (skipList->insertElement(std::stoi(tokens[1]), tokens[2])?"update value":"success");
                        return "{\"result\": \"" + result + "\"}";
                    }catch(std::exception e){
                        return "";
                    }
                }
            }else if(tokens[0]=="delete") {
                if(tokens.size()!=2)return "";
                else{
                    try{
                        std::string result = (skipList->deleteElement(std::stoi(tokens[1]))?"no key":"success");
                        return "{\"result\": \"" + result + "\"}";
                    }catch(std::exception e){
                        return "";
                    }
                }
            }else if(tokens[0]=="search") {
                if(tokens.size()==1){ // 全查
                    std::vector<std::pair<int,std::string>> result = skipList->searchAll();
                    std::string json = "[";
                    for(auto iter=result.begin();iter!=result.end();iter++){
                        json+="{\"k\": \"" + std::to_string(iter->first) + "\", \"v\": \"" + iter->second + "\"}, ";
                    }
                    json+="]";
                    return json;
                }else if(tokens.size()==2){
                    try{
                        std::pair<std::string, bool> value = skipList->searchElement(std::stoi(tokens[1]));
                        if(value.second){
                        return "{\"k\": \"" + tokens[1] + "\", \"v\": \"" + value.first + "\"}";
                        }else return "{}";
                    }catch(std::exception e){
                        return "";
                    }
                }else return "";
            }else if(tokens[0]=="size") {
                if(tokens.size()!=1)return "";
                else {
                    return "{\"size\": \"" + std::to_string(skipList->size()) + "\"}";
                }
            }else if(tokens[0]=="dump") {
                if(tokens.size()!=1)return "";
                else {
                    skipList->dump("dump_file");
                    return "{\"result\": \"success\"}";
                }
            }else return "";
        }
    }
}

Processor::~Processor() {
    delete skipList;
}
