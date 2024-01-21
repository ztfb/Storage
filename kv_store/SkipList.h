#ifndef SKIPLIST
#define SKIPLIST

#include <fstream>
#include <vector>
#include <mutex>
#include <cstring>

// TODO：最好使用智能指针
class Node {
public:
    Node(const int k, const std::string& v, int level) {
        this->key = k;
        this->value = v;
        // 该结点需要存储0~level共level+1层结点地址
        this->forward = new Node*[level+1];
        // 不同层下一个结点地址初始化为0（NULL）
        memset(this->forward, 0, sizeof(Node*)*(level+1));
    }
    ~Node() {
        delete []forward;
    }
    int getKey() const{
        return this->key;
    }
    std::string getValue() const{
        return this->value;
    }
    void setValue(const std::string v) {
        this->value=v;
    }
    // 不同层下一个结点地址
    Node** forward;
private:
    int key;
    std::string value;
};

class SkipList {
public:
    explicit SkipList(int maxLevel);
    ~SkipList();
    int size() const{ // 获取跳表元素数量
        return this->count;
    }

    void dump(const std::string& fileName); // 落盘
    void load(const std::string& fileName); // 加载
    int insertElement(int key, const std::string value); // 插入数据：0插入成功；1key已存在，更新value
    int deleteElement(int key); // 删除数据：0删除成功；1key不存在
    std::pair<std::string, bool> searchElement(int key); // 查询数据
    std::vector<std::pair<int, std::string>> searchAll(); // 查询所有数据
private:
    int maxLevel; // 跳表最大层数
    int currLevel; // 跳表当前层数
    Node* header; // 头结点指针
    int count; // 跳表当前元素数量

    std::ofstream writer; // 将跳表落盘
    std::ifstream reader; // 从磁盘中加载跳表
    std::mutex mutex; // 读写锁
    // 随机生成新元素所在层
    int getRandomLevel();
};

#endif
