
把 **Agent / Agency、Skill、MCP** 理解成 AI 从“只会聊天”变成“能做事”的三个不同层次。它们不是同一个概念。

最先给你一个总图：

```
                    用户目标
                       │
                       ▼
                 ┌──────────┐
                 │  Agent   │
                 │  负责决策 │
                 └────┬─────┘
                      │
          ┌───────────┼───────────┐
          ▼           ▼           ▼
       Skill        Tool        MCP
     “怎么做”      “具体能力”    “怎么接外部能力”
          │           │           │
          └───────────┴───────────┘
                      │
                      ▼
              外部系统 / 数据 / 软件
```

如果用你熟悉的嵌入式来类比：

```
Agent
≈ 主控逻辑 / Scheduler + 决策层

Skill
≈ 已经封装好的功能模块
   uart_send()
   flash_write()
   crc_check()

MCP
≈ 一套标准通信协议 / 接口规范
   让不同模块按统一方式连接
```

---

# 1. Agent 是什么

`Agent` 可以翻译成：

> 智能体。

普通 LLM 往往是：

```
你问
↓
模型回答
↓
结束
```

比如：

```
你：
“解释一下STM32 DMA”

AI：
给你解释
```

这基本还是：

```
Input
↓
LLM
↓
Output
```

而 Agent 更像：

> **给它一个目标，它自己决定下一步该做什么，并且可以多次调用工具完成任务。**

例如你说：

```
“帮我检查这个STM32项目为什么编译失败，
然后找出错误并修改。”
```

Agent 可能自己做：

```
读取项目文件
↓
查看编译错误
↓
定位源码
↓
修改代码
↓
重新编译
↓
发现还有错误
↓
继续修改
↓
编译成功
↓
总结修改内容
```

这就不再是简单：

```
Prompt → Answer
```

而是：

```
Goal
↓
Think / Plan
↓
Action
↓
Observe Result
↓
再决定下一步
↓
Action
↓
...
↓
Finish
```

---

# 2. Agent 最核心的地方不是“聪明”，而是循环

Agent 的核心模型可以记成：

```
Observe
↓
Reason
↓
Act
↓
Observe
↓
Reason
↓
Act
...
```

比如：

```
目标：
“检查我的Bootloader”
```

第一次：

```
Agent:
先读取parser.c
```

发现：

```
LEN没有做范围检查
```

然后：

```
继续读取flash.c
```

又发现：

```
WRITE_APP没有检查APP_END
```

再继续：

```
查看jump.c
```

最终才给出结果。

所以 Agent 本质是：

> **LLM + 状态 + 工具 + 多步执行逻辑。**

---

# 3. Agency 是什么

你写的是 `agency`，如果你指的是 AI 里的 **agency / agentic capability**，它和 Agent 有一点区别。

`Agent`：

> 一个具体的智能体系统。

`Agency`：

> 这个系统“自主行动、做决定、推进目标”的能力程度。

比如两个 AI：

A：

```
你问一句
→ 回答一句
```

它 agency 很低。

B：

```
你给目标
→ 自己查文件
→ 自己运行测试
→ 自己发现错误
→ 自己修改
→ 自己验证
```

它的 agency 更强。

所以：

```
Agent
= 一个系统

Agency
= 这个系统有多大的自主行动能力
```

类似：

```
Task
= 一个具体任务

Priority
= 这个Task的一个属性
```

不是同一个维度。

---

# 4. 什么叫 Skill

Skill 可以理解为：

> **AI 已经封装好的一套“完成某类任务的方法”。**

比如：

```
PDF Skill
Spreadsheet Skill
Slide Skill
Coding Skill
Research Skill
```

它通常不只是一个函数。

而可能包含：

```
规则
步骤
最佳实践
工具使用方法
输入输出要求
```

例如一个：

```
PDF Skill
```

可能告诉 Agent：

```
怎么读取PDF
↓
怎么修改
↓
用什么库
↓
怎么检查分页
↓
怎么生成最终文件
```

所以 Skill 比普通 Tool 更“高级”。

---

# 5. Skill 和函数有什么区别

比如：

```
FLASH_WriteWord();
```

这是一个：

```
function
```

只负责：

