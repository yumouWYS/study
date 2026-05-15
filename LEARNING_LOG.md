# 学习日志

记录每日有技术价值的提问与对应知识点，按日期归档，方便检索复习。

## 目录

- [2026-05-08](#ymrpg-2026-05-08) — UCLASS 说明符、UBT/UHT 构建系统、GA 生命周期与四项策略、FObjectInitializer、CurrentActorInfo 解耦、网络模型、GameplayTag 体系
- [2026-05-10](#ymrpg-2026-05-10) — 输入系统架构（三个 Handle 数组）、AbilitiesToAdd/Active 配置层、FGameplayAbilitySpec 全字段、CDO 陷阱、ProcessAbilityInput 调用链、DynamicAbilityTags 传递链、数据驱动 vs 继承、 WaitInputRelease 排查、UE 5.4→5.5 PredictionKey 迁移根因
- [2026-05-13](#ymrpg-2026-05-13) — ComboComponent 连招组件、AnimNotify vs AnimNotifyState、const 成员函数返回指针、ASC StopMontageIfCurrent 与 Montage 双通路、ProcessAbilityInput 三阶段、ENetMode 网络模式、bOrientRotationToMovement
- [2026-05-16](#ymrpg-2026-05-16) — Attribute 角色与功能、ATTRIBUTE_ACCESSORS 宏展开、FGameplayEffectSpec 运行时实例、CDO vs UClass、UAttributeSet 五个钩子函数、GetLifetimeReplicatedProps、REPNOTIFY_Always vs OnChanged、__super vs Super

---

<details>
<summary><b>2026-05-08</b></summary>

### UCLASS 说明符

- **`Abstract`**：标记为抽象类，蓝图类选择器中不可直接创建，只能作为父类被继承。类比 C++ 纯虚类，但更偏向"编辑器中不可实例化"。
- **`HideCategories`**：在蓝图细节面板隐藏指定分类的属性。`HideCategories = Input` 对 `UGameplayAbility`（继承 `UObject`）无效，因为 `Input` 是 `AActor` 的分类。
- **`Meta = (ShortTooltip = "...")`**：蓝图类选择器悬停时显示的简短提示文字。

### UE 构建系统（UBT / UHT）

- **UHT（Unreal Header Tool）**：构建的第一步，解析所有 `.h` 文件，生成 `.generated.h` 和 `.gen.cpp`。失败则整个构建无法继续。
- **UBT（Unreal Build Tool）**：负责模块依赖解析、工具链选择、makefile 创建、调度 UHT 和编译器。
- **`.generated.h` 缺失的级联效应**：IntelliSense 的 E0293 "不允许使用间接非虚拟基类" 就是典型症状——`GENERATED_BODY()` 和 `Super` typedef 展开异常，不是语法问题，是构建产物缺失。
- **自适应构建（Adaptive Non-Unity Build）**：UBT 通过 `git status` 扫描工作区文件来确定变更集。如果 `git status` 因文件路径问题崩溃，整个 makefile 创建就失败。
- **makefile 缓存**：makefile 存在且比源码新时，UBT 跳过全量扫描直接复用。`.uproject` 修改或 `Intermediate` 被清会触发全量重建。

### Git 状态与 UBT 交互

- **带引号路径导致 UBT 崩溃**：`git status` 对含空格/中文的文件名输出带双引号。UBT 的 `GitSourceFileWorkingSet.AddPath()` 未处理引号，`.NET` 抛出 `ArgumentException`。
- **暂存（staged）和未暂存（unstaged）**：`git status` 对 `D`（删除）和 `R`（重命名）状态都会输出旧文件名。仅 `git add` 不够——重命名行（`R`）带着旧中文名直到 commit。
- **空文件与跨目录重命名**：`git` 的 rename detection 依赖文件内容相似度。空文件（0 字节）无法正确匹配，导致跨目录乱配对。

### UGameplayAbility 生命周期

六个核心函数，调用链：

```
TryActivateAbility()    → 入口，权鉴 + 网络复制 + 实例化
  CanActivateAbility()  → 纯只读检查，不改状态（const）
  CallActivateAbility() → 样板前置，标记 Active、注册 Tag、等待复制
    ActivateAbility()   → 唯一需要子类重写的函数，技能实际逻辑
      CommitAbility()   → 必须调用！扣资源 + 进冷却
      EndAbility()      → 技能自己结束自己
CancelAbility()         → 外部打断（被击飞/死亡）
```

- **`CanActivateAbility()` 需重写的条件**：无法用 GameplayTag / GE 表达的额外条件。引擎的 Cost/Cooldown GE 和 ActivationBlockedTags 已自动处理常见门禁。
- **Tag/GE 门禁的配置位置**：都在 `UGameplayAbility` 的 `UPROPERTY` 上（`ActivationBlockedTags`、`ActivationRequiredTags`、`CostGameplayEffectClass`、`CooldownGameplayEffectClass`），蓝图 Class Defaults 里填。

### GA 四项策略

| 策略 | 值 | 含义 |
|------|-----|------|
| `ReplicationPolicy` | `ReplicateYes` | 技能激活/结束状态同步给所有客户端 |
| `InstancingPolicy` | `InstancedPerActor` | 每个 Actor 一个持久实例，复用 |
| `NetExecutionPolicy` | `LocalPredicted` | 客户端先预测执行，服务器确认后正式 |
| `NetSecurityPolicy` | `ClientOrServer` | 客户端和服务器都能发 RPC 激活 |

- **四项合一起的效果**：标准动作游戏战斗技能——客户端零延迟手感 + 服务器权威验证 + 状态全网同步 + 实例常驻复用。
- **基类设一次即可**：构造函数里设，子类自动继承。个别特殊技能单独覆盖即可。子类必须调 `Super(ObjectInitializer)`。

### FObjectInitializer 与构造函数

- **为什么有的构造函数不写 `ObjectInitializer` 参数也没事**：UHT 生成的隐藏构造函数 + TLS 读取机制。`FObjectInitializer::Get()` 是一个线程局部静态函数，整个构造链共享同一个 Initializer。
- **什么时候必须写**：`AActor` / `UActorComponent` 子类里用了 `CreateDefaultSubobject`。UE 官方源码的风格是始终显式写。
- **`CreateDefaultSubobject`**：在构造函数里为自己创建子组件。只能在构造函数里用；构造外用 `NewObject<T>()`。
- **构造函数不能做的事**：调用 `GetWorld()`（可能返回空）、初始化需要 World 上下文的系统（如 ASC 的 `InitAbilityActorInfo`）。

### TLS（线程局部存储）

- 操作系统给每个线程分配的独立存储空间。同一全局变量名，不同线程读写各自副本，不共享不加锁。
- UE 利用它传递 `FObjectInitializer`：`NewObject<T>()` 把 Initializer 存 TLS，构造函数链通过 `FObjectInitializer::Get()` 读同一个。
- UObject 操作通常只在主线程，TLS 是防御性设计。

### CurrentActorInfo 与解耦设计

- **`FGameplayAbilityActorInfo`**：聚合 `OwnerActor`、`AvatarActor`、`PlayerController`、`ASC`、`AnimInstance`、`SkeletalMeshComponent`、`MovementComponent`。
- **初始化时机**：`ASC::InitAbilityActorInfo(Owner, Avatar)` 调用时。此函数需手动在 `BeginPlay` / `PossessedBy` 中调用。
- **为什么 ASC 不自己调**：ASC 不知道 Owner 和 Avatar 分别是谁——这是设计决策，把选择权留给你。
- **解耦效果**：GA 不关心 ASC 挂在哪（Character / PlayerState / Controller），只从 `CurrentActorInfo` 读现成指针。架构迁移时 GA 代码一行不用改。
- **`TWeakObjectPtr`**：GA 可能异步执行（`WaitDelay`、`WaitTargetData`），期间拥有者可能死亡。弱引用自动变 `nullptr`，防止悬垂指针。

### 网络模型

- **`GetLocalRole() == ROLE_Authority`**：只在服务器端执行初始化类操作。配置类操作（`InitAbilityActorInfo`、`GiveAbility`）需判断；运行时操作（`ApplyGameplayEffect`）引擎内部已做复制分发。
- **服务器和客户端各有一份 ASC**：服务器是权威端，客户端是镜像。属性通过复制自动同步。客户端的 `InitAbilityActorInfo` 由复制自动完成，不需要手动调。
- **客户端 ASC 不只收发信息**：还做预测执行（`LocalPredicted`）、属性缓存（UI 读本地数据）、输入绑定。
- **动画/TFX 不跑服务器的原因**：专用服务器无显卡。纯表现走 `GameplayCue`（引擎保证只在客户端执行），影响 Gameplay 的动画用 AbilityTask（如 `PlayMontageAndWait`，服务器也要跑以掌握动画进度和 AnimNotify 触发）。

### GameplayTag 体系

- **树状层级**：用 `.` 分隔。父 Tag 查询自动包含子 Tag（`Status.Death` 匹配 `Status.Death.Dying` 和 `Status.Death.Dead`）。
- **五类 Tag**：Ability_Behavior（技能元数据）、InputTag（输入绑定）、GameplayEvent（事件驱动）、SetByCaller（数值透传）、Status（运行时状态标记）。
- **Tag 定义位置**：`.h` 用 `UE_DECLARE_GAMEPLAY_TAG_EXTERN` 声明，`.cpp` 用 `UE_DEFINE_GAMEPLAY_TAG_COMMENT` 定义（含描述）。

</details>

---

<details>
<summary><b>2026-05-10</b></summary>

### 输入系统架构 — 三个 Handle 数组

在 ASC 中维护三个数组，连接 Enhanced Input 与技能激活：

| 数组 | 输入事件 | 典型用途 |
|------|---------|---------|
| `InputPressedSpecHandles` | 按下瞬间 | 普攻、闪避、跳跃 |
| `InputReleasedSpecHandles` | 松开瞬间 | 蓄力攻击、拉弓射箭 |
| `InputHeldSpecHandles` | 持续按住 | 瞄准、格挡、奔跑 |

- **为什么不直接在输入回调里 `TryActivateAbility`**：硬编码耦合严重，且按下的瞬间 ASC 可能还没准备好（网络复制中、被其他技能阻止）。
- **设计原理**：输入到技能激活分批走——按下的瞬间只"缓存 Tag 到 Handle 的匹配"，每帧的 `ProcessAbilityInput` 再统一处理激活。

### 配置层 vs 索引层 — AbilitiesToAdd / AbilitiesToActive

四个数据结构的职责分工：

```
AbilitiesToAdd (TMap<Tag, TSubclassOf<UA>>)      ← 静态配置层，蓝图填写"这个角色有什么技能"
        ↓ BeginPlay() GiveAbility()
AbilitiesToActive (TMap<Tag, Handle>)             ← 运行时索引层，Tag→Handle 的 O(1) 查表
        ↓ AbilityInputTagPressed / Released
InputPressedSpecHandles / Released / Held         ← 输入分发层，按输入状态分类
        ↓ ProcessAbilityInput
TryActivateAbility(Handle)                        ← 执行层
```

- **AbilitiesToAdd**：蓝图中设计师填 `{InputTag.Jump → GA_Jump}`。`UPROPERTY(EditDefaultsOnly)`，CDO 上配置。
- **AbilitiesToActive**：`BeginPlay()` 里 `GiveAbility()` 返回 Handle 后填入。无 `UPROPERTY`——纯运行时成员，供 AI 决策 / GameplayEvent 触发时 O(1) 查表。
- **三个数组**：只用于玩家输入，区分按下/松开/按住三种状态。

### FGameplayAbilitySpec 全字段

`GiveAbility()` 时 ASC 内部分配一个 `FGameplayAbilitySpec`，放入 `ActivatableAbilities` 数组。

| 分组 | 字段 | 含义 |
|------|------|------|
| 身份 | `Ability` | GA 的 CDO / 类引用 |
| | `AbilityInstance` | 实例化策略为 `InstancedPerActor` 时的常驻实例 |
| | `NonInstancedAbility` | 临时实例（`InstancedPerExecution`） |
| | `Handle` | int32 句柄，Index + Generation 组成 |
| | `SourceObject` | 谁授予了这个技能（角色自身/装备/队友 Buff 来源） |
| 状态 | `Level` | 技能等级，透传给 GE 的 CurveTable 查表 |
| | `InputID` | 历史遗留的 int32 输入绑定（Lyra 风格已改用 Tag） |
| | `bIsActive` | `ActiveCount > 0` |
| | `ActiveCount` | 同时激活次数（`InstancedPerActor` 下最多 1） |
| Tag | `AbilityTags` | 从 GA CDO 拷贝的**静态**标签——定义"这是什么种类" |
| | `DynamicAbilityTags` | **运行时动态**添加的标签——同一个 GA 类，不同角色的 Spec 可以有不同标签 |
| | `ActivationOwnedTags` | 激活时给自己打上，结束时自动移除 |
| | `CancelAbilitiesWithTag` | 激活时自动取消带这些 Tag 的技能 |
| | `BlockAbilitiesWithTag` | 激活期间阻止带这些 Tag 的技能 |
| | `ActivationRequiredTags` / `ActivationBlockedTags` | 激活前的门禁条件 |
| 特殊 | `RemoveAfterActivation` | 一次性技能（药水/卷轴），用完自动移除 |
| 网络 | `ReplicatedData` | 复制 `bIsActive`、`ActiveCount`、`Handle` 到客户端 |

**AbilityTags vs DynamicAbilityTags 的核心区别**：
- `AbilityTags` 来自 CDO，是静态的、被 `GiveAbility()` 时一次性拷贝。所有角色同一 GA 的 AbilityTags 完全相同。
- `DynamicAbilityTags` 属于 Spec 私有，运行时增删。同一个 GA 类，不同角色可打不同 Dynamic Tag。
- **典型用途**：武器元素附魔——"火焰剑"给攻击技能打上 `Tag.Element.Fire`，GE Execution 检测此 Tag 就附加灼烧。卸剑清 Tag，换冰杖打冰 Tag。GA 代码一行不改。

### CDO 陷阱 — GetCurrentAbilitySpecHandle 为什么不能走 CDO

```cpp
FGameplayAbilitySpecHandle UGameplayAbility::GetCurrentAbilitySpecHandle() const
{
    return CurrentSpecHandle;  // 在 CallActivateAbility() 时被赋值
}
```

- CDO（Class Default Object）是**全局唯一**的单例，被所有角色共享。
- `CurrentSpecHandle` 只在技能**激活后**才有效，`GiveAbility()` 时不会设置。
- 用 CDO 的 `GetCurrentAbilitySpecHandle()` 拿到的不是"当前 Spec 的 Handle"，而是"上次谁激活了这个 CDO 时写入的值"——角色 A 激活后残留的是 A 的 Handle。
- **正确做法**：直接从 `FGameplayAbilitySpec::Handle` 取——`GiveAbility()` 返回的那一刻就是对的，不需要等激活，不需要绕 CDO。

### ProcessAbilityInput — 输入分发的三阶段

```cpp
void UYMRPGAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
    // 阶段一：处理按下
    for (Handle in InputPressedSpecHandles):
        Spec.InputPressed = true;
        if (Spec.IsActive()):
            AbilitySpecInputPressed(Spec);  // 技能在跑→送事件进去
        else:
            AbilitiesToActive.Add(Handle);  // 技能未激活→待激活

    // 阶段二：真正激活
    for (Handle in AbilitiesToActive):
        TryActivateAbility(Handle);

    // 阶段三：处理松开
    for (Handle in InputReleasedSpecHandles):
        Spec.InputPressed = false;
        if (Spec.IsActive()):
            AbilitySpecInputReleased(Spec);  // 松开事件送进 GA 内部

    // 阶段四：清空 Pressed/Released 缓冲区（Held 保留跨帧）
    InputPressedSpecHandles.Reset();
    InputReleasedSpecHandles.Reset();
}
```

- **调用位置**：`PlayerController::PostProcessInput()`，比普通 Tick 更早，保证输入在物理/渲染之前被处理。
- **为什么分三阶段**：避免"同一个 Frame 内激活又立刻收到松开事件"的问题——先让所有该激活的技能激活完，再统一处理松开。
- **Held 不被 Reset**：蓄力、瞄准、防御等"按住持续生效"的技能需要跨帧保持标记，在 `AbilityInputTagReleased` 时才从 Held 移除。

### AbilitySpecInputPressed / Released 重写

引擎默认实现只做 `TryActivateAbility`。Lyra 风格重写后成为**输入事件分发中心**：

```cpp
void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
    Super::AbilitySpecInputPressed(Spec);  // 走引擎默认
    if (Spec.IsActive())
    {
        // 技能已在运行→把 InputPressed 事件送进 GA 内部
        InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed,
            Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
    }
}
```

- **关键逻辑**：技能已激活时不重新激活，而是把事件**送给正在运行的 GA 实例**。
- **蓄力攻击的数据流**：按下时激活 GA_ChargeAttack（启动 WaitInputRelease Task）→ 松开时 `AbilitySpecInputReleased` → `InvokeReplicatedEvent(InputReleased)` → WaitInputRelease Task 收到事件 → 释放蓄力。
- 如果没重写这两个函数，`WaitInputPress` / `WaitInputRelease` Task 永远收不到事件，蓄力连招等技能无法工作。

### 完整调用链：一次空格跳跃

```
玩家按空格
  → EnhancedInput BindAction(JumpAction, Started, &ActiveJump)
  → YMRPGCharacter::ActiveJump()
  → YMRPGGameplayTags::FindTagByString("Input.Jump") → 获取 Tag
  → ASC::AbilityInputTagPressed(Tag)
      → 遍历 ActivatableAbilities
      → 匹配 Spec.DynamicAbilityTags.HasTagExact(Tag)
      → InputPressedSpecHandles.Add(Handle)
      → InputHeldSpecHandles.Add(Handle)

下一个 PostProcessInput()
  → ASC::ProcessAbilityInput(DeltaTime, bGamePaused)
      → 阶段一：Spec.InputPressed = true, !IsActive()
          → AbilitiesToActive.Add(Spec.Handle)
      → 阶段二：TryActivateAbility(Handle)
          → CanActivateAbility() → 检查代价/冷却/Tag阻止
          → CallActivateAbility() → 设置 CurrentSpecHandle
          → ActivateAbility() → GA_Jump 执行跳跃逻辑
      → 阶段四：InputPressedSpecHandles.Reset()

玩家松空格
  → ASC::AbilityInputTagReleased(Tag)
      → InputReleasedSpecHandles.Add(Handle)
      → InputHeldSpecHandles.Remove(Handle)

下一次 ProcessAbilityInput()
      → 阶段三：Spec.InputPressed = false, IsActive()
          → AbilitySpecInputReleased(Spec)
          → InvokeReplicatedEvent(InputReleased)
          → GA 内部 WaitInputRelease 收到回调
```

### GameplayAbilitySpecHandle 编译错误

`TMap<FGameplayTag, FGameplayAbilitySpecHandle>` 的 `FGameplayAbilitySpecHandle` 是值类型（不是指针），模板实例化时需要完整定义。只前向声明 `class UYMRPGAbilitySystemComponent` 不够——需要 `#include "GameplayAbilitySpec.h"`。

### 数据驱动 vs 继承

GAS 不鼓励用继承变种技能（GA_MeleeAttack → GA_MeleeAttack_Fire → ...）。差异性大半在数据配置层面：

| 差异维度 | 数据驱动方案 |
|---------|------------|
| 伤害值 | `DamageGameplayEffect` 上配不同 Magnitude / SetByCaller |
| 附带 Buff | `AdditionalEffectMap` 按 Tag 选 GE |
| 冷却时间 | `CooldownGameplayEffectClass` 蓝图变量 |
| 动画 | 武器资产上存 Montage，GA 从武器读取 |
| 命中判定 | 武器资产上存 TraceShape / Distance / HalfSize / Angle |
| 伤害类型 | 武器资产上存 DamageTypeTag 和 DamageEffectClass |

继承只在 C++ 流程逻辑真不一样时使用（GA_Channeled / GA_Projectile / GA_Passive 三大类）。

### 武器数据资产的运行时传递

```
武器 DataAsset
  ├── Montage 数组（轻击连招 + 重击 + 空中）
  ├── Trace 参数（距离 / 扇形角度 / 碰撞盒尺寸）
  ├── DamageGE Class
  ├── SetByCaller 数值（BaseDamage / ArmorPen / CritRate）
  └── 元素 Tag（Physical / Fire / Ice / Thunder）

GA::ActivateAbility():
  Montage = GetCurrentWeaponData()->GetComboMontage(Index)
  TraceParams = GetCurrentWeaponData()->GetTraceConfig()
  DamageGE = GetCurrentWeaponData()->GetDamageGE()
  GE Spec 填入 SetByCaller 数值
```

- **GE Execution** 检测 `DynamicAbilityTags` 中的元素 Tag，自动附加对应效果（灼烧/减速/感电）。
- GA 本身只做"什么时候做 Trace、什么时候挂 GE"，具体参数全从武器读。
- 换武器 = 换 DataAsset 引用，零 C++ 改动。

### PlayerInputComponent 与 APlayerController 协同

- **PlayerController** 是决策者：拥有 InputComponent、每帧调度输入处理时机、通过 `PostProcessInput` 驱动 `ProcessAbilityInput`
- **InputComponent** 是接线员：挂载在 Controller 或 Pawn 上，把按键翻译成函数调用
- **挂载决定绑定范围**：
  - 挂在 Pawn 上（`SetupPlayerInputComponent`）→ 换 Pawn 自动换绑定
  - 挂在 Controller 上（`SetupInputComponent`）→ 全局统一绑定
- **每帧调用时序**：`PlayerTick` → `PreProcessInput` → InputStack 处理 → 回调触发 → `PostProcessInput` → `ProcessAbilityInput`

### AssetManager 预热 — InitGlobalData

`UYMRPGAssetManager::StartInitialLoading` 中调用 `UAbilitySystemGlobals::InitGlobalData()`：

- **目的**：游戏启动时预加载 GAS 全局数据表（属性元数据、TargetData 序列化注册、GameplayCue 路径映射、CurveTable 列表）
- **为什么必须在 StartInitialLoading**：游戏世界创建之前的预加载阶段。运行时首次 ApplyGameplayEffect 不需要磁盘 IO
- **不调的后果**：首次 GE 多一次反射查找、GameplayCue 首次可能找不到特效、SetByCaller 曲线首次返回 0

### AttributeSet 结构

- **定义**：GAS 中所有数值的容器——血量、体力、攻击力、移速、护甲
- **FGameplayAttributeData 内部**：`{ CurrentValue, BaseValue }`，CurrentValue = BaseValue * Multiplier + Additive
- **为什么不用普通 float**：自带网络复制（`DOREPLIFETIME`）、变化回调（`PreAttributeChange` / `PostAttributeChange`）、Clamp 限制、预测执行
- **GA 不直接改属性**：GA 申请 GE，GE 修改 AttributeSet

### 蓝图 AbilityTag — 技能的身份证

AbilityTags **不是激活必要条件**，但决定技能能否参与 Tag 驱动的协作网络：

- 其他技能通过 `CancelAbilitiesWithTag` / `BlockAbilitiesWithTag` 找到它
- GE 通过 `SourceTags` / `TargetTags` 判断技能类型
- `ActivationOwnedTags`：激活时打上，结束时移除，让外部系统感知"正在干什么"

### 蓝图如何使用断点

- 右键点击节点 → "添加断点" 或 F9
- 运行在 Play In Editor 或独立进程模式下才能命中

### WaitInputRelease 无法触发的完整排查

**排查链路**（与教程对比后每一步都错了）：

1. C++ 日志：全链路正确（Pressed → Activate → Released → IsActive=1）
2. 尝试 `Instance->InputReleased()` 显式调用 → 无效，Task 监听的不是 `OnGameplayAbilityInputReleased`
3. 尝试 `GenericLocalInputCallbacks` 广播 → 无效，Task 监听的不是 ASC 本地回调
4. 尝试 `AbilityReplicatedEventDelegate` 直接广播 → 无效
5. **对比教程发现根因**——PredictionKey 来源：

```
UE 5.4: InvokeReplicatedEvent(Key ← Spec.ActivationInfo.GetActivationPredictionKey())
UE 5.5+: InvokeReplicatedEvent(Key ← Instance->GetCurrentActivationInfo().GetActivationPredictionKey())
```

**UE 5.5+ 核心变更**：PredictionKey 的权威存储从 `FGameplayAbilitySpec::ActivationInfo` 迁移到 `UGameplayAbility::CurrentActivationInfo`（实例级别）。

- **原因**：支持同一 Spec 多次激活（`InstancedPerExecution`），每个实例有独立 PredictionKey
- **后果**：`Spec.ActivationInfo` 在 5.5 被标记为 deprecated，存的是 GiveAbility 时旧值
- **校验机制**：`InvokeReplicatedEvent` 内部比对传入 Key 与 Task 注册时的 Key（来源实例）——不匹配就不广播 delegate

**最终修复**（仅两处）：

```cpp
// 1. ProcessAbilityInput: 用 Spec->Handle 代替 CDO->GetCurrentAbilitySpecHandle()
AbilitiesToActivate.AddUnique(AbilitySpec->Handle);

// 2. AbilitySpecInputPressed/Released: 从实例取 PredictionKey
const UGameplayAbility* Instance = Spec.GetPrimaryInstance();
FPredictionKey Key = Instance
    ? Instance->GetCurrentActivationInfo().GetActivationPredictionKey()
    : Spec.ActivationInfo.GetActivationPredictionKey();
InvokeReplicatedEvent(EventType, Spec.Handle, Key);
```

### InvokeReplicatedEvent 返回值的含义

- `true`：事件分发到 Task 且触发网络复制
- `false`：(1) 无 Task 监听，(2) PredictionKey 校验失败，(3) 客户端无需复制
- 单机/PIE 下返回 false 可能是因为不需要复制，不代表 Task 没收到的——前提是 PredictionKey 正确

</details>

---

<details>
<summary><b>2026-05-13</b></summary>

### ComboComponent 连招组件设计

**数据结构**：

| 字段 | 作用 |
|------|------|
| `ComboIndex` | 当前连招段数 |
| `ComboMaxIndex` | 段数上限（BeginPlay 从 `ComboMontages.Num() - 1` 推算） |
| `bShortPress` | 攻击键是否处于按住状态 |
| `ComboMontages` | 蓝图配置的各段 Montage 数组 |
| `LastPlayedMontage` | 上一段 Montage（`GetComboAnimMontage()` 的副作用） |

**核心方法**：

- `GetComboAnimMontage()`：取 `ComboMontages[ComboIndex]`，副作用存入 `LastPlayedMontage`
- `UpdateComboIndex()`：index+1，超上限归零（循环连招）
- `ResetComboIndex()`：归零（连招终止/末尾重置）
- `SetPressed()` / `ResetPressed()` / `IsStillPressed()`：攻击键状态标记

**设计思想**：将连招状态从 Character 拆分为独立 Component，职责单一。`SetIsReplicated(false)`——连招索引不做网络同步，走预测通路。

### AnimNotify vs AnimNotifyState

| | `UAnimNotify` | `UAnimNotifyState` |
|---|---|---|
| 生命周期 | 单帧触发 | 持续一段时间 |
| 重写函数 | `Notify()` | `NotifyBegin()` / `NotifyTick()` / `NotifyEnd()` |
| 典型用法 | 伤害判定、重置连招标记 | 碰撞检测窗口、输入缓冲窗口 |

两者都挂载在 AnimMontage 资产的 Notify 轨道上。Notify 是一个点，NotifyState 是一个段。

### AnimiNotifyState_NextCombo — 连招输入窗口

挂载在每段攻击 Montage 后摇部分。通过 NotifyState 的持续时间创建一个"输入缓冲窗口"：

```
NotifyBegin → ResetPressed()                    // 窗口开启，清掉按住标记
  ┌── 玩家在此窗口内按键 → SetPressed(true) ──┐
NotifyEnd   → if IsStillPressed() == true
                → ComboMelee() 下一段           // 窗口结束检测
              else → 连招断开
```

### AnimNotify_ResetCombo — 单帧重置连招

挂载在最后一段 Montage 末尾，`ResetComboIndex()` 归零。

### 连招完整数据流

```
按下近战键 → ActiveMelee()
  ├─ ComboComponent->SetPressed()
  └─ ASC->AbilityInputTagPressed(InputTag.Melee)
       → ProcessAbilityInput → TryActivateAbility(GA_Melee)
         → GetComboAnimMontage() → 取 montage[index] + 记 LastPlayed
         → UpdateComboIndex()    → index++
         → PlayMontageAndWait(montage)

Montage 后摇 → NotifyState_NextCombo
  → NotifyBegin: ResetPressed()
  → 窗口内按键: ActiveMelee() → SetPressed(true)
  → NotifyEnd: if IsStillPressed()
      → ComboMelee() (仅 AutonomousProxy)
        → StopMontageIfCurrent(LastMontage)
        → ActiveMelee() → 循环下一段

最后一段末尾 → AnimNotify_ResetCombo → ResetComboIndex() → index=0
```

### const 成员函数返回指针

`const` 成员函数约束的是 `this` 的成员变量，**不约束外部对象**：

```cpp
UAnimMontage* GetComboAnimMontage() const;  // 返回非 const 指针
// 调用方拿到指针后可以修改 Montage 对象，因为 const 只管 ComboComponent 自身
```

- 返回值：拷贝，跟原值无关
- 返回指针：可修改指向的外部对象
- 返回引用：必须 `const&`，改不了

### ASC 内置 Montage 管理

**核心数据结构 `FGameplayAbilityLocalAnimMontage`**：

| 字段 | 作用 |
|------|------|
| `AnimatingAbility` | 发起播放的 GA |
| `AnimMontage` | 当前播放的 Montage 资产 |
| `PlayInstanceId` | 同 Montage 多次播放的区分标记 |
| `PredictionKey` | 启动播放的预测键 |

**双通路设计**：

- **LocalAnimMontageInfo**：本地追踪，不复制。预测客户端走这条通路
- **RepAnimMontageInfo**：通过 `AnimMontage_UpdateReplicatedData()` 拷贝后复制，旁观者/模拟端走这条

**`StopMontageIfCurrent`**：

```cpp
void StopMontageIfCurrent(const UAnimMontage& Montage, float OverrideBlendOutTime)
{
    if (&Montage == LocalAnimMontageInfo.AnimMontage)  // 地址比较
        CurrentMontageStop(OverrideBlendOutTime);
}
```

用**地址比较**而非资产 ID，确保只停 ASC 当前追踪的那个 Montage 实例。只处理 `LocalAnimMontageInfo`（本地通路），用于外部打断场景（受击、死亡）。

**`PlayInstanceId` 的作用**：同个 Montage 连续播放时，递增 ID 防止 Notify 回调串到错误的播放实例。

**ASC 管 Montage 的原因**：让 Montage 生命周期与 GA 自动绑定——GA 结束 → PlayMontageAndWait Task 析构 → Montage 自动回收。

### ProcessAbilityInput 输入分发

`UAbilitySystemComponent` 没有内置此函数，是 **Lyra 示例项目**首创的自定义模式。

**由谁调用**：`PlayerController::PostProcessInput()` 每帧触发，位于所有输入回调处理完毕之后。

**三阶段处理**：

```
阶段一（Pressed）:
  ├─ AbilitySpec 未激活 → 加入 AbilitiesToActivate → 统一 TryActivateAbility
  └─ AbilitySpec 已激活 → AbilitySpecInputPressed → InvokeReplicatedEvent(InputPressed)

阶段二（Released）:
  └─ AbilitySpec 已激活 → AbilitySpecInputReleased → InvokeReplicatedEvent(InputReleased)

阶段三:
  └─ InputPressedSpecHandles / InputReleasedSpecHandles 清空
```

**设计意图**：输入回调只做缓存（`AbilityInputTagPressed`/`Released` 只填数组），`ProcessAbilityInput` 作为**统一的帧末激活点**，保证同一帧多个输入的时序确定性。

**时序链**：`PlayerTick → PreProcessInput → InputStack → 回调(ActiveMelee等) → PostProcessInput → ProcessAbilityInput`

### ENetMode 网络模式

| 模式 | 含义 |
|------|------|
| `NM_Standalone` | 单机模式（PIE 默认），逻辑上仍是服务器 |
| `NM_DedicatedServer` | 纯服务器，无渲染，无本地玩家 |
| `NM_ListenServer` | 有人当主机，既是服务器又是玩家 |
| `NM_Client` | 纯客户端，连接别人 |

**枚举值顺序是刻意设计**：`NM_Standalone(0) < NM_DedicatedServer(1) < NM_ListenServer(2) < NM_Client(3)`，因此 `GetNetMode() < NM_Client` 等价于"本进程是服务器变体"。

与 `GetLocalRole()` 的区别：
- `GetNetMode()`：问**当前进程**的身份
- `GetLocalRole()`：问**这个 Actor** 在本地是权威还是代理（`ROLE_AutonomousProxy` = 玩家自己控制的 Pawn）

### bOrientRotationToMovement

`UCharacterMovementComponent` 属性。设为 `true` 时，角色**自动朝加速度方向旋转**，转速由 `RotationRate` 控制。每帧 `PhysicsRotation()` → `ComputeOrientToMovementRotation()` 执行。

必须与 `bUseControllerRotationYaw` 互斥——后者是面朝摄像机方向，前者是面朝移动方向。动作游戏通常关前者、开后者。

</details>

---

<details>
<summary><b>2026-05-16</b></summary>

### Attribute 在 GAS 中的角色与功能

Attribute 是 GAS 中的游戏数值载体，承担三层角色：

- **数据载体**：所有参与游戏逻辑的浮点数（血量、攻击力、移速）由 `FGameplayAttributeData` 封装，通过 `ATTRIBUTE_ACCESSORS` 宏自动生成 Get/Set/Init 函数。引擎内部用 `FGameplayAttribute`（类似反射指针）引用任意属性，使 GE、GA、UI 无需依赖具体类名。
- **GE 的操作目标**：实现"数值变化"与"谁触发了变化"的解耦——GA 负责何时改，GE 负责怎么改，Attribute 是被操作的数据。GE 通过 Modifier 指向目标 Attribute，伤害计算、护甲减免可独立配置和组合。
- **网络同步单元**：ASC 内置对 Attribute 的复制，Attribute 变化时自动通过 `RepNotify` 推送到客户端，无需手写 RPC。

`FGameplayAttributeData` 内部 Base/Current 双值机制：Base 存永久修改（升级加点、装备），Current 存临时修改（Buff、扣血）。GE 过期时只清除 Current 贡献，Base 不受影响，因此 Buff 结束属性自动还原。

### ATTRIBUTE_ACCESSORS 宏展开

`ATTRIBUTE_ACCESSORS(ClassName, PropertyName)` 含四个子宏：

| 子宏 | 生成函数 | 用途 |
|------|---------|------|
| `PROPERTY_GETTER` | `static FGameplayAttribute GetXxxAttribute()` | 返回 `FGameplayAttribute`（属性指针），GE 里指定目标属性 |
| `VALUE_GETTER` | `FORCEINLINE float GetXxx() const` | 返回 `CurrentValue`，UI 显示、条件判断 |
| `VALUE_SETTER` | `FORCEINLINE void SetXxx(float)` | 通过 ASC `SetNumericAttributeBase` 修改，走完整 GAS 流程 |
| `VALUE_INITTER` | `FORCEINLINE void InitXxx(float)` | 同时设 Base 和 Current，直接写、不触发回调、不走 ASC |

**Setter 走 ASC 通道**意味着修改会触发 `PreAttributeChange` → 修改 → `PostGameplayEffectExecute`，且自动同步到客户端。直接赋值 `Health = x` 会绕过 GAS 体系，导致回调缺失、网络不同步。**Init 只在创建时调用一次**，避免初始化阶段触发死亡检测等不应运行的逻辑。

### DECLARE_MULTICAST_DELEGATE_SixParams 与 FGameplayEffectSpec

**委托声明**定义多点广播事件类型 `FYMRPGAttributeEvent`，六个参数：

| 参数 | 含义 |
|------|------|
| `EffectInstigator` | 效果的触发者（谁按下了攻击键） |
| `EffectCauser` | 效果的物理来源（那颗火球是哪个 Actor） |
| `EffectSpec` | GE 运行时实例指针 |
| `EffectMagnitude` | 此次修改的数值幅度 |
| `OldValue` / `NewValue` | 属性变化前后的值 |

**FGameplayEffectSpec** 是 `UGameplayEffect` 资产的运行时实例。GE 资产不可变（immutable），但需要在运行时决定倍率、等级、攻击者等信息——这些动态数据存在 Spec 里。

核心成员：`Def`（指向 GE CDO 的只读指针）、`Modifiers`（已计算好幅度的修改器）、`EffectContext`（谁发起，在哪发生）、`Duration`/`Period`、`CapturedSourceTags`/`CapturedTargetTags`、`SetByCallerTagMagnitudes`。

**类比**：`UGameplayEffect`（CDO）= 模具，`FGameplayEffectSpec` = 用模具浇铸出的具体产品，`Def` = 产品内部指向模具的指针。

### CDO vs UClass

| | CDO | UClass |
|----|-----|--------|
| 本质 | 一个默认实例 | 一个类型描述符 |
| 保存 | 属性默认值 | 类的结构信息（有哪些 UProperty、UFUNCTION） |
| 获取方式 | `GetDefault<T>()`、`GetMutableDefault<T>()` | `GetClass()`、`StaticClass()` |
| 可修改 | `GetMutableDefault` 可以改 | 不能，元数据编译期固定 |

**GAS 例子**：`PROPERTY_GETTER` 用 `StaticClass()` 拿 UClass 查结构（找属性的偏移位置），Spec 的 `Def` 指向 CDO 读规则数据（冷却多久、修改哪个属性）。

### UAttributeSet 五个钩子函数

| 钩子 | 调用时机 | 用途 |
|------|----------|------|
| `PreAttributeChange` | **任何**修改发生之前 | 钳制值 `Clamp(Health, 0, MaxHealth)` |
| `PostAttributeChange` | 任何修改发生后 | UI 刷新血条，不知道谁改的 |
| `PreGameplayEffectExecute` | GE 执行修改 Base 前 | 免疫判断、返回 false 阻止生效 |
| `PostGameplayEffectExecute` | GE 执行修改 Base 后 | 伤害响应、死亡判定、广播事件 |
| `PreAttributeBaseChange` | Base 值被修改前（有 Aggregator 时） | 钳制 Base 值 |

关键区分：`PostAttributeChange` 不提供 EffectSpec（不知道谁打的），适合 UI 刷新；`PostGameplayEffectExecute` 有完整 GE 信息，适合死亡判定和击杀奖励。

### GetLifetimeReplicatedProps

定义在 `UObject`（不是 `UAttributeSet`），`UAttributeSet` 通过继承链拿到它。这是 UE 网络属性复制注册函数——引擎网络初始化阶段调用，你往 `OutLifetimeProps` 里填条目，每条声明一个属性的复制规则：

```cpp
DOREPLIFETIME(ClassName, PropertyName, Condition, RepNotifyCondition);
//  ↑ 所属类       ↑ 属性      ↑ 复制给谁     ↑ 怎么触发 OnRep
```

`COND_None`（无条件复制）用于 AttributeSet 是正确的——属性值需要所有客户端可见。此声明必须与 `UPROPERTY(ReplicatedUsing = OnRep_Xxx)` 配对，缺一不可。

### REPNOTIFY_Always vs REPNOTIFY_OnChanged

定义在 `CoreNetTypes.h`：

```cpp
REPNOTIFY_OnChanged = 0,  // 只有收到的值 ≠ 本地当前值时，才调 OnRep
REPNOTIFY_Always = 1,     // 只要收到服务器同步，无条件调 OnRep
```

**GAS 必须用 Always 的原因**：客户端预测 GE 执行后，本地值已被修改。服务器确认时如果数值恰好相同（预测正确），`OnChanged` 会跳过 OnRep 调用。但 `GAMEPLAYATTRIBUTE_REPNOTIFY` 宏内部不只是设值，还负责清理预测标记——跳过 OnRep 意味着预测状态残留。

`OnChanged` 是性能优化，`Always` 是预测系统的正确性保证。

### `__super` vs `Super`

`__super` 是 MSVC 编译器专有关键字，表示直接父类名称。不可移植。UE 的 UHT 在 `.generated.h` 中生成 `typedef ParentClass Super;`，因此 UE 项目统一使用 `Super::`。在 UE 代码中绝不使用 `__super`。

</details>
