# K-V存储引擎文档

## 实现细节

`Storage`使用的K-V存储引擎基于经典的跳表结构。跳表具体的实现细节可自行查阅资料，这里不再赘述。

## 操作演示

### 插入操作

#### 插入新key

![](../images/insert插入.png)

#### 更新数据

![](../images/insert更新.png)

### 删除操作

#### 删除存在的key

![](../images/delete存在key.png)

#### 删除不存在的key

![](../images/delete不存在key.png)

### 查询操作

#### 全查

![](../images/search全查.png)

#### 按key查

![](../images/search按key查.png)

#### 查size

![](../images/size.png)

### 落盘操作

![](../images/dump.png)

![](../images/dump_file.png)