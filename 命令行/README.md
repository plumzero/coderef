
## 参数
- 参数输入格式
  ```shell
    -h --help -?
    -v --version --show-version
    -d --depth --max-depth
    -f --file
  ```
- 参数保存格式
    + 参数通过映射表字符串存储，如:
      ```shell
        ./app.out --user=me --pass=123 -h
      ```
      存储为:
      ```shell
        map["--user"]="me", map["--pass"]="123", map["-h"]=true
      ```

## 主要函数
- getArg(...)
    + 没有初始化要求；
    + 不定参函数，第一个参数缺省为选项值(是否为必选项)；
- str()
    + 可读字符串打印
- size()
    + 映射表长度
