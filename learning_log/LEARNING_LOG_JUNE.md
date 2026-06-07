# 学习日志

记录每日有技术价值的提问与对应知识点，按日期归档，方便检索复习。

## 目录

- [2026-06-01](#ymrpg-2026-06-01) — UPrimaryDataAsset 继承链 、 UHT Category 引号规则 、 UBT 同名冲突 、 物品系统
- [2026-06-06](#ymrpg-2026-06-06) — RPC 调用链源码分析、WithValidation、属性复制管线、ReplicatedUsing 机制、Component 子对象复制、背包系统网络架构
- [2026-06-07](#ymrpg-2026-06-07) — Blueprint Timeline、AIGC 概念与提效、Git 团队管理模式、README 文档重写

---

<details>
<summary><b>2026-06-01</b></summary>

### UPrimaryDataAsset 完整继承链

```
UObjectBase → UObjectBaseUtility → UObject → UDataAsset → UPrimaryDataAsset
```

| 层级 | 职责 | 新增 |
|---|---|---|
| `UObjectBase` | "我是谁" | ClassPrivate, NamePrivate, OuterPrivate, InternalIndex |
| `UObjectBaseUtility` | "怎么查" | GetFName/GetPathName/HasAnyFlags/IsA 等内联便利函数 |
| `UObject` | "怎么活" | Serialize/PostLoad/PreSave/CreateDefaultSubobject/GetWorld/MarkAsGarbage 生命周期 |
| `UDataAsset` | "怎么存" | NativeClass 属性 + `LoadBehavior = "LazyOnDemand"` → .uasset 文件化 + 按需加载 |
| `UPrimaryDataAsset` | "怎么被找到" | GetPrimaryAssetId() 全局标识 + AssetBundleData 资产包依赖 + PostLoad/PreSave 钩子 |

**设计原则**：每层都是单一职责的加法。UObjectBase 存身份 → UObjectBaseUtility 提供查询 API → UObject 定义生命周期 → UDataAsset 变成独立资产文件 → UPrimaryDataAsset 纳入 AssetManager 管理体系。

`GetPrimaryAssetId()` 返回值格式为 `Type:Name`（如 `"GameplayEffect:GE_Damage_Fire"`），CDO 向上遍历找第一个继承 UPrimaryDataAsset 的类名做 Type，实例用类名做 Type、对象名做 Name。

`AssetBundleData`（仅 Editor）是一个 `TArray<FAssetBundleEntry>`，每个 Entry = Bundle 名 + 资产路径列表，支持按 Bundle 分组加载依赖资产（如 `AssetManager.LoadPrimaryAsset(Id, {"Mesh"})` 只加载 Mesh bundle）。

### UHT UPROPERTY Category 引号规则——源码证据

**文件**：`EpicGames.UHT/Tokenizer/UhtToken.cs` + `Parsers/UhtSpecifierParser.cs:402`

UHT tokenizer 将 `UPROPERTY(...)` 内容按 C++ 词法切 token：

| Token 类型 | 匹配规则 | 示例 |
|---|---|---|
| `Identifier` | `[a-zA-Z_][a-zA-Z0-9_]*` | `Item`, `Combat`, `BlueprintReadOnly` |
| `StringConst` | `"..."` 双引号包裹 | `"Damage\|Execution"` |
| `Symbol` | 单个符号字符 | `=`, `,`, `\|` |

`ReadValue()` 中：`Identifier` token → 直接拼接值；`StringConst` token → `GetConstantValue()` 剥离外层引号。

**结论**：`Category = Item` 不加引号因为 `Item` 是合法 C++ 标识符。`Category = "Damage|Execution"` 必须加引号因为 `|` 是 Symbol，会切断 token 流。这不是 Category 专属行为，所有 specifier/meta 值都遵循同一套 tokenizer 词法。

### UBT 禁止同模块同名 .cpp

非 Unity 构建时，不同目录的同名 `.cpp` 生成的 `.obj` 全部放在同一个 Intermediate 目录，文件名冲突导致链接失败。UBT 在 makefile 生成阶段就检测并报 `Input filename conflicts`。

**教训**：同模块内所有 `.cpp` 文件名必须全局唯一。`.h` 无此限制（不独立编译）。`.txt` 也无此限制（不被 UBT 识别为编译单元）。

### 物品系统架构：UPrimaryDataAsset + GAS 集成

```
UYMRPGAssetManager (UAssetManager 子类)
  ├─ 定义静态 FPrimaryAssetType 常量（"Potion"/"Token"/"Equipment"）
  ├─ StartInitialLoading() → UAbilitySystemGlobals::InitGlobalData()
  └─ ForceLoadItem() 同步加载工具
         │
         │ 管理
         ▼
UYMRPGItem (UPrimaryDataAsset 子类, abstract, BlueprintType)
  ├─ ItemType / ItemName / ItemDescription / ItemIcon / Price / MaxCount / MaxLevel
  ├─ IsConsumable() → MaxCount > 0 即消耗品
  ├─ GetIdentifierString() → FPrimaryAssetId(Type, Name).ToString()
  └─ GetPrimaryAssetId() → FPrimaryAssetId(ItemType, GetFName())
         │
    ┌────┼────────┐
    ▼    ▼        ▼
Equipment  Potion    Token
 (挂 GE)   (授 GA)  (纯背包)
```

**关键设计决策**：

1. **用 `FPrimaryAssetType ItemType` 区分子类而非 RTTI**。每个子类构造时写死自己的 ItemType（引用 AssetManager 的静态常量）。`GetPrimaryAssetId()` 返回 `Type:Name`，AssetManager 查找不需要知道具体子类。

2. **Equipment = 挂 GE**。`UGameplayEffect` 子类引用，穿戴时 Apply（Infinite Duration），卸下时 Remove。天然匹配 GE Duration 语义。

3. **Potion = 授 GA**。`UYMRPGGameplayAbility` 子类引用 + AbilityLevel，使用时 Grant → Activate → 结束后 Remove。和之前学的动态授予/移除 GA 方案完全对接。

4. **Token = 类型标记器**。空子类，仅设 ItemType。代币/任务道具不需要 GAS 交互。

5. **`UAbilitySystemGlobals::InitGlobalData()`** 是 GAS 项目标配调用。初始化 `FGlobalActiveGameplayEffectHandles` 映射表和 `TargetDataStructCache`。不调用会导致 TargetData 网络传输和 GE 句柄查找失败。放在 `StartInitialLoading()` 确保游戏加载阶段就完成初始化。

### 项目清理

- 删除 26 个中文名 txt 占位文件（"新建 文本文档" 及副本），替换为 `Placeholder{N}.txt`
- 修复 `YMRPGItem.cpp:14`：`GetPrimaryAssetId.ToString()` → `GetPrimaryAssetId().ToString()`（缺少函数调用括号）

</details>

---

<details>
<summary><b>2026-06-06</b></summary>

### RPC 完整调用链——源码逐层追踪

之前（5.31）学了三方向 + Reliable/Unreliable 基础，今天追源码走完一条 RPC 的完整路径。

**调用链五步**（以 `ServerTryActivateAbility` 为例）：

```
客户端调用 Server RPC
  → UHT 生成的 exec* thunk 拦截
  → GetFunctionCallspace() 判定走 Local 还是 Remote
  → CallRemoteFunction() → ProcessRemoteFunction()
  → 序列化参数到 Bunch → Actor Channel 发送
  → 服务器收包 → 反序列化 → 调 _Implementation
```

**`GetFunctionCallspace` 决策树**（`Actor.cpp:5396-5594`）：

| 判定条件 | 结果 |
|---|---|
| `NetMode == NM_Standalone` | Local（单机直接本地执行） |
| `FUNC_NetMulticast` + 服务器 | `Local \| Remote`（本地执行 + 广播） |
| `FUNC_NetServer` + 客户端 | Remote（发往服务器） |
| `FUNC_NetClient` + 服务器 | Remote（发往拥有客户端） |
| Server RPC + 服务器上调 | **Local**（不经过网络，直接执行） |
| Client RPC + 客户端上调 | **Local**（同上） |

**关键源码**（`Actor.cpp:5506-5523`）：服务器调 Server RPC = 走 `Callspace`（Local），因为 `bIsServer && !(FUNC_NetClient)`。Listen Server 和 Standalone 的天然优化。

**`ProcessRemoteFunction`**（`NetDriver.cpp:7795-7936`）：
- Server RPC 路径：`Actor->GetNetConnection()` 找拥有连接 → `InternalProcessRemoteFunction` 序列化发送
- Multicast RPC 路径：遍历所有客户端连接 → 对每个调用 `IsNetRelevantFor` 检查相关性 → 相关则发送
- **Multicast 不保证可靠**——UE 设计哲学：多播用于表现层，状态同步靠属性复制

### WithValidation 机制

声明 `UFUNCTION(Server, reliable, WithValidation)` → UHT 额外生成 `_Validate` 函数。

**执行顺序**：服务器收到 RPC → 先调 `_Validate` → 返回 false 则断开客户端（反作弊） → 返回 true 才调 `_Implementation`。

**设计原则**：Validate 放"一眼判定作弊"的检查（参数范围、权限），不放复杂业务逻辑。GAS 的 `ServerTryActivateAbility_Validate` 直接 `return true`——复杂验证交给 PredictionKey 机制。

### RPC vs 属性复制——三个维度的本质区别

| | RPC | 属性复制 |
|---|---|---|
| **语义** | "做一件事"（事件驱动） | "状态变成 X"（状态同步） |
| **状态追踪** | 无 ShadowState | ShadowState 比较差异，增量发送 |
| **Late Join** | 不重放历史 RPC | 自动初次复制当前值 |
| **底层入口** | `ProcessRemoteFunction` | `FRepLayout::ReplicateProperties` |
| **适用场景** | 开火、跳跃、激活技能 | 血量、等级、Tag 容器 |

**记忆口诀**：
- 属性复制 = 同步水位高度（持续性状态，晚来的船也能看到当前水位）
- RPC = 按一下喇叭（一次性事件，后来的人听不到）

### AttributeSet 属性复制——两层机制

ASC 用两个 UPROPERTY 完成属性同步：

**第一层：`SpawnedAttributes`**——`ReplicatedUsing = OnRep_SpawnedAttributes`（`AbilitySystemComponent.h:1957`）。复制 AttributeSet 对象列表。客户端收到后清理旧 Aggregator 或切换到服务器权威对象。

**第二层：`ActiveGameplayEffects`**——`FActiveGameplayEffectsContainer : FFastArraySerializer`（`GameplayEffect.h:1621`），`TStructOpsTypeTraits` 标记 `WithNetDeltaSerializer = true`。这才是属性**数值**的复制通道：

```
服务器：GE.Apply → Modifier 计算 → Aggregator 更新 CurrentValue
  → ActiveGameplayEffects 标记 dirty
  → NetDeltaSerialize 只序列化变化的 GE 条目
  → 发往客户端

客户端：收到增量 → PostReplicatedAdd/Change
  → 重新构建 Aggregator → 重新计算 CurrentValue → 值同步
```

**结论**：属性数值通过 GE 聚合器的增量序列化同步，不是直接复制 `FGameplayAttributeData::CurrentValue` 的 float。

### Component::SetIsReplicated — 子对象属性复制通道

`SetIsReplicated(true)` 走的是**属性复制体系中的子对象复制**，不是 RPC。

**完整路径**：
1. `SetIsReplicated(true)` → `bReplicates = true` + `UpdateReplicatedComponent(this)`
2. `UpdateReplicatedComponent` → 加入 Actor 的 `ReplicatedComponents` + `ReplicatedComponentsInfo`
3. `UActorChannel::ReplicateActor()` 每帧遍历 `ReplicatedComponents` → `WriteSubObjectInBunch` → **调 Component 自己的 `FRepLayout::ReplicateProperties`**（和 Actor 属性复制同管线）
4. 首次创建子对象指令 → 后续增量属性复制 → `CallRepNotifies`

**三层网络机制的完整分类**：

| | RPC | Actor 属性复制 | Component 子对象复制 |
|---|---|---|---|
| **入口** | `UFUNCTION(Server/Client/NetMulticast)` | `UPROPERTY(Replicated)` | `SetIsReplicated(true)` |
| **底层** | `ProcessRemoteFunction` | `FRepLayout::ReplicateProperties` | 同左 |
| **Late Join** | 不重放 | 自动初始复制 | 自动初始复制 |

### ReplicatedUsing (RepNotify) — 复制管线第三阶段的回调钩子

不是独立的同步机制，而是属性复制管线在客户端的回调钩子。

**属性复制三阶段**：
1. **脏检测**（服务器）：ObjectData vs ShadowState 逐属性比较（Push Model 用 dirty bit 优化跳过）
2. **序列化传输**：只发送差异属性
3. **接收 + RepNotify**（客户端）：**先设置属性值，再调 OnRep 函数**

源码证据（`RepLayout.cpp:4661-4790`）——`CallRepNotifies` 遍历 `RepState->RepNotifies` 列表，通过 `Object->ProcessEvent(RepNotifyFunc, ...)` 调用 OnRep 函数。OnRep 被调时属性值已经是最新的。

### 背包系统网络架构

今天对 `ActiveSkillByInventoryId`（Server RPC）和 `ClientRPCFunction`（Client RPC）做了完整的双向网络调用链逐行分析：

**路径A：客户端使用物品**
```
客户端 UI_InventorySlot 点击
  → ActiveSkillByInventoryId(InventoryId) [Server RPC]
  → 参数由客户端提供（UI 格子的 InventoryId）
  → 序列化 InventoryId → 网络

服务器 _Implementation:
  → InventoryComponent->ActiveSkillByInventoryId(InInventryId)
  → 查 InventoryItems → 是 Potion → GiveAbility → TryActivateAbility
  → GA 提交 → lambda → ClientRPCFunction(AbilityTags, CoolDownTime) [Client RPC]
  → 参数由服务器提供（GA 实例的 AbilityTags + GetCooldownTimeRemaining()）

客户端 _Implementation:
  → AbilityCoolDownDelegate.Broadcast → UI 更新冷却
```

**路径B：服务器修改背包 → 属性复制自动推送**
```
服务器: AddInventoryItem → InventoryItems 变化
  → 属性复制增量 → 客户端 OnRep_InventoryItems → delegate → UI 刷新
```

**路径C：UI 面板打开时初始刷新**
```
客户端: UI_InventoryPanel::NativeConstruct
  → 绑定 OnInventoryItemChanged delegate
  → 用客户端已有的 InventoryItems（之前属性复制来的）刷新 UI
```

### Bug 修复

**C4458 编译错误**：`InventoryItemChanged_Implementation` 参数名 `InventoryItems` 与类成员变量 `InventoryItems` 同名。C++ 参数优先级高于成员变量，产生名称遮蔽。UE `/W4` + `/WX` 将 C4458 视为 Error。修复：参数改名 `InInventoryItems`（UE 惯例 `In` 前缀）。

**代码问题记录**：
- `CallServerDownLoadInfo` 命名误导：实际不经过网络，应改为 `RefreshInventoryUI`
- `ActiveSkillByInventoryId` 缺少 `WithValidation`：客户端参数直接做数组下标无边界检查
- `InventoryItemChanged` Client RPC 从未被服务器端调用：`OnRep_InventoryItems` 已覆盖属性复制刷新路径，该 RPC 可考虑删除

</details>

---

<details>
<summary><b>2026-06-07</b></summary>

### Blueprint Timeline — 时间驱动动画节点

Timeline 是蓝图中的时间驱动插值工具，底层由两套机制配合：

| 编译期 | 运行期 |
|---|---|
| `UTimelineTemplate`：存储轨道定义、曲线数据（蓝图节点编译时生成） | `FTimeline`：每帧 Tick，计算当前值，触发 Update/Finished 回调 |

**Track 类型**：Float Track、Vector Track、Color Track、Event Track（一条 Timeline 可有多条轨道）。

**Playback 控制**：Play / PlayFromStart / Stop / Reverse / SetNewTime / SetPlayRate，Length 由最长轨道曲线决定。

**C++ 侧等效做法**：手动创建 `FTimeline`，通过 `AddInterpFloat` 绑定曲线 + `FOnTimelineFloat` 委托，在 `Tick` 中调 `TickTimeline(DeltaTime)`。蓝图 Timeline 本质是这套流程的自动生成封装。

**常用场景**：开门/关门旋转动画、材质参数过渡、UI 缓动滑入/淡入、过场事件编排。

### AIGC 概念与提效

**AIGC = AI Generated Content**，用 AI 模型自动生成文本、图像、3D、代码、音频、视频等数字内容。

**游戏开发的提效模式**：
- 策划：ChatGPT 批量出 NPC 对话草稿 → 人力精修
- 美术：Midjourney 出概念图 → 挑图细化
- 程序：Claude Code 生成样板代码 → 人审查和改业务逻辑
- 3D：Meshy / CSM 生成道具模型 → 美术调整

**本质**："AI 出草稿，人做决策和精修"——把重复劳动交给 AI，人的精力集中在判断、设计、调优。

**求职关联**：国内大厂普遍在推 AIGC 工具链融入美术/策划/程序管线。面试时结合具体的 GAS/AI 辅助开发经历说明 "AI 做了什么、我做了什么" 更有说服力。

### Git 团队管理模式

| 模型 | 特点 | 适用 |
|---|---|---|
| **Git Flow** | main + develop + feature/release/hotfix 多分支，按版本合并 | 有固定发布周期的中大型团队 |
| **Trunk-Based** | 所有人往 main 高频合并，靠 Feature Flag 隐藏未完成功能 | 有完善 CI/CD + Feature Flag 体系的团队 |
| **GitHub Flow** | 无 develop，feature → main → tag，比 Git Flow 简单 | 中小团队 |
| **Fork-Based** | 每人 fork 个人副本，PR 合入主仓库 | 开源项目（Epic UE 源码也用此模型） |

**UE 项目特殊性**：Trunk-Based 在 UE 项目中较痛苦——蓝图、关卡等二进制资产的冲突处理复杂，频繁合并不可行。国内 UE 游戏团队最常见 Git Flow 变体，配合 Git LFS 管理大文件，feature 分支按模块划分。

### README 文档重写

参考简历描述重写了 README.md：
- 去掉"不依赖大模型生成代码"，改为"基于对架构的设计与掌握指导规范代码生成"
- 新增项目文件目录讲解章节（靠前放置），逐目录说明模块职责
- 更新进度：从 2 项勾选扩展到 10 项已完成 + 3 项待做
- 新增网络架构表格（属性复制 / RPC / Component 子对象复制 / FFastArraySerializer）
- 新增 AIGC 辅助开发章节（源码蒸馏→Skills、学习日志归档、代码生成模式）

</details>
