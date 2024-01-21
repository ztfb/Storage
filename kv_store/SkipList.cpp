#include "SkipList.h"
#include <ctime>

SkipList::SkipList(int maxLevel) {
    this->maxLevel = maxLevel;
    this->currLevel = 0;
    this->count = 0;
    // 创建头结点（头结点不存储数据，只存储索引）
    this->header = new Node(0, "", maxLevel);
}

SkipList::~SkipList() {
    // 关闭文件
    if (this->writer.is_open()) {
        this->writer.close();
    }
    if (this->reader.is_open()) {
        this->reader.close();
    }
    //删除跳表节点
    Node* curr = this->header->forward[0];
    while(curr) {
        Node* temp = curr->forward[0];
        delete curr;
        curr = temp;
    }
    delete this->header;
}

void SkipList::dump(const std::string &fileName) {
    this->writer.open(fileName, std::ios::out);
    Node* curr = this->header->forward[0]; // 第一个数据结点
    while (curr) {
        this->writer << curr->getKey() << ":" << curr->getValue() << "\n";
        curr = curr->forward[0];
    }
    // 将数据从缓冲区刷入磁盘
    this->writer.flush();
    this->writer.close();
}

void SkipList::load(const std::string &fileName) {
    this->reader.open(fileName, std::ios::in);
    if(this->reader.is_open()) {
        std::string line;
        std::string delimiter = ":"; // k-v分隔符
        while (getline(this->reader, line)) {
            // 检查line是否有效
            if(!line.empty()&&line.find(delimiter)!=std::string::npos) {
                std::string key = line.substr(0, line.find(delimiter));
                std::string value = line.substr(line.find(delimiter)+1, line.length());
                if (key.empty()||value.empty()) {
                    continue;
                }
                insertElement(stoi(key), value);
            }
        }
        this->reader.close();
    }
}

int SkipList::getRandomLevel(){
    int k = 0;
    srand(time(nullptr));
    while (rand() % 2) {
        k++;
    }
    k = (k < this->maxLevel) ? k : this->maxLevel;
    return k;
}

int SkipList::insertElement(int key, const std::string value) {
    std::unique_lock<std::mutex> lock(this->mutex);
    Node* curr = this->header;
    // update中存放forward应该被更新的结点
    Node* update[this->maxLevel+1];
    memset(update, 0, sizeof(Node*)*(this->maxLevel+1));
    // 从跳表最高层开始找
    for(int i = this->currLevel; i >= 0; i--) {
        while(curr->forward[i] && curr->forward[i]->getKey() < key) {
            curr = curr->forward[i];
        }
        update[i] = curr;
    }

    curr = curr->forward[0];
    // 如果key存在，则更新value
    if (curr && curr->getKey() == key) {
        curr->setValue(value);
        return 1;
    }

    if (curr == NULL || curr->getKey() != key ) {
        // 随机生成结点的插入层
        int randomLevel = this->getRandomLevel();
        // 如果randomLevel>跳表当前层，则update数组中这些层应该填上head
        if (randomLevel > this->currLevel) {
            for (int i = this->currLevel+1; i < randomLevel+1; i++) {
                update[i] = this->header;
            }
            this->currLevel = randomLevel;
        }
        // 插入结点
        auto node = new Node(key, value, randomLevel);
        for (int i = 0; i <= randomLevel; i++) {
            node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = node;
        }
        this->count++;
    }
    return 0;
}

int SkipList::deleteElement(int key) {
    std::unique_lock<std::mutex> lock(this->mutex);
    Node* curr = this->header;
    Node* update[this->maxLevel+1];
    memset(update, 0, sizeof(Node*)*(this->maxLevel+1));
    // 从跳表最高层开始找
    for (int i = this->currLevel; i >= 0; i--) {
        while (curr->forward[i] && curr->forward[i]->getKey() < key) {
            curr = curr->forward[i];
        }
        update[i] = curr;
    }

    curr = curr->forward[0];
    if (curr && curr->getKey() == key) {
        // 从最低层开始删除当前结点
        for (int i = 0; i <= this->currLevel; i++) {
            // 如果curr结点没在该层，说明已经删完，退出循环
            if (update[i]->forward[i] != curr)break;
            // 删除curr
            update[i]->forward[i] = curr->forward[i];
        }
        // 移除没有元素的层
        while (this->currLevel > 0 && this->header->forward[this->currLevel] == NULL) {
            this->currLevel--;
        }
        // delete curr;
        this->count--;
        return 0;
    }else return 1; // key不存在
}

std::pair<std::string,bool> SkipList::searchElement(int key) {
    std::unique_lock<std::mutex> lock(this->mutex);
    Node* curr = this->header;
    // 从跳表最高层开始找
    for (int i = this->currLevel; i >= 0; i--) {
        while (curr->forward[i] && curr->forward[i]->getKey() < key) {
            curr = curr->forward[i];
        }
    }
    // 到达第0层，还需要再向前看一个
    curr = curr->forward[0];
    // 如果当前结点的key等于目标key，则找到
    if (curr&&curr->getKey()==key) {
        return std::pair<std::string,bool>(curr->getValue(), true);
    }else { // 没有找到目标key
        return std::pair<std::string,bool>("", false);
    }
}

std::vector<std::pair<int,std::string>> SkipList::searchAll() {
    std::unique_lock<std::mutex> lock(this->mutex);
    std::vector<std::pair<int,std::string>> result;
    Node* curr = this->header->forward[0];
    while(curr) {
        result.push_back({curr->getKey(), curr->getValue()});
        curr = curr->forward[0];
    }
    return result;
}