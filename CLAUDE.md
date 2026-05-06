# 学习仓库 — UE 5.6 学习项目集

## 概述

这是一个个人 UE 5.6 学习仓库，包含两个跟着教程做的项目。所有项目共用同一个 git 仓库。

## 项目

### Project_YM_WibP
- **主题**：第三方库集成 — 将 libwebp 编译为 UE 插件
- **核心内容**：静态/动态 WebP 生成、读取、视口截图、蓝图暴露
- **架构**：`ThirdParty 原生库 → FYMSampleWebPLib → FYMSampleWebPCore → GameInstanceSubsystem → 蓝图`

### YMRPG
- **主题**：多玩法 RPG 游戏系统
- **三个变体**：
  - `Variant_Combat` — 第三人称战斗（AI、连招、StateTree）
  - `Variant_Platforming` — 平台跳跃（Dash 技能）
  - `Variant_SideScrolling` — 横版过关（移动平台、跳板、拾取物）
- **依赖模块**：EnhancedInput、AIModule、StateTree、GameplayStateTree、UMG

## 环境

- **OS**：Windows 11
- **引擎**：UE 5.6
- **IDE**：Visual Studio（通过 .sln 打开）
- **版本控制**：Git + LFS（大文件由 .gitattributes 管理）

## 交流

- 使用简体中文
- 在学习场景下，优先解释原理而非直接给代码
- 复杂任务先做计划再实施

## 用户

- Git 用户名：yumouWYS
- 学习路径：底层 C/C++ 集成 → UE C++ 封装 → 蓝图暴露 → 游戏玩法系统