```
写一个Word
```

但：

```
Firmware Update Skill
```

可能包含：

```
检查固件大小
↓
验证地址
↓
擦除Flash
↓
分包写入
↓
CRC验证
↓
更新metadata
↓
重启
```

所以你可以理解：

```
Tool
≈ 一个函数

Skill
≈ 一个模块 / 工作流 / SOP
```

这个类比对你最好理解。

---

# 6. Agent 和 Skill 什么关系

Agent 可以调用 Skill。

例如：

```
Agent
│
├── PDF Skill
├── Git Skill
├── Code Review Skill
└── Data Analysis Skill
```

用户说：

```
“分析这个项目，然后写一份报告。”
```

Agent可能：

```
Code Analysis Skill
↓
检查代码

Git Skill
↓
查看commit

Document Skill
↓
生成报告
```

所以：

> Agent 是“谁决定做什么”。

Skill 是：

> “某件事具体怎么做”。

---

# 7. MCP 是什么

MCP：

> **Model Context Protocol**

可以先翻译成：

> 模型上下文协议。

它解决的问题是：

> **AI 怎么用统一方式连接外部工具和数据源。**

比如 AI 想访问：

```
GitHub
Google Drive
Database
Filesystem
Slack
Notion
Jira
自定义服务器
```

以前每一个可能都要：

```
自己写一套接口
自己写一套格式
自己处理调用逻辑
```

MCP 想做的是：

```
统一标准
```

类似于：

```
USB
```

为什么好用？

因为：

```
鼠标
键盘
U盘
摄像头
```

都是不同设备，但只要符合 USB 标准：

```
Host
↔
USB Protocol
↔
Device
```

就能通信。

MCP类似：

```
AI Client
↔
MCP
↔
External Server
```

---

# 8. 用你熟悉的串口协议理解 MCP

你自己的 Bootloader 有：

```
AA 55 | LEN | CMD | DATA | SUM
```

为什么需要协议？

因为上位机和 STM32 必须约定：

```
包头是什么
长度在哪里
CMD在哪里
数据是什么
校验怎么做
```

否则双方根本不知道怎么沟通。

MCP解决的问题很像：

```
AI：
“我想读取文件。”

MCP Server：
“我提供read_file工具。”

AI按照标准格式调用：
read_file(path)
```

所以：

> MCP 本质不是“AI模型”。

也不是：

> 一个 Agent。

而是一套：

> **模型/Agent 和外部能力之间的标准接口协议。**

---

# 9. MCP Server 是什么

假设你写一个：

```
STM32 Debug MCP Server
```

它可以提供：

```
read_register
read_memory
read_map_file
flash_firmware
run_gdb
```

那么 Agent 看到的是类似：

```
可用工具：

read_memory(address, length)

read_register(name)

flash_firmware(file)
```

Agent 就能调用这些工具。

底层可能实际上调用：

```
OpenOCD
GDB
J-Link
Python
Serial
```

但 Agent 不需要知道全部实现。

---

# 10. MCP Client 是什么

通常：

```
AI应用 / Agent
```

是 MCP Client。

例如：

```
AI Agent
↓
MCP Client
↓
MCP Protocol
↓
MCP Server
↓
GitHub
```

MCP Server 把 GitHub 能力暴露出来：

```
read_repository
get_issue
create_issue
read_pull_request
```

Agent 就可以用。

---

# 11. MCP 里面通常提供什么

你第一遍可以知道三个概念：

```
Tools
Resources
Prompts
```

其中最重要的是 `Tools`。

比如：

```
Tools
→ 可以执行操作
```

例如：

```
read_file()
search_code()
send_email()
query_database()
```

Resources 更像：

```
可以读取的内容
```

例如：

```
文件
数据库记录
文档
日志
```

Prompts 可以理解为：

```
预定义工作模板
```

第一遍其实掌握：

```
MCP Server
↓
向AI暴露工具和数据
```

已经够用了。

---

# 12. Tool 和 MCP 又是什么关系

这里特别容易混。

Tool：

> 一个具体能力。

例如：

```
search_code()
```

MCP：

> 把这个 Tool 暴露给 AI 的标准机制之一。

所以：

```
Tool
≠ MCP
```

