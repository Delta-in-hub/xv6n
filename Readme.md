# XV6N
> https://jihulab.com/xv6n/xv6n


## 实验项目

共包含8个大的实验项目，每个大的实验项目中，又包含一系列循序渐进的子实验项目，具体如下。

```

Labs 名称
- lab1
- lab2
	- lab2-1-hello
	- lab2-2-find
	- lab2-3-getppid
	- lab2-4-getsched
- lab3
	- lab3-1-pthread
	- lab3-2-sem
	- lab3-3-sem_impl
- lab4
	- lab4-1-sched
	- lab4-2-mlfq
- lab5
	- lab5-1-exception
	- lab5-2-minigdb
- lab6
	- lab6-1-bestfit
	- lab6-2-buddy
	- lab6-3-umalloc
- lab7
	- lab7-1-lazy
	- lab7-2-cow
- lab8
	- lab8-1-symlink
	- lab8-2-largefile
	- lab8-3-mmap


```


## 实验项目用法

以实验 lab2-1-hello 为例，说明如何完成实验。



1. 克隆实验代码 ，`git clone https://jihulab.com/xv6n/xv6n.git`
2. 切换到实验分支 ，`git switch lab2-1-hello`
3. 按照要求，自行完成实验实现代码。
4. 登陆Web实验项目管理系统，登陆自己的账号，在主页的右侧->个人信息->获取TOKEN（若是第一次获取，点击刷新TOKEN；TOKEN是你个人身份认证的标识，所有实验通用，注意保存）。
5. 配置`conf`文件夹下的`lab.mk`文件，示例如下

```
LAB = lab2-1-hello
TOKEN = 20190000-xxxxxxxxxxxxxxxx
```

6. 进行本地自测，命令行中执行`make test`，仅在本地执行测试，不会上传成绩。
7. 或者评测并提交成绩，命令行中执行`make handin`，执行评测后会上传成绩，同时也会打包代码一并上传（注意要配置好`conf/lab.mk`）。本步骤需要网络环境。
8. 保存当前工作区对代码的修改，`git add . ` 然后 `git commit -m 'your msg'`







## 后台部署



### 前端Web部署



>  https://jihulab.com/xv6n/xv6n-frontend



见以上的Repo.





### 后端服务器部署



> https://jihulab.com/xv6n/xv6n-backend



见以上的Repo.


