# YMRPG — UE 5.6 网络 RPG 个人 Demo

基于 **UE 5.6 Gameplay Ability System（GAS）** 的网络 RPG 游戏个人项目，涵盖连招战斗、技能、道具、背包、装备等完整玩法系统，支持服务器权威的网络同步。

技术栈：**GAS + UMG + UE 网络架构（属性同步/RPC） + UE C++**，使用 Claude Code + DeepSeek-v4 辅助开发，**基于对架构的设计与掌握指导规范代码生成**。

---

## 项目文件目录

```
study/
├── README.md                          # 本文件
├── CLAUDE.md                          # 项目指令（AI 协作规则）
├── .claude/                           # Claude Code 本地配置
├── learning_log/                      # 学习日志（按月归档）
│   ├── LEARNING_LOG_MAY.md            # 5月日志：GAS 基础、AttributeSet、GE、UI架构
│   └── LEARNING_LOG_JUNE.md           # 6月日志：物品系统、RPC 源码分析、背包网络架构
│
└── YMRPG/                             # UE 5.6 项目根目录
    ├── YMRPG.uproject                 # 项目描述文件
    ├── Config/                        # 项目配置（Engine/Game/Input/GameplayTags）
    ├── Content/                       # 蓝图、材质、UI 资源、动画等资产
    │
    └── Source/                        # C++ 源码
        ├── YMRPG.Target.cs            # Editor 目标
        ├── YMRPGServer.Target.cs      # 专用服务器目标
        └── YMRPG/                     # 游戏模块
            ├── YMRPG.Build.cs         # 模块依赖声明
            │
            ├── AbilitySystem/         # GAS 核心
            │   ├── YMRPGAbilitySystemComponent    # 自定义 ASC
            │   ├── Abilities/          # GA 技能（普攻、死亡、跳跃）
            │   ├── Attributes/         # AttributeSet（属性集定义）
            │   └── Executions/         # GE Execution（伤害计算公式）
            │
            ├── Character/             # 角色类（Base / 玩家角色）
            ├── Player/                # PlayerController / PlayerState
            ├── GameMode/              # GameMode / GameState
            │
            ├── Component/             # 游戏逻辑组件
            │   ├── YMRPGHealthComponent       # 生命值管理
            │   ├── YMRPGComboComponent        # 连招系统
            │   ├── YMRPGInventoryComponent    # 背包系统（属性复制 + RPC 双通道）
            │   └── YMRPGEquipmentComponent    # 装备系统
            │
            ├── Item/                  # 物品体系（UPrimaryDataAsset 继承链）
            │   ├── YMRPGItem          # 物品基类
            │   ├── YMRPGPotion        # 药水（授予 GA）
            │   ├── YMRPGEquipment     # 装备（挂 GE）
            │   └── YMRPGToken         # 代币/任务道具
            │
            ├── UI/                    # UMG 界面层
            │   ├── YMRPGHUD           # HUD 管理
            │   ├── Inventory/         # 背包面板 / 格子
            │   ├── Equipment/         # 装备面板 / 格子
            │   ├── Core/              # UI 基类 / Slot 基类
            │   ├── Common/            # 拖拽图标 / 信息描述
            │   ├── Damage/            # 伤害数字
            │   └── CharacterInfo/     # 角色信息面板
            │
            ├── DamagedActor/          # 碰撞伤害体系（HitBox + ApplyGE）
            ├── FeedBack/              # 伤害数字弹出系统（NumberPop）
            ├── Notify/                # AnimNotify（攻击、重置连招、施加 GE）
            ├── NotifyState/           # AnimNotifyState（受力、禁用输入、停止转向）
            ├── System/                # AssetManager 资产管理系统
            │
            └── Variant_*/             # 三种玩法变体（模板自带，非学习重点）
                ├── Variant_Combat/    # 战斗变体（AI / 连招 / StateTree）
                ├── Variant_Platforming/   # 平台跳跃变体（Dash）
                └── Variant_SideScrolling/ # 横版过关变体
```

---

## 项目概述

这是一个基于 GAS 的 UE 5.6 网络 RPG 单人 Demo，目标是构建一个具备完整游戏玩法系统的多人在线角色扮演游戏原型。

**核心设计理念**：通过继承、Component、GameplayAbility、GameplayEffect 等 GAS 机制实现高效的代码复用——装备（挂 GE）、药水（授 GA）、战斗技能（GA + ComboComponent）共享同一套属性和网络同步基础设施。

### 网络架构

**服务器权威模型**：角色的生命周期、技能激活/释放、伤害判定、背包内容的增删改均由服务器裁决，客户端仅做预测与状态接收。

| 机制 | 同步通道 | 适用场景 |
|---|---|---|
| **属性复制** | `FRepLayout::ReplicateProperties`（增量序列化） | 血量、等级、Tag 容器 |
| **RPC（Server/Client）** | `ProcessRemoteFunction`（事件驱动） | 激活技能、使用物品 |
| **Component 子对象复制** | 子对象复制管线（同属性复制） | 背包内容、装备槽 |
| **FFastArraySerializer** | NetDeltaSerialize（增量） | GE 容器同步 |

### AIGC 辅助开发

使用 Claude Code + DeepSeek-v4 辅助学习与开发：
- **源码蒸馏**：自行分析引擎 GAS 源码与 Lyra 示例，编写为 Claude Code Skills 规范代码生成
- **知识沉淀**：每日学习日志按月归档，记录技术提问与源码级知识点
- **代码生成**：基于对 GAS 架构的理解指挥 AI 生成高质量 C++ 代码，人做架构设计与审查

---

## 完成进度

- [x] 项目搭建：GAS 类继承体系（Character / GameMode / GameState / PlayerState / HUD / ASC / PlayerController）
- [x] 三种玩法变体基础代码（Combat / Platforming / SideScrolling）
- [x] GAS 基础：GameplayTag 体系、GA 生命周期、GE 机制（Duration / Components / Execution）、AttributeSet
- [x] 连招系统：ComboComponent + AnimNotify 协同
- [x] 伤害计算管线：DamageExecution 攻防公式 + HitBox 碰撞体系
- [x] 技能系统：冲刺、大招、生命/魔法回复
- [x] 伤害数字弹出系统（NumberPop）
- [x] 物品系统骨架：YMRPGItem 继承体系 + AssetManager
- [x] 背包系统 UI + 网络同步：InventoryComponent 属性复制 + Server/Client RPC 双通道
- [ ] 装备系统完善
- [ ] AI 行为系统（StateTree 深入学习）
- [ ] DSLinux 打包与专用服务器部署

---

## 环境

- **引擎**：UE 5.6
- **IDE**：Visual Studio 2022
- **版本控制**：Git + LFS
- **OS**：Windows 11
- **语言**：C++ / 蓝图混合
