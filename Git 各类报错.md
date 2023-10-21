# Git 各类错误处理

[Git 各类错误处理](#git-各类错误处理)\
      - [0 命令行操作仓库常见命令](#0-命令行操作仓库常见命令)\
      - [1 报错 “在签出前，请清理存储库工作树。”](#1-报错-在签出前请清理存储库工作树)\
      - [2 报错 “fatal: unable to access ‘https://github.com/…’: OpenSSL SSL\_read: Connection was reset, errno 10054”](#2-报错-fatal-unable-to-access-httpsgithubcom-openssl-ssl_read-connection-was-reset-errno-10054)\
      - [3 报错 “fatal: bad boolean config value ‘“false”’ for ‘http.sslverify’”](#3-报错-fatal-bad-boolean-config-value-false-for-httpsslverify)\
      - [4 报错 “Git:fatal:unable to access https://github.com/…':Failed to connect to github.com port 443 after 21086 ms: Timed out”](#4-报错-gitfatalunable-to-access-httpsgithubcomfailed-to-connect-to-githubcom-port-443-after-21086-ms-timed-out)\
      - [5 报错 “fatal: ‘origin’ does not appear to be a git repository”](#5-报错-fatal-origin-does-not-appear-to-be-a-git-repository)\
      - [6 报错 “fatal: Not a git repository (or any of the parent directories): .git”](#6-报错-fatal-not-a-git-repository-or-any-of-the-parent-directories-git)\
      - [7 报错 “error: failed to push some refs to ‘https://github.com/仓库名’ ”](#7-报错-error-failed-to-push-some-refs-to-httpsgithubcom仓库名-)\
      - [8 报错 “fatal: unable to access ‘https://git.liuzhen007.com/boffmpeg.git/’: The requested URL returned error: 403”](#8-报错-fatal-unable-to-access-httpsgitliuzhen007comboffmpeggit-the-requested-url-returned-error-403)\
      - [8 在vscode里修改分支名](#8-在vscode里修改分支名)\
      - [9 命令“git commit”之后弹出窗口并“hint: Waiting for your editor to close the file…”](#9-命令git-commit之后弹出窗口并hint-waiting-for-your-editor-to-close-the-file)\
      - [10 命令“git pull”之后报错“error: You have not concluded your merge (MERGE\_HEAD exists).”](#10-命令git-pull之后报错error-you-have-not-concluded-your-merge-merge_head-exists)\
      - [11 同步node代码，忽略node\_modules文件夹](#11-同步node代码忽略node_modules文件夹)\


#### 0 命令行操作仓库常见命令

**初次使用：**

1. 在github或者gilab新建一个仓库

2. 进入项目目录

3. 初始化（生成.git目录）：`git init`

4. 连接远程仓库：`git remote add origin http://github.com/仓库地址`
5. 所有文件加入暂存区：`git add .`
6. 提交到本地仓库: `git commit -m '备注'`
7. 推送到远程仓库：`git push orgin 分支名或强制推送git push orgin 分支名 --force`

**连接到仓库、同步到本地：**

1. 进入项目目录
2. 初始化（生成.git目录）：`git init`
3. 连接远程仓库：`git remote add origin http://github.com/仓库地址`
4. 从远程仓库拉取文件：`git pull origin "分支名"`

**更改部分代码后提交：**

1. 查看状态：`git status或者git status -s`
2. 所有不同的文件加入暂存区：`git add .`
3. 提交到本地仓库: `git commit -m '备注'`
4. 从远程仓库拉取文件：`git pull origin "分支名"或使用git fetch origin 分支名`
5. 推送到远程仓库：`git push orgin 分支名或强制推送git push orgin 分支名 --force`

**登录命令：**

1. 邮箱： `git config --global user.email "you@example.com"`
2. 密码：`git config --global user.name "Your Name"`

#### 1 报错 “在签出前，请清理存储库工作树。”

使用vscode提交代码，提示错误

**问题：** 仓库代码和本地代码存在冲突

**解决办法：**

1. 手动解决
   ①git stash 先将本地修改存储起来
   ②git pull 拉取远程
   ③git stash pop 还原暂存内容

2. 放弃本地修改，直接覆盖
   ①git reset --hard
   ②git pull

#### 2 报错 “fatal: unable to access ‘https://github.com/…’: OpenSSL SSL_read: Connection was reset, errno 10054”

**问题：** 一般是这是因为服务器的SSL证书没有经过第三方机构的签署，所以才报错
**解决方法：** 输入

```bash
git config --global http.sslVerify "false"
```

解除ssl验证后，再次git即可

#### 3 报错 “fatal: bad boolean config value ‘“false”’ for ‘http.sslverify’”

**问题：** 解决 “OpenSSL SSL_read: Connection was reset, errno 10054”后出现的错误，因为双引号错误。

**解决方法：** 搜索”.gitconfig“文件，配置：

```bash
[http]
	sslVerify = “false”
```


修改为

```bash
[http]
	sslVerify = false
```

#### 4 报错 “Git:fatal:unable to access https://github.com/…':Failed to connect to github.com port 443 after 21086 ms: Timed out”

**问题：** 网络问题

**解决方法：**

1. 关梯子

2. 取消全局代理：

   ```bash
   git config --global --unset http.proxy
   git config --global --unset https.proxy
   ```

#### 5 报错 “fatal: ‘origin’ does not appear to be a git repository”

用 vscode 的仓库面板第一次推送代码很容易出现问题，比如使用 `git push -u origin master` 命令的时候

```bash
$ git push -u origin master
fatal: 'origin' does not appear to be a git repository
fatal: Could not read from remote repository.
```

使用 `git remote -v` 看一下远程仓库情况：

```bash
git remote -v
master  https://github.com/Eggze2/md_pic.git (fetch)
master  https://github.com/Eggze2/md_pic.git (push)
```

正常应该是这样，前面是origin：

```bash
git remote -v
origin  https://github.com/Eggze2/md_pic.git (fetch)
origin  https://github.com/Eggze2/md_pic.git (push)
```

所以删除代码文件夹里的隐藏.git文件夹重新连接远程仓库，用代码行连接好之后，再用vscode 面板的拉取推送等一般功能

#### 6 报错 “fatal: Not a git repository (or any of the parent directories): .git”

**问题：** 提示没有.git目录

**解决方法：** 输入命令 `git init`

#### 7 报错 “error: failed to push some refs to ‘https://github.com/仓库名’ ”

**问题：** 在控制台用 `git push orgin` 分支名和 `git push orgin 分支名 --force` 推送到

远程仓库，报错：

```bash
D:/Codes/GitHub/md_pic>git push origin main

error: src refspec main does not match any 

error: failed to push some refs to 'https://github.com/Eggze2/md_pic.git'
```

**解决方法：**参考这篇文章： [Error: failed to push some refs to – How to Fix in Git (freecodecamp.org)](https://www.freecodecamp.org/news/error-failed-to-push-some-refs-to-how-to-fix-in-git/)

#### 8 报错 “fatal: unable to access ‘https://git.liuzhen007.com/boffmpeg.git/’: The requested URL returned error: 403”

**问题：**在git push的时候出现

**解决方法：**出现这个问题就是git和你账号链接有问题，主要检查两个地方：

1. 用命令git config --list，检查用户名和密码
2. 检查一下你github邮箱设置页面，是不是把邮箱设置成private了：https://github.com/settings/emails

#### 8 在vscode里修改分支名

**问题：**vscode初始化仓库分支名是master，如图位置，提交到仓库报错没有这个分支，需要把它改成main或者其他

**解决方法：**输入快捷键：CTRL + SHIFT + P，搜索 rename ，点 “Git：重命名分支”，输入“main” 回车。

#### 9 命令“git commit”之后弹出窗口并“hint: Waiting for your editor to close the file…”

使用命令 `git commit` 之后，弹出文件“COMMIT MESAGE”窗口，并显示：

```bash
hint: Waiting for your editor to close the file...
```

直接关闭窗口，显示：

```bash
Aborting commit due to empty commit message.
```

**问题：**没提交注释信息

**解决方法：**使用命令 `git commit -m '注释'` 提交

#### 10 命令“git pull”之后报错“error: You have not concluded your merge (MERGE_HEAD exists).”

**问题：** 拉取代码时，路由文件发生冲突，解决此文件冲突后，再次拉取，报错。

**解决方法：**https://blog.csdn.net/L_smwy/article/details/121400574

#### 11 同步node代码，忽略node_modules文件夹

**问题：**node.js里的组件包，文件太多太碎，怎么让git忽略这个文件夹

**解决方法：**在项目的根目录中创建.gitignore文件，然后在其中添加node_modules一行，告诉git忽略该文件夹

参考：https://blog.csdn.net/codark/article/details/108668311