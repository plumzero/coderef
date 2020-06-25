
## 编译安装 curl-7.29.0
- 需求
    + 低版本 curl
    + 只需要支持基本的 http 的 GET 和 POST 请求功能
    + 不需要支持 SSL 功能
    + 不需要支持 zlib 库
    + 可能有动态库需求
- 编译过程
    + 解压 curl-7.29.0
      ```shell
        unzip curl-7.29.0.zip
      ```
    + 进入目录配置
      ```shell
        ./configure --prefix=/usr/local/libcurl7290 --without-ssl --without-zlib --enable-shared
      ```
    + 编译及安装
      ```shell
        make
        make install
      ```
## 交叉编译 curl-7.29.0
- 交叉编译环境 aarch64-fsl-linux
- 编码过程
    + 解压 curl-7.29.0
      ```shell
        unzip curl-7.29.0.zip
      ```
    + 进入目录配置，需要指定交叉编译宿主机器
      ```shell
        ./configure --prefix=/tmp/curl-aarch64 --host=aarch64-linux --without-ssl --without-zlib --enable-shared --without-libidn CFLAGS=-fPIC
      ```
    + 编译及安装
      ```shell
        make
        make install
      ```
