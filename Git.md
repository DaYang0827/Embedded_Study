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

## 1.3 Local Repository 和 Remote Repository

电脑上的`C:\Users\...\Obsidian Vault\Knowledge`属于$\boxed{\text{Local Repository}}$ 而 GitHub 上的`github.com/DaYang0827/Knowledge` 属于`\boxed{\text{Remote Repository}}`

Git 的核心就是在这两者之间同步：

```
Local
   ↓ push
GitHub
   ↓ pull
Local
```

## 1.4 `origin` 

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

## 1.5 branch：main 

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

## 1.6 `Untracked files`

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

## 1.7 Git 模型

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

# 2 指令
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

## 2.6 核心四个命令

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

---

# 3 为什么你会遇到 `non-fast-forward`

你 push Knowledge 时出现：

```
[rejected] main -> main (non-fast-forward)
```

这是这次最重要的 Git 知识点之一。

当时情况是：

```
本地：
A → B

GitHub：
A → C
```

也就是：

远程有本地没有的提交\text{远程有本地没有的提交}

Git 不允许你直接：

```
B 覆盖 C
```

因为这可能会把别人/远程已有内容删掉。

所以 Git 告诉你：

> 先 pull。

也就是：

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

最后再 push。

所以：

non-fast-forward = 远程历史领先/分叉了\boxed{\text{non-fast-forward = 远程历史领先/分叉了}}

解决思路通常是：

```
git pull
```

然后再：

```
git push
```

---

# 4 `--allow-unrelated-histories`

你执行过：

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

一般只有**第一次把两个独立仓库接起来**时会用到。

以后正常同步通常不需要。

---

# 5 Merge 是什么

你 pull 时进入 Vim，看到：

```
Merge branch 'main' ...
```

说明 Git 正在创建：

Merge Commit\boxed{\text{Merge Commit}}

Merge 就是：

> 把两个不同历史线合并成一条。

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

---

# 6 Vim 为什么突然出现

Git 需要你输入：

```
Merge commit message
```

于是默认调用了 Vim。

你当时看到：

```
-- INSERT --
```

说明处于输入模式。

Vim 最基础记忆：

```
Esc
```

退出 INSERT 模式。

然后：

```
:wq
```

= write + quit。

也就是：

保存并退出\boxed{\text{保存并退出}}

如果不要保存：

```
:q!
```

所以以后碰到 Vim：

```
Esc
:wq
Enter
```

就够用了。

---

# 7 GitHub 为什么不能直接用密码

你当时出现：

```
Password authentication is not supported
```

GitHub HTTPS Git 操作现在不能使用普通 GitHub 密码。

需要：

Personal Access Token\boxed{\text{Personal Access Token}}

或者 Credential Manager / browser authentication。

所以 PAT 本质上可以理解成：

> 专门给程序访问 GitHub 的密码。

而且可以限制：

```
Repository:
Knowledge only

Permission:
Contents → Read and write
```

这样比直接使用账户密码安全很多。

---

# 8 一个仓库可以有多个 Token

你之前还问过这个。

可以：

```
Knowledge repo
├─ Windows PAT
├─ iPad PAT
└─ iPhone PAT
```

它们不会互相冲突。

Token 只是：

认证身份\boxed{\text{认证身份}}

并不会控制 Git 历史。

所以一台设备一个 token 是很合理的做法。

---

# 9 Obsidian Git 到底帮你做了什么

你现在开了：

```
Auto commit-and-sync interval = 5
```

Obsidian Git 其实就是自动替你执行类似：

```
git add .
git commit
git pull
git push
```

所以插件并没有创造新的 Git 机制。

它只是把 Git 命令自动化了。

整个流程：

```
你修改 Markdown
       ↓
Obsidian Git
       ↓
git add
       ↓
git commit
       ↓
git pull
       ↓
git push
       ↓
GitHub
```

这也是为什么：

学会基础 Git 以后，Obsidian Git 就很好理解了\boxed{\text{学会基础 Git 以后，Obsidian Git 就很好理解了}}

---

# 10 你现在整套同步结构

你现在实际上已经搭出了：

```
Windows Obsidian
      │
      │ git push/pull
      ▼
   GitHub
      ▲
      │
      │ git push/pull
      │
iPad / iPhone Obsidian
```

也就是：

Windows↔GitHub↔iPad/iPhone\boxed{ Windows \leftrightarrow GitHub \leftrightarrow iPad/iPhone }

GitHub 相当于中间的远程仓库。

---

