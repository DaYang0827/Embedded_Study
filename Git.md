# 1 Git
## 1.1 Git 仓库是什么

原来的 Obsidian 文件夹，比如：

```text
Daily_life/
Knowledge/
```

本来只是普通文件夹。执行`git init`以后，Git 会在里面生成一个隐藏目录`.git/` 于是这个文件夹就变成：

```text
普通文件夹 + .git
= Git repository
```

也就是说$\boxed{\text{Git 仓库的核心标志就是 `.git` 目录}}$ 所以可以理解成：

```text
Obsidian Vault
├─ .obsidian/
├─ 笔记.md
├─ 图片
└─ .git/
```

同一个文件夹既可以是`\text{Obsidian Vault}` 也是`\text{Git Repository}` 这就是为什么不需要重新创建 Obsidian Vault。

## 1.2 Git 分区

Git 可以粗略理解成三个区域：

```text
工作区
Working Directory
      ↓ git add
暂存区
Staging Area
      ↓ git commit
本地仓库
Local Repository
      ↓ git push
远程仓库
GitHub
```

你可以记成$\boxed{ 工作区 \rightarrow 暂存区 \rightarrow 本地仓库 \rightarrow GitHub }$

---

## 1.3 Git 模型

不要把 Git 理解成“云盘同步”。应该理解成：

```text
文件发生修改
      ↓
Git发现变化
      ↓
add：我要记录这些变化
      ↓
commit：保存成本地版本
      ↓
push：把版本上传GitHub
      ↓
另一台设备pull
      ↓
获得这个版本
```

最核心的一句话是$\boxed{\text{Git 管的是“版本历史”，GitHub 保存的是“远程版本历史”。}}$

## 1.4 Local Repository 和 Remote Repository

电脑上的`C:\Users\...\Obsidian Vault\Knowledge`属于$\boxed{\text{Local Repository}}$ 而 GitHub 上的`github.com/DaYang0827/Knowledge` 属于`\boxed{\text{Remote Repository}}`

Git 的核心就是在这两者之间同步：

```
Local
   ↓ push
GitHub
   ↓ pull
Local
```

## 1.5 `origin` 

执行`git remote add origin https://github.com/DaYang0827/Knowledge.git` 这条命令的意思不是“上传”。而是告诉 Git**以后我把这个 GitHub 仓库称为 `origin`。** 所以`origin` 只是一个远程仓库别名。完整关系：

```text
origin
    ↓
https://github.com/DaYang0827/Knowledge.git
```

查看可以用`git remote -v`,例如：

```c
origin  https://github.com/.../Knowledge.git (fetch)
origin  https://github.com/.../Knowledge.git (push)
```

所以$\boxed{\texttt{origin} = 远程仓库的默认名字}$ 它并不是 GitHub 专属关键字，甚至可以写`git remote add github ...` 只是大家约定俗成使用 `origin`。

---

## 1.6 branch：main 

一开始看到`(master)` 后来执行`git branch -M main` 于是变成`(main)`

Branch 就是$\boxed{\text{代码/文件历史的一条开发线}}$ 比如：

```
main
│
├─ commit A
├─ commit B
├─ commit C
```

现在只有一个主要分支`main` 所以 Obsidian 笔记都是在$\boxed{\text{main branch}}$ 上同步。`git branch -M main` 就是把当前 branch 强制重命名为 `main`。

---

## 1.7 `Untracked files`

执行`git status`可以看到：

```text
Untracked files:
    .obsidian/
    xxx.md
```

意思就是**Git 看到了这些文件，但是目前还没有开始跟踪**。例如：

```
Knowledge/
├─ abc.md      ← Git知道它存在
└─ picture.png ← 但还没追踪
```

这时候`git add .`才会让 Git 开始跟踪它们。所以$\boxed{\text{Untracked = 文件存在，但还没加入 Git 管理}}$

## 1.8 `non-fast-forward`

 push vault 时出现：

```
[rejected] main -> main (non-fast-forward)
```

情况是：

```
本地：
A → B

GitHub：
A → C
```

也就是$\boxed{\text远程有本地没有的提交}$ Git 不允许直接B 覆盖 C 因为这可能会把别人/远程已有内容删掉。所以 Git 告诉`先 pull`。也就是：

```
A → B
 \
  C
```

先把 B 和 C 合并：

```
A → B
     \
      Merge
     /
A → C
```

最后再 push。所以$\boxed{\text{non-fast-forward = 远程历史领先/分叉了}}$ 解决思路通常是`git pull` 然后再`git push`

## 1.9 `--allow-unrelated-histories`

出现

```
git pull origin main --allow-unrelated-histories
```

这是因为：

你本地：

```
git init
→ commit A
```

GitHub：

```
创建仓库
→ README commit B
```

这两个仓库各自独立出生。

Git 看起来像：

```
A

B
```

没有共同祖先。

所以默认不愿意合并。

加：

```
--allow-unrelated-histories
```

相当于告诉 Git：

> 我知道这两个历史原本没有关系，但我就是要把它们合起来。

于是：

```
A \
   Merge
B /
```

一般只有**第一次把两个独立仓库接起来**时会用到。以后正常同步通常不需要。

## 1.10 Merge 

 pull 时进入 Vim，看到：

```
Merge branch 'main' ...
```

说明 Git 正在创建：$\boxed{\text{Merge Commit}}$ Merge 就是**把两个不同历史线合并成一条**。

例如：

```
Local:
A → B

Remote:
A → C
```

merge 后：

```
    B
   / \
A     M
   \ /
    C
```

M 就是 merge commit。

## 1.11 Vim 

Git 需要输入：

```
Merge commit message
```

于是默认调用了 Vim。看到：`-- INSERT --` 说明处于输入模式。Vim 最基础记忆`Esc` 退出 INSERT 模式。

然后`:wq= write + quit`。也就是$\boxed{\text{保存并退出}}$

如果不要保存`:q!`

所以以后碰到 Vim的操作为：

```
Esc
:wq
Enter
```


# 2 指令
## `-`和`--`

`-` 和 `--`，本质上是在命令行里表示**选项（option / flag）** 的两种常见写法。最简单记$\boxed{- \text{ 通常表示短选项}} \]\[ \boxed{-- \text{ 通常表示长选项}
$
比如：

```
git branch -M main
```

这里：

```
-M
```

就是短选项。

而：

```
git rm --cached file.txt
```

这里：

```
--cached
```

就是长选项。

它们本质上都是在告诉程序：

> “除了执行这个命令之外，我还要你用某种特殊方式执行。”

例如：

```
git rm file.txt
```

默认是：

> 删除本地文件 + 从 Git 跟踪中移除。

但：

```
git rm --cached file.txt
```

加了：

```
--cached
```

以后就变成：

> 只从 Git index 里移除，本地文件保留。

也就是说：

```
git rm
```

是主命令，

```
--cached
```

是在修改这个命令的行为。

再看你之前用过的：

```
git push -u origin main
```

这里：

```
-u
```

是一个短选项，作用是设置 upstream。

完整一点其实相当于：

```
git push --set-upstream origin main
```

也就是说：

```
-u
```

和：

```
--set-upstream
```

表达的是同一个意思。

所以很多命令都有：

```
短写法
-long?
```

准确说是：

```
短写法：-u
长写法：--set-upstream
```

短写法方便敲，长写法方便读。

比如常见的：

```
git commit -m "message"
```

这里：

```
-m
```

就是：

```
message
```

选项。

再比如：

```
git rm -r folder/
```

这里：

```
-r
```

就是 recursive，递归。

长写法有些程序会写成：

```
--recursive
```

不过 Git 某些子命令不一定每个短参数都有完全对应的长参数，具体要看命令帮助。

还有一个你以后会经常看到的点：多个短选项有时可以合并。

比如在很多 Unix 命令里：

```
ls -l -a
```

可以写成：

```
ls -la
```

也就是：

```
-l
-a
```

合并成：

```
-la
```

但长选项一般不能这么合并：

```
--cached
--force
```

通常要分开写。

你可以这样理解：

```
-   → 单字母 shorthand
--  → 完整单词 option
```

比如：

```
-u
-m
-r
-f
```

都是短选项。

```
--cached
--force
--help
--version
```

都是长选项。

还有一个很重要的特殊情况：

```
--
```

单独两个横线，有时不是“长选项”，而是表示：

> 后面的东西不再当参数选项解析，而是当普通文件名/位置参数处理。

例如：

```
rm -- -test.txt
```

如果文件名真的叫：

```
-test.txt
```

那直接：

```
rm -test.txt
```

程序可能以为 `-t -e -s...` 是选项。

所以：

```
rm -- -test.txt
```

中的：

```
--
```

意思是：

> 选项到这里结束，后面的 `-test.txt` 是文件名。

这个在 Linux/Git Bash 里很常见。

所以你最后可以整理成：

```
-command
```

准确说不是这样，应该是：

```
-command?
```

不对，应该记：

```
- x      短选项，例如 -m -u -r
--word   长选项，例如 --cached --force
--       结束选项解析
```

对应到你现在学的 Git：

```
git commit -m "message"
```

`-m` = 短选项。

```
git push -u origin main
```

`-u` = 短选项。

```
git rm --cached file.txt
```

`--cached` = 长选项。

```
git pull origin main --allow-unrelated-histories
```

`--allow-unrelated-histories` = 长选项。

所以最核心一句：

\[ \boxed{\text{`-` 多用于短选项，`--` 多用于长选项；它们都在改变命令的执行方式。}} \]


## 2.1 `git status`

最开始执行`git status` 出现`fatal: not a git repository` 说明$\boxed{\text{当前目录还没有 `.git`}}$ 执行 `git init` 以后，再运行`git status` 就可以看到`On branch main` 以及哪些文件：

- modified
- untracked
- staged

所以$\boxed{\texttt{git status} = 查看当前 Git 仓库状态}$ 这是以后排查 Git 问题最常用的命令之一。

## 2.2 `git add .`

执行：

```text
git add .
```

意思是**把当前目录所有新增/修改内容加入暂存区。** `.` 代表**当前目录**所以`git add .` 可以理解成$\boxed{\text{把当前所有变化准备好，等待 commit}}$

**注意`git add .`并没有上传 GitHub**。只是：

```
Working Directory
      ↓
Staging Area
```

`git add` 后，文件进入暂存区并开始被 Git 纳入版本控制流程；commit 后才正式进入版本历史。

## 2.3 `git commit`

```
git commit -m "Initial Knowledge sync"
```

意思是**把暂存区当前状态保存成一个 Git 版本**。这一步很重要。Commit 可以理解成$\boxed{\text{给当前文件状态拍一张快照}}$ 比如：

```
commit A
初始笔记

commit B
加入 DMA 笔记

commit C
修改 Bootloader 内容
```

以后你可以回到任意历史版本。所以$\boxed{\text{Git 的核心其实不是同步，而是版本管理}}$ GitHub 只是把这些 commit 放到了云端。

`commit` 更好、更生动的翻译应该是：**“存档”**、**“快照”** 或 **“递交存证”**。在 Git 里，**`git commit` 执行的就是这个“新建存档”的动作**。它把当前代码所有文件的状态死死地冻结这一刻，生成一个专属的“读档哈希值”。以后哪怕你把项目删光了，只要通过这个“存档”，你就能瞬间完成“时光倒流（读档）”。

## 2.4 `git push`

执行：

```
git push -u origin main
```

拆开看`git push` = 上传本地 commit。`origin` = 上传到哪个远程仓库。 `main` = 上传哪个 branch。所以`git push origin main` 意思就是$\boxed{\text{把本地 main 分支上传到 origin}}$

而`-u` 作用是建立 upstream。以后 Git 就知道：

```text
本地 main
↔
origin/main
```

所以后面就可以直接`git push` 不需要每次写`git push origin main`

## 2.5 `git pull`

执行：

```
git pull origin main
```

意思就是$\boxed{\text{把 GitHub 上 origin/main 的修改拉下来}}$ 本质上是：

```text
GitHub
  ↓
Local
```

所以：

```
push = 上传
pull = 下载 + 合并
```

可以这样记：

```
Local  --push--> Remote
Local <--pull--- Remote
```

---

## 2.6 核心命令

手动同步其实只需要：

```
git status
```

查看状态。

```
git add .
```

把修改放到暂存区。

```
git commit -m "update notes"
```

保存一个版本。

```
git pull
git push
```

同步 GitHub。

所以核心其实就是$\boxed{ status \rightarrow add \rightarrow commit \rightarrow pull \rightarrow push }$

## 2.7 git rm

最核心先记住$\boxed{\texttt{git rm} = 从 Git 跟踪中删除文件，并且默认也删除本地文件}$ 而$\boxed{\texttt{git rm --cached} = 只让 Git 停止跟踪，本地文件保留}$ 这两个区别非常关键。

### 2.7.1 `rm` 和 `git rm`

先区分系统命令：

```
rm file.txt
```

这是 Linux / Git Bash 的文件删除命令。它**只是把本地文件删掉**。删完以后 Git 会发现

```
deleted: file.txt
```

然后你还需要：

```
git add .
git commit -m "delete file"
```

才能把“删除”记录到 Git 历史。

而：

```
git rm file.txt
```

相当于同时完成：

```
删除本地文件
+
把删除操作加入暂存区
```

所以它本质上可以理解成：

```
rm file.txt
+
git add file.txt
```

只是 Git 帮你一步做完。

### 2.7.2 `git rm file`

比如：

```
git rm test.md
```

执行后：

```
本地 test.md 被删除
↓
Git staging area 记录：
test.md 将被删除
```

这时：

```
git status
```

可能看到：

```
Changes to be committed:
    deleted: test.md
```

然后：

```
git commit -m "Remove test note"
```

再：

```
git push
```

远程仓库里的 `test.md` 也会被删掉。

所以流程是：

```
git rm
↓
staged deletion
↓
git commit
↓
git push
```

### 2.7.3 `git rm --cached`

比如：

```
git rm --cached .obsidian/workspace.json
```

意思是**Git 不再管理这个文件，但 Windows 本地继续保留**。所以:

```text
本地文件
✅ 保留

Git index
❌ 删除

GitHub 后续
❌ 不再继续跟踪
```

特别适合这些文件：

```
workspace.json
API key
本地配置
IDE 配置
缓存文件
编译产物
```

通常会配合 `.gitignore` 一起使用：

```
.obsidian/workspace.json
```

然后：

```
git rm --cached .obsidian/workspace.json
git commit -m "Stop tracking workspace"
git push
```

这时候以后本地 `workspace.json` 再变化，Git 就不管了。

### 2.7.4 总结

```
Working Directory
↓
Staging Area
↓
Local Repository
```

那么普通：

```
git rm file.txt
```

会同时影响：

```
Working Directory
和
Staging Area
```

而：

```
git rm --cached file.txt
```

只影响：

```
Staging Area / Index
```

工作区文件保留。


```text
我要本地和 GitHub 都删
→ git rm file

我要本地保留，但 Git 不再管
→ git rm --cached file

我要停止跟踪整个目录
→ git rm -r --cached folder/

我要连目录一起删
→ git rm -r folder/
```


## 2.8 gitignore

`.gitignore` 本质上就是一个 **“告诉 Git 哪些文件不要纳入版本管理”** 的规则文件。最核心的理解是：

```text
工作目录里有文件
↓
Git 默认都能看到
↓
.gitignore 告诉 Git：
这些文件别管
```

但这里有一个关键点$\boxed{\text{`.gitignore` 只对“还没被 Git 跟踪”的文件直接生效}}$ 如果某个文件以前已经 `git add` + `commit` 过了，单纯把它写进 `.gitignore`，Git 还是会继续跟踪它。

### 2.8.1 最常见的 `.gitignore` 写法

忽略一个具体文件：

```
secret.txt
```

忽略一个文件夹：

```
build/
```

忽略某一类后缀：

```
*.log
```

表示所有 `.log` 文件都忽略。

比如：

```
debug.log
error.log
app.log
```

都会被忽略。

### 2.8.2 符号含义

#### 2.8.2.1 `*` 

`*` 是**通配符**。

比如：

```
*.tmp
```

表示忽略所有：

```
xxx.tmp
abc.tmp
test.tmp
```

再比如：

```
temp*
```

会匹配：

```
temp
temp123
temporary
```

#### 2.8.2.2 `?` 

`?` 匹配一个字符。

例如：

```
file?.txt
```

会匹配：

```
file1.txt
fileA.txt
```

但不会匹配：

```
file10.txt
```

因为 `?` 只代表一个字符。

#### 2.8.2.3 `/` 

这个很重要。如果写`build/` 通常表示仓库里名为 `build` 的目录都可能被匹配。

如果写：

```
/build/
```

表示只忽略**仓库根目录下的 build**。

例如：

```
repo/
├─ build/          ← 忽略
└─ project/
   └─ build/       ← 不匹配 /build/
```

所以前面的 `/` 表示$\boxed{\text{从仓库根目录开始匹配}}$ 

#### 2.8.2.4 `!` 

`!` 表示**前面虽然忽略了，但这个文件我要保留**。

例如：

```
*.log
!important.log
```

意思：

```
debug.log       忽略
error.log       忽略
important.log   不忽略
```

#### 2.8.2.5 `#`

以 `#` 开头：

```
# Ignore log files
*.log
```

这一行只是说明，不会生效。