比如：

```
Tool：
read_file()

MCP：
告诉AI：
有这么一个工具
叫什么
参数是什么
返回什么
怎么调用
```

类似：

```
USART_SendData()
```

是功能。

而：

```
UART Protocol
```

是通信规则。

---

# 13. Skill 和 MCP 也不是一回事

比如：

```
GitHub MCP Server
```

提供：

```
get_file
search_code
create_issue
create_branch
```

这些都是工具。

然后你可能有一个：

```
Code Review Skill
```

里面规定：

```
先读README
↓
再看目录结构
↓
再找关键模块
↓
再运行测试
↓
再检查边界条件
↓
最后输出Review
```

而这些步骤中会调用：

```
GitHub MCP提供的Tool
```

所以：

```
Skill
→ 怎么完成任务

MCP
→ 怎么连接工具

Tool
→ 实际执行什么操作
```

---

# 14. Agent、Skill、MCP 全部串起来

比如你以后做：

> “自动分析 STM32 Bootloader 项目”

可以设计：

```
                    User
                     │
                     ▼
               Bootloader Agent
                     │
            “我要检查这个项目”
                     │
          ┌──────────┴──────────┐
          ▼                     ▼
 Code Review Skill        Debug Skill
          │                     │
          ▼                     ▼
        MCP                   MCP
          │                     │
     GitHub Server        GDB/J-Link Server
          │                     │
          ▼                     ▼
       Repository             STM32
```

Agent：

```
决定：
先看代码
再看MAP
再测试
```

Skill：

```
告诉Agent：
Bootloader应该检查哪些东西
```

例如：

```
Vector Table
MSP
Reset_Handler
CRC
APP boundary
Flash erase
VTOR
```

MCP：

```
帮Agent真正连上：
GitHub
GDB
J-Link
Filesystem
```

Tool：

```
read_file()
read_memory()
get_register()
```

这样你就能看到完整架构。

---

# 15. 为什么现在大家很重视 MCP

因为以前 AI 系统经常这样：

```
GitHub integration
→ 自己写

Slack integration
→ 再写一套

Database integration
→ 又写一套

Filesystem
→ 又写
```

大量重复工作。

有统一协议以后：

```
                Agent
                  │
                 MCP
           ┌──────┼──────┐
           ▼      ▼      ▼
        GitHub   DB    Filesystem
```

Agent 对这些能力的使用方式更统一。

这和嵌入式标准接口很像：

```
不同SPI Sensor
↓
遵循SPI通信规则
↓
MCU都知道怎么通信
```

虽然具体寄存器还是不同，但通信框架统一了。

---

# 16. Agent 为什么离不开 Tool

LLM 本身只能：

```
输入Token
↓
计算
↓
输出Token
```

它本身不能天然：

```
打开你的电脑文件
运行gcc
发送邮件
读取GitHub
查数据库
控制STM32
```

所以：

```
LLM
+
Tools
```

以后才有真正“做事情”的能力。

再加：

```
Memory
Planning
Loop
```

就越来越接近 Agent：

```
LLM
+
Tools
+
Memory
+
Planning
+
Execution Loop
=
Agent
```

这不是严格数学公式，只是非常好用的理解方式。

---

# 17. Agent 和普通脚本有什么区别

普通 Python：

```
read_file()compile()test()
```

流程：

```
人提前写死
```

比如：

```
Step1
Step2
Step3
Step4
```

Agent：

```
读取文件
↓
发现编译错误
↓
根据错误决定下一步
```

可能：

```
Error A
→ 查header

Error B
→ 查linker script

Error C
→ 看map
```

所以差别是：

```
传统程序
→ 路径主要由程序员提前定义

Agent
→ LLM根据当前状态动态决定下一步
```

---

# 18. 但是 Agent 也不是“无限自主”

这是你理解 AI 工程很重要的一点。

真正好的 Agent 通常会限制：

```
它能用哪些Tool
能访问什么数据
能执行哪些操作
什么时候需要用户确认
最多执行多少步
```

因为 agency 越强：

```
自动化能力越强
```

同时：

```
错误操作风险也越高
```

所以实际工程强调：

```
Permission
Sandbox
Validation
Human approval
Logging
```