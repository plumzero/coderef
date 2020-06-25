
## 说明
- 进程在运行时有时可能需要产生一些临时文件，这些文件不作永久存储，而是在接下来程序运
  行的某个时间点或进程退出之后"消失"。因为是临时文件，所以对文件名的名称并不刻意要求，
  只要保证在该文件使用周期内能够找到这个文件即可。
- c/c++ 标准库提供了 tmpnam 函数用于产生一个临时文件名:
  ```c
    char* tmpnam(char* str);
  ```
  调用上述函数后，会产生类似于如下格式的文件:
  ```shell
    /tmp/filelJLngP
    /tmp/fileelCL6j
    /tmp/filegjO0yY
  ```
  可以看到，该文件在"存放在" /tmp 目录下，表示这是一个临时文件。
- 在 linux 下测试完成。
  
## 使用 tmpnam 创建临时文件
- 进程从其他地方获得一个二进制文件流，因为一些原因(如调用第三方函数需要一个文件句柄 FILE)，
  ，不得不将这个流先写入文件才能解析这个流中的内容。这时就可以调用上面的 tmpnam 函数创建一
  个临时文件写入。
- 简单示例如下:
  ```c
    /** [1] 生成临时文件名(注意，这时并未在 /tmp 目录下产生文件) */
    char tfname[256] = {0};
    tmpnam(tfname);
    printf("%s\n", tfname);
    /** [2] 写入文件。这时会在 /tmp 目录下产生一个名字为 tmpname 的磁盘文件 */
    fout = fopen(tfname, "w");
    fwrite(ibuf, 1, strlen(ibuf), fout);
    fclose(fout);
    /** [3] 从文件中读取 */
    fin = fopen(tfname, "r");
    fread(obuf, 1, sizeof(obuf), fin);
    fclose(fin);
    /** [4] 因为第 2 步的写入，文件虽然是临时的，但却固化在了磁盘中，所以需要调用 unlink 删除 */
    unlink(tfname);
  ```
  程序示例: tmpnam.cpp
- 在编译上述文件时会报警告:
  ```c
    warning: the use of `tmpnam' is dangerous, better use `mkstemp'
  ```
  可见系统并不推荐这样的做法，虽然这样做并没有什么不妥。听系统的话，用 mkstemp 实现。
  
## 使用 mkstemp 创建临时文件
- 简单示例如下:
  ```c
    /** [1] 创建临时文件(真实的磁盘文件) */
    char tfname[] = { "tmp_XXXXXX" };
    int fd = mkstemp(tfname);
    printf("%s\n", tfname);
    /** [2] [3] [4] 相同 */
  ```
  虽然传入的 tfname 是一个固定值，但 mkstemp 会对其进行一些转换，转换后的格式如下:
  ```shell
    /tmp/filexNR8vL
  ```
  可以看到，其格式和调用 tmpnam 函数产生的临时文件名是相同的。
- mkstemp 调用成功后会返回一个针对于当前用户才能使用的文件描述符。进程可以通过这个描述符调
  用 fdopen 创建一个文件句柄 FILE，也可以直接根据转换后的临时文件名 tfname 打开文件句柄，不
  过更建议前者，因为文件描述符只对当前进程用户有效，更加安全一些。
- 程序示例: mkstemp.c