# 学习日志

记录每日有技术价值的提问与对应知识点，按日期归档，方便检索复习。

## 目录

- [2026-05-08](#ymrpg-2026-05-08) — UCLASS 说明符、UBT/UHT 构建系统、GA 生命周期与四项策略、FObjectInitializer、CurrentActorInfo 解耦、网络模型、GameplayTag 体系
- [2026-05-10](#ymrpg-2026-05-10) — 输入系统架构（三个 Handle 数组）、AbilitiesToAdd/Active 配置层、FGameplayAbilitySpec 全字段、CDO 陷阱、ProcessAbilityInput 调用链、DynamicAbilityTags 传递链、数据驱动 vs 继承、 WaitInputRelease 排查、UE 5.4→5.5 PredictionKey 迁移根因
- [2026-05-13](#ymrpg-2026-05-13) — ComboComponent 连招组件、AnimNotify vs AnimNotifyState、const 成员函数返回指针、ASC StopMontageIfCurrent 与 Montage 双通路、ProcessAbilityInput 三阶段、ENetMode 网络模式、bOrientRotationToMovement
- [2026-05-15](#ymrpg-2026-05-15) — Attribute 角色与功能、ATTRIBUTE_ACCESSORS 宏展开、FGameplayEffectSpec 运行时实例、CDO vs UClass、UAttributeSet 五个钩子函数、GetLifetimeReplicatedProps、REPNOTIFY_Always vs OnChanged、__super vs Super
- [2026-05-16](#ymrpg-2026-05-16) — AttributeSet 编译错误排查、`override` 误用、`Lifetime` 大小写、Attribute 体系完整架构、DamageExecution 伤害计算管线、GA_Melee 蓝图节点图分析
- [2026-05-18](#ymrpg-2026-05-18) — HealthComponent 实现与编译错误排查、UGameFrameworkComponent 继承体系、BlueprintAssignable、mutable、UFUNCTION 元数据、FGameplayEffectSpec 与 GE 的关系、SetNumericAttributeBase、Component Tag 归属原则、生命/死亡系统完整链路
- [2026-05-19](#ymrpg-2026-05-19) — AHUD 与 WBP/UUserWidget 架构关系、Sequence 流控制节点、统一事件源（GAS 事件总线 vs 观察者模式）、工厂模式在 UE/GAS 中的四层体现
- [2026-05-21](#ymrpg-2026-05-21) — UMG 四层架构与 Slate 即时模式、UMG 核心类体系、UWidgetBlueprintGeneratedClass 动画查找与 `_INST` 后缀、BindWidget 元数据自动绑定、UI Tick 路径中的 FString→FText 字符串性能优化

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
<summary><b>2026-05-15</b></summary>

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

---

<details>
<summary><b>2026-05-16</b></summary>

### Attribute 体系完整架构分析

#### 四层模型

```
┌──────────────────────────┐
│ UYMRPGGameplayAbility    │  ← 技能层：定义四项策略、提供 ASC/Character 访问器
├──────────────────────────┤
│ AYMRPGCharacterBase      │  ← 宿主层：持有 ASC、CharacterSet、ComboComponent
│ 构造函数: CDO            │     BeginPlay: 授予技能
├──────────────────────────┤
│ UYMRPGCharacterAttributeSet │ ← 属性数据层：12个属性、5个委托、5个钩子
├──────────────────────────┤
│ UYMRPGAbilitySystemComponent│ ← 能力管理+输入路由层
└──────────────────────────┘
```

#### 各资源实例化时机与位置

| 资源 | 何时实例化 | 位置 | 方式 |
|------|-----------|------|------|
| ASC | Character CDO 构造 | `CreateDefaultSubobject` | 每个 Character 一个，Mixed 复制模式 |
| CharacterSet | Character CDO 构造 | `CreateDefaultSubobject` | 每个 Character 一个，12 属性全复制 |
| ComboComponent | Character CDO 构造 | `CreateDefaultSubobject` | 本地组件，不复制 |
| GA CDO | 引擎启动/类注册 | UHT/UBT | `GetDefaultObject<T>()` 读取，全局唯一 |
| GA 实例 | 首次激活时 | `InstancedPerActor` 策略 | ASC 内部管理，每个 Actor 一个持久实例 |
| `FGameplayAbilitySpec` | BeginPlay 循环中 | 栈上构造后传 GiveAbility | ASC 内部接管生命周期 |
| `FGameplayAbilitySpecHandle` | GiveAbility 返回 | 存于 AbilitiesToActive | 句柄，用于 TryActivateAbility |
| `FGameplayEffectSpec` | GE 被应用时 | ASC 内部创建 | 运行时临时对象，含 Context+Modifiers |

#### 服务器端：GE 修改 Health 的完整调用链

```
GE 被 ASC 应用
  ├─1. PreGameplayEffectExecute(Data)
  │     HealthBeforeAttributeChange  = GetHealth()     // 快照
  │     MaxHealthBeforeAttributeChange = GetMaxHealth()
  │     return true
  ├─2. GE 内部 Modifier 执行（修改 Health 值）
  ├─3. PreAttributeChange(Attribute, NewValue)          // 扩展点：值生效前修改
  ├─4. PreAttributeBaseChange(Attribute, NewValue)
  ├─5. 值写入 FGameplayAttributeData::CurrentValue
  ├─6. PostAttributeChange(Attribute, OldValue, NewValue)
  │     if MaxHealth 变了 && Health > 新MaxHealth:
  │       ASC->ApplyModToAttribute(Health, Override, 新MaxHealth)
  │     if bOutOfHealth && Health > 0: bOutOfHealth = false
  └─7. PostGameplayEffectExecute(Data)
        switch (被修改的属性):
          case Damage:
            Health = Clamp(Health - Damage, 0, MaxHealth)
            Damage = 0   // 元属性已消费
          case Health:   Clamp
          case MaxHealth: OnMaxHealthChanged.Broadcast(...)
          case Mana:     Clamp
          case MaxMana:  OnMaxManaChanged.Broadcast(...)
        统一尾部:
          if Health 变化: OnHealthChanged.Broadcast(...)
          if Health <= 0 && !bOutOfHealth: OnOutOfHealth.Broadcast(...)
          bOutOfHealth = (Health <= 0)
```

**关键设计点**：
- `Damage` 是元属性（`HideFromModifiers`），不直接被 GE 修改。由 GA/Execution 通过 `SetDamage()` 写入，PostGameplayEffectExecute 看到 Damage > 0 就扣到 Health 上，然后立刻清零——Damage 是"暂存中转站"
- `OnHealthChanged` 和 `OnOutOfHealth` 的广播在所有 case 处理完后统一执行，保证不因 case 顺序漏广播
- `HealthBeforeAttributeChange` 在 PreGameplayEffectExecute 中快照，确保后续多次修改 Health，旧值仍为 GE 执行前的那个值

#### 客户端端：属性复制的完整路径

```
服务器 Health 变化 → 网络复制 → 客户端收到
  └─ OnRep_Health(OldValue)
       ├─ GAMEPLAYATTRIBUTE_REPNOTIFY(...)  → 通知 ASC 缓存更新
       ├─ EstimatedMagnitude = CurrentHealth - OldValue.GetCurrentValue()
       ├─ OnHealthChanged.Broadcast(nullptr, nullptr, nullptr, Magnitude, Old, Current)
       ├─ if !bOutOfHealth && CurrentHealth <= 0:
       │     OnOutOfHealth.Broadcast(...)   // 首次死亡，无施法者
       └─ bOutOfHealth = (CurrentHealth <= 0)
```

**服务器 PostGameplayEffectExecute vs 客户端 OnRep 关键区别**：

| | 服务器 | 客户端 |
|---|---|---|
| 施法者信息 | EffectInstigator/Causer 有效 | 全部 nullptr |
| 触发时机 | GE 执行完成后 | 属性复制到达时 |
| 信息来源 | `FGameplayEffectModCallbackData` | `OldValue`（上次复制值） |

#### 输入 → 技能激活的完整路径

```
玩家按键
  → ASC->AbilityInputTagPressed(InputTag)
       遍历 ActivatableAbilities.Items，匹配 DynamicAbilityTags
       → InputPressedSpecHandles / InputHeldSpecHandles
  松开:
  → ASC->AbilityInputTagReleased(InputTag)
       → InputReleasedSpecHandles / 从 InputHeldSpecHandles 移除
  下一帧:
  → ASC->ProcessAbilityInput(DeltaTime)
       阶段一: PressedHandle → 未激活 → AbilitiesToActivate
                             → 已激活 → AbilitySpecInputPressed (RPC)
       阶段二: TryActivateAbility(Handle) → GAS 标准激活流程
       阶段三: ReleasedHandle → AbilitySpecInputReleased (RPC)
       阶段四: 三个数组清空

AbilitySpecInputPressed/Released 内部:
  InvokeReplicatedEvent(EventType, SpecHandle, PredictionKey)
    → 带 PredictionKey 的 RPC，保证客户端-服务器事件对应
```

#### 数据传递链

**GE 执行时的数据流**：
```
GE Spec(FGameplayEffectSpec)
  ├─ Context(FGameplayEffectContextHandle)
  │   ├─ GetInstigator() → 谁按下了攻击键
  │   ├─ GetEffectCauser() → 物理来源（火球/拳头/陷阱）
  │   └─ GetOriginalInstigator() → 连锁GE的最初来源
  ├─ Modifiers[] → 引擎执行 → EvaluatedData
  │   └─ Data.EvaluatedData.Attribute → 被修改的属性
  │   └─ Data.EvaluatedData.Magnitude → 修改量
  └─ 广播: FYMRPGAttributeEvent(Instigator, Causer, Spec*, Magnitude, Old, New)
```

**Ability 授予时的数据流**：
```
AbilitiesToAdd (TMap<FGameplayTag, TSubclassOf<UYMRPGGameplayAbility>>)
  Key: GameplayTag → 输入绑定用 + 写入 Spec.DynamicAbilityTags
  Value: TSubclassOf<>
    → GetDefaultObject<T>() 读 CDO（不创建实例）
    → FGameplayAbilitySpec(CDO, Level=1)
       Spec.SourceObject = this
       Spec.DynamicAbilityTags.AddTag(Tag)
    → GiveAbility(Spec) → FGameplayAbilitySpecHandle
```

### OnRep_Health 函数详解

```cpp
void OnRep_Health(const FGameplayAbilityData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UYMRPGCharacterAttributeSet, Health, OldValue);
    // ↑ 通知 ASC 属性已复制，触发 OnGameplayAttributeValueChange 回调

    const float EstimatedMagnitude = CurrentHealth - OldValue.GetCurrentValue();
    // ↑ 估算变化量。叫 Estimated 是因为客户端可能被预测修改过

    OnHealthChanged.Broadcast(nullptr,nullptr,nullptr, Magnitude, Old, Current);
    // ↑ 前三个 nullptr —— OnRep 没有施法者信息

    if (!bOutOfHealth && CurrentHealth <= 0.f)   // ← !bOutOfHealth 保证只触发一次
        OnOutOfHealth.Broadcast(...);
    bOutOfHealth = CurrentHealth <= 0.f;
}
```

### UYMRPGDamageExecution — 伤害计算管线

`UGameplayEffectExecutionCalculation` 是 GAS 中最灵活的属性修改方式，与简单 Modifier / MMC 对比：

| 途径 | 能力 | 场景 |
|------|------|------|
| GE Modifier | 固定系数/倍率直接改属性 | 回血10点 |
| MMC | 自定义公式算修改量 | 伤害=攻击力×1.5 |
| ExecutionCalculation | 自由捕获多属性、读Tag、写多属性 | 完整伤害公式 |

**核心结构 — FDamageStatics**：

```cpp
struct FDamageStatics
{
    FGameplayEffectAttributeCaptureDefinition BaseDamageDef;

    FDamageStatics()
    {
        BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(
            UYMRPGCharacterAttributeSet::GetDamageAttribute(),
            EGameplayEffectAttributeCaptureSource::Source,  // 从施法者身上捕获
            true  // Snapshot=true: GE Spec创建时快照，而非应用时再读
        );
    }
};
```

三个参数含义：
- **捕获目标属性**：攻击者的 Damage 值
- **`CaptureSource::Source`**：从 GE 施法者（攻击者）身上读；`::Target` 即从目标读
- **`Snapshot = true`**：GE Spec 创建时就快照 —— 弓箭射出时记录攻击力，飞2秒后命中仍用射出时的值，不受中途虚弱影响

**构造函数**：`RelevantAttributesToCapture.Add(DamageStatics().BaseDamageDef)` 注册要捕获的属性。

**Execute_Implementation**（`#if WITH_SERVER_CODE` 仅服务器执行）：
1. 获取 `FGameplayEffectSpec`，提取 SourceTags / TargetTags
2. `AttemptCalculateCapturedAttributeMagnitude` 读出已捕获属性值
3. `DamageDone = Max(BaseDamage, 0.f)` 保底非负
4. `AddOutputModifier` → 写到目标的 Damage 元属性

**与 AttributeSet 的完整流水线**：

```
GE_Attack 应用
  → PreGameplayEffectExecute (快照)
  → Execute_Implementation (捕获攻击者Damage → 写目标Damage)
  → PostGameplayEffectExecute (Damage > 0 → Health = Clamp(Health - Damage))
  → OnHealthChanged.Broadcast → 复制 → 客户端 OnRep_Health
```

**为什么中间加 Damage 元属性**：
- 伤害处理统一入口：不管近战/远程/DOT/AOE，都走 `case Damage` 分支
- 伤害可被拦截：护盾、免疫在 PostGameplayEffectExecute 集中处理
- `HideFromModifiers` 强制所有伤害走 Execution 通道

**当前局限与扩展**：目前只捕获一个属性（Damage）。完整版应捕获 Attack / Defense / ArmorPen / CritRate / ElementTag，在 Execute_Implementation 中合并计算。

### GE 资产侧绑定

GE 资产不使用 `Modifiers` 数组，而是在 `Execution` 区域指定 `UYMRPGDamageExecution` 类。GE 应用时自动找到 Execution 类、创建实例、调用 `Execute_Implementation`。

### 蓝图中添加注释

- 选中节点 → 按 **C** → 创建注释框包裹节点，可修改标题和颜色
- 右键空白处 → 搜索 "Comment" → 独立注释节点
- Details 面板填写 "描述" (Description) → 节点上方显示灰色小字

### GA_Melee 蓝图节点图完整分析

蓝图事件图实现了完整的近战攻击流程，分六个阶段：

**阶段一：代价/冷却检查**
```
K2_ActivateAbility → K2_CheckAbilityCost → K2_CheckAbilityCooldown → BooleanAND
  ├─ True → CommitAbility → SelectMontage → PlayMontageAndWait
  └─ False → K2_EndAbility (直接终止)
```
`K2_CheckAbilityCost` / `K2_CheckAbilityCooldown` 是 UE GAS 内置蓝图函数，读 GE CDO 上的 Cost/Cooldown 配置。蓝图显式写是因为覆盖了 `K2_ActivateAbility` 事件，不会自动走 C++ 的 `ActivateAbility` 默认流程。

**阶段二：Montage 播放**
`SelectMontage`（蓝图自定义纯函数，根据地面/空中/奔跑状态选 Montage）→ `PlayMontageAndWait`。四个结束引脚（OnCompleted/OnBlendOut/OnInterrupted/OnCancelled）全部接 `K2_EndAbility`。

**阶段三：服务器端碰撞检测**
`K2_HasAuthority` → True → `CapsuleTraceSingleForObjects`。Trace 只在服务器跑 —— 客户端播动画（预测），服务器判定打中没打中。MeleeTraceLogic（蓝图纯函数）提供 StartTrace / EndTrace / IgnoreActors / YMRPGCharacterBase。

**阶段四：命中验证（遮挡检测）**
命中后通过第二次 `LineTraceMulti` 检查目标是否被墙壁/障碍遮挡。两段 Trace 设计：第一次找 Pawn，第二次验证视线通畅——防止穿墙打人。

**阶段五：三路并行输出（ExecutionSequence）**
- then_0：距离>100 → `ApplyRootMotionConstantForce`（近身吸附）。`NormalizeToRange(Distance, 0, MaxDistance)` 把距离映射到 [0,1]，越远推力越弱
- then_1：`MakeEffectContext` → `EffectContextAddHitResult` → `BP_ApplyGameplayEffectToTarget(TargetASC, GE_Class)` → `K2_ExecuteGameplayCueWithParams(GameplayCue.Character.Hit)`
- then_2：`PlaySoundAtLocation`（Shinbi技能音效）

**阶段六：CustomEvent "Melee Impact"**
同样调用 `PlaySoundAtLocation`，但有一个调用节点报 `ErrorMsg="Could not find Melee Impact"` —— 蓝图内函数名错配，待修复 Bug。

**蓝图与 C++ 衔接点**：
| 蓝图做 | C++做 | 衔接 |
|--------|--------|------|
| 选 Montage | ComboComponent 管理连招索引 | 蓝图 SelectMontage 可改为从 ComboComponent 读 |
| GE 资产指定 | UYMRPGDamageExecution 计算伤害 | 蓝图 GE_Melee 指定 ExecutionClass |
| Trace 碰撞 | — | 纯蓝图实现 |
| 结束技能 | PostGameplayEffectExecute 广播死亡 | C++ AttributeSet 侧事件未回传蓝图 |

</details>

---

<details>
<summary><b>2026-05-18</b></summary>

### YMRPGHealthComponent 生命周期组件

实现了一个独立于 Character 的 HealthComponent，负责：

- **死亡状态机**：`NotDead → DeathStarted → DeathFinished`（不可逆三态）
- **属性查询 API**：GetHealth / GetMaxHealth / GetMana / GetMaxMana / GetHealthPercent / GetManaPercent / IsDeadOrDying
- **蓝图事件广播**：OnDeathStarted、OnDeathFinished、OnHealthChanged、OnMaxHealthChanged、OnManaChanged、OnMaxManaChanged
- **ASC Tag 同步**：死亡时自动管理 `Status.Death.Dying` / `Status.Death.Dead` 的 LooseGameplayTag

**组件基类改造**：`YMRPGComponentBase` 从 `UActorComponent` 改为继承 `UGameFrameworkComponent`（ModularGameplay 插件），所有子类构造函数改为接收 `const FObjectInitializer&` 参数。

### UGameFrameworkComponent 继承体系

`UGameFrameworkComponent` 本身只提供三个便捷访问器：`GetGameInstance<T>()`、`HasAuthority()`、`GetWorldTimerManager()`，以及一个 `TComponentIterator<T>`。真正有价值的是它的四个子类：

| 子类 | 挂载到 | 提供 |
|------|--------|------|
| `UPawnComponent` | Pawn | `GetPawn<T>()`, `GetPlayerState<T>()`, `GetController<T>()` |
| `UControllerComponent` | Controller | `GetController<T>()`, `GetViewTarget<T>()`, `PlayerTick()`, `ReceivedPlayer()` |
| `UGameStateComponent` | GameState | `GetGameState<T>()`, `GetGameMode<T>()`, `HandleMatchHasStarted/Ended()` |
| `UPlayerStateComponent` | PlayerState | `GetPlayerState<T>()`, `GetPawn<T>()`, `Reset()`, `CopyProperties()` |

**99% 的情况下不直接继承 UGameFrameworkComponent**，而是继承对应 Owner 类型的子类。Lyra 里 `ULyraHealthComponent : UPawnComponent`、`ULyraHeroComponent : UPawnComponent`。直接继承仅用于不依赖特定 Owner 类型的通用组件。

配套接口 `IGameFrameworkInitStateInterface` 解决多 Component 间的初始化顺序问题——配合 `UGameFrameworkComponentManager` 实现有序初始化与依赖就绪检查。

### BlueprintAssignable 委托说明符

标记 Multicast Delegate 使其在蓝图中可作为事件节点被绑定/解绑。四个说明符的分工：

| 说明符 | 蓝图可做什么 |
|--------|-------------|
| `BlueprintAssignable` | 绑定/解绑事件处理函数 |
| `BlueprintCallable` | 调用（广播）委托（仅 Dynamic Multicast） |
| 两者同时用 | 蓝图既能绑定也能广播 |

通常 `BlueprintAssignable` 和 `BlueprintCallable` 一起用——前者让蓝图能**监听**事件，后者让 C++/蓝图能**广播**事件。

### mutable 关键字

C++ 关键字，突破 `const` 约束——允许被标记的成员变量在 `const` 成员函数中被修改。三类典型场景：缓存/延迟计算、统计/调试计数、`mutable std::mutex`（const 方法的线程安全）。现阶段 GAS 上层玩法逻辑中很少用到，知道"打破 const 用的"即够。

### UFUNCTION 元数据

**`BlueprintCallable` vs `BlueprintPure`**：

| | Pure（纯函数） | Callable（非纯函数） |
|---|---|---|
| 执行引脚 | 无 | 有（白色箭头） |
| 调用时机 | 每次用到返回值都重新算 | 只沿执行流触发 |
| 有无副作用 | 无 | 可以修改状态 |

四行完全等价：`BlueprintPure` / `BlueprintPure = true` / `BlueprintCallable, BlueprintPure = true` / `BlueprintCallable, BlueprintPure`。惯例：纯函数写 `BlueprintPure`，非纯函数写 `BlueprintCallable`。

**`Category = "YMRPG|Health"`**：`|` 是层级分隔符，在蓝图右键菜单中形成 YMRPG → Health 的子类目。纯 UI 组织，不影响逻辑。

**`Meta = (ExpandBoolAsExecs = "ReturnValue")`**：返回 bool 的函数，将单条返回值引脚展开为 True / False 两条执行输出线，节省一个 `Branch` 节点。

### FGameplayEffectSpec 与 GE 的引用关系

`FGameplayEffectSpec` 通过 `TObjectPtr<const UGameplayEffect> Def` 直接持有源 GE 资产指针，**不需要 ASC 就能找到 GE**。需要 ASC 的是另一层：

| 场景 | 需要什么 |
|------|---------|
| 读 GE 静态数据（Modifier 列表、Duration Policy、Tag） | 仅 `Spec.Def` |
| 计算实际 Modifier 数值（属性快照、MMC） | `Spec` + **Target ASC** |
| 计算实际 Duration（属性基准持续） | `Spec` + Source ASC + Target ASC |

`Def` 给配置长什么样，ASC 给实际算出来是多少。

### SetNumericAttributeBase

直接写入属性的 **Base 值**（即 GE Modifier 聚合前的底层值）。属性最终值 = Base值 + GE Modifiers 聚合结果。SetNumericAttributeBase 写的是前半部分。

| 函数 | 读/写 | 含义 |
|------|-------|------|
| `SetNumericAttributeBase` | 写 | 直接设 Base 值 |
| `GetNumericAttributeBase` | 读 | 读 Base 值（排除 GE Modifier） |
| `GetNumericAttribute` | 读 | 读最终值（Base + GE Modifier 聚合后） |

**不是常规玩法手段**——常规改属性应靠 GE。`SetNumericAttributeBase` 主要用于属性初始化阶段（AttributeSetInitter 从 DataTable 写入初始值）。平常写 GA/GE 逻辑不应直接调它。

### Component 中 Tag 的归属原则

**不是所有 Tag 都走 ASC**。判断标准：这个 Tag 是否需要被 GAS 框架层感知。

| Tag 用途 | 放哪里 | 例子 |
|----------|--------|------|
| 阻止 GA 激活 | ASC | `State.Dead`、`State.Stunned` |
| GE ApplicationTagRequirements | ASC | `State.ShieldActive` |
| GE 自动添加/移除 | ASC（通过 GE GrantedTags） | Buff 标记 |
| 其他 Component 需查询 Actor 状态 | ASC | 动画蓝图判断 `State.InAir` |
| 纯组件内部状态、数据标记 | 组件自己的 `FGameplayTagContainer` | NumberPop 样式选择、连招内部标记 |

ASC 上的 Tag 有引擎级开销：Add/Remove LooseTag 会触发 GA 激活条件重新评估、GE 生效条件重新检查、网络复制。组件内部 Tag 只是普通变量，无这些副作用。

### HealthComponent 编译错误排查（四个问题）

**错误 1 — API 宏拼写错误**：`YMORPG_API` 应为 `YMRPG_API`。字母顺序错了，链接阶段找不到导出符号。

**错误 2 — OnRep_DeathState 缺少 UFUNCTION()**：RepNotify 函数必须标记 `UFUNCTION()`，UHT 靠此宏识别。光写 `virtual` 不行。正确写法：`UFUNCTION() virtual void OnRep_DeathState(EYMRPGDeathState OldValue);`

**错误 3 — 委托签名不匹配**：`FYMRPGAttributeEvent` 的第三个参数是 `const FGameplayEffectSpec*`（指针），但 HealthComponent 的 5 个 Handler（HandleHealthChanged 等）全写成了 `const FGameplayEffectSpec&`（引用）。`AddUObject` 绑定时签名对不上，编译报错。

**错误 4 — 比较运算符误用作赋值**：构造函数里 `DeathState == EYMRPGDeathState::NotDead;` 是 `==` 而非 `=`，这是一个无副作用的比较表达式，DeathState 从未被初始化。

### 生命/死亡系统完整链路

#### 伤害触发死亡的调用链

```
GE 应用
  → PreGameplayEffectExecute（快照 HealthBeforeAttributeChange）
  → Execute_Implementation（捕获攻击者属性 → 写入目标 Damage）
  → 引擎执行 Modifier → PostGameplayEffectExecute
      ├─ case Damage: Health = Clamp(Health - Damage, 0, MaxHealth)
      ├─ if Health 变化: OnHealthChanged.Broadcast(Instigator, Causer, &Spec, ...)
      └─ if Health ≤ 0 && !bOutOfHealth: OnOutOfHealth.Broadcast(...)
  → HealthComponent::HandleOutOfHealth
      └─ #if WITH_SERVER_CODE  // 仅服务端
         StartDeath()
           ├─ DeathState = DeathStarted
           ├─ ASC->SetLooseGameplayTagCount(Status.Death.Dying, 1)
           ├─ OnDeathStarted.Broadcast(Owner)
           └─ Owner->ForceNetUpdate()
```

#### 网络同步：两条并行通道

**通道 A — DeathState 直接复制**：

```
[Server] StartDeath() → DeathState = DeathStarted ──复制──→ [Client] OnRep_DeathState
  → 快照 NewValue → 恢复 OldValue → 重放 StartDeath()
  → Tag/Event 在客户端以相同顺序执行
```

`OnRep_DeathState` 采用"重放"而非"赋值"设计——先恢复旧值，再调用 StartDeath/FinishDeath，保证客户端经历和服务端完全一样的 Tag 和 Broadcast 顺序。

**通道 B — AttributeSet 属性复制**：

```
[Server] PostGameplayEffectExecute → [Client] OnRep_Health
  → OnHealthChanged.Broadcast(nullptr, nullptr, nullptr, ...)
  → if Health ≤ 0: OnOutOfHealth.Broadcast(nullptr, nullptr, nullptr, ...)
```

客户端 OnRep 的三个指针参数全为 nullptr（属性复制不传上下文）。因此 HealthComponent 的 HandleOutOfHealth 用 `#if WITH_SERVER_CODE` 守卫——客户端不通过属性变化触发死亡，死亡状态完全由通道 A 驱动。

**两通道协调**：`WITH_SERVER_CODE` 是关键保险——无论 OnRep_DeathState 和 OnRep_Health 谁先到达客户端，都能正确收敛，不会重复触发死亡。

#### DeathState 状态机

```
NotDead ──StartDeath()──→ DeathStarted ──FinishDeath()──→ DeathFinished
  ↑                         │
  └────── 不可逆 ───────────┘

StartDeath() 守卫: DeathState != NotDead → return
FinishDeath() 守卫: DeathState != DeathStarted → return
```

每个状态对应的 ASC Tag：

| 状态 | Status.Death.Dying | Status.Death.Dead |
|------|--------------------|-------------------|
| NotDead | 0 | 0 |
| DeathStarted | 1 | 0 |
| DeathFinished | 0 | 1 |

#### 其他发现

**PostGameplayEffectExecute 中存在一个潜在问题**：MaxMana 变化时广播 `OldValue` 传了 `MaxHealthBeforeAttributeChange`（应为 Mana 系自己的快照）。PreGameplayEffectExecute 只快照了 Health 和 MaxHealth，没有快照 Mana。当前影响不大（委托主要给 UI 做动画），但后续做精确蓝量变化反馈需修复。

### Game Feature

UE 的模块化功能插件系统，核心解决"功能按需动态加载/卸载"。每个 Game Feature 是一个 `.uplugin`，通过 `GameFeatureData` DataAsset 声明 Actions（`AddComponent`、`AddDataRegistry` 等），由 `GameFeaturesSubsystem` 统一管理生命周期。Lyra 以此将每个游戏模式拆为独立插件，Fortnite 以此实现限时模式。

**当前项目不需要**——单玩法学习项目无此复杂度。但 GAS 的 GA/GE/AttributeSet 注册机制与 Game Feature 加载入口完全通用，学会 GAS 后回头理解它很快。

</details>

---

<details>
<summary><b>2026-05-19</b></summary>

### AHUD 与 WBP（UUserWidget）的架构关系

**层级定位**：

```
GameMode (指定 HUDClass)
  └─ PlayerController (创建并持有 HUD)
       └─ AHUD (AActor，由框架管理生命周期)
            └─ UUserWidget (WBP — 实际的 UMG UI 内容)
```

- **`AHUD`** 是 `AActor`，受 GameMode/PlayerController 管控，存在于游戏世界中，参与 Tick。是游戏框架层的 UI 管理者。
- **WBP（Widget Blueprint）** 是 `UUserWidget` 的蓝图子类，属于 UMG 框架层。实际的 UI 控件树（按钮、血条、文字、动画等）都在 UUserWidget 里。

**职责分工**：

| | AHUD | UUserWidget (WBP) |
|---|---|---|
| 是什么 | AActor，游戏框架层 | UObject，UI 框架层 |
| 生命周期 | 由 GameMode 在游戏开始时生成 | 由代码/蓝图动态 Create / AddToViewport |
| 渲染方式 | 传统 Canvas 绘制（`DrawHUD()`） | UMG 控件树，Slate 底层渲染 |
| 典型用途 | UI 容器/入口，管理 Widget 生命周期 | 具体 UI 面板：主界面、血条、背包等 |

**现代 UE5 的正确模式**：AHUD 趋向于做薄容器——在 `BeginPlay()` 中 `CreateWidget` → `AddToViewport` 挂载主 UI，持有子 Widget 引用。具体的 UI 逻辑（血量绑定、按钮事件、动画）全部下沉到 WBP 中。项目中 `AYMRPGHUD` 当前为空壳，后续可在此创建 `WBP_GameMainUI`。

**访问链路**：`PlayerController->GetHUD()` → `Cast<AYMRPGHUD>()` → HUD 持有的主 Widget。项目中 `AYMRPGPlayerController::GetYMRPGHUD()` 已搭好该链路。

### Sequence 流控制节点

将一个执行脉冲按顺序拆成多个分支：

```
事件触发 ──→ Sequence ──→ Then 0（先执行，完成后再走 Then 1）
                        ├──→ Then 1
                        └──→ Then 2
```

**典型场景**：`BeginPlay` 初始化 — Then 0 初始化 AttributeSet → Then 1 绑定 UI → Then 2 播放动画 → Then 3 启用输入。

**关键注意**：若某个 Then 分支使用了 **`Delay`**（Latent 节点），后续 Then **不会等待它**——Delay 之后逻辑异步执行，Sequence 不暂停。需要"等某件事完成再继续"应用 `Do N` + 回调，或 StateTree / AbilityTask。

**与相关节点区别**：`Branch`（条件选一）、`FlipFlop`（交替走 AB）、`DoOnce`（仅一次）、`MultiGate`（顺序/随机走一，可重置）。

### 统一事件源 — GAS 核心设计思想

**定义**：`UAbilitySystemComponent`（ASC）就是统一事件源——一个中心化的事件调度枢纽。所有和玩法相关的状态变化（属性改变、标签变化、技能激活/结束、GE 应用/移除），全部经由 ASC 这个单一入口分发。

**在项目中的四层体现**：

**1. 统一委托签名 — AttributeSet 层**：
```cpp
// YMRPGAttributeSet.h — 所有属性变化用同一套六参数签名
DECLARE_MULTICAST_DELEGATE_SixParams(
    FYMRPGAttributeEvent,
    AActor* /*EffectInstigator*/,
    AActor* /*EffectCauser*/,
    const FGameplayEffectSpec* /*EffectSpec*/,
    float /*EffectMagnitude*/,
    float /*OldValue*/,
    float /*NewValue*/
);
```

**2. ASC 作为事件总线 — 绑定与分发**：
```cpp
// HealthComponent 向 ASC 管理的 AttributeSet 注册回调
HealthSet->OnHealthChanged.AddUObject(this, &UYMRPGHealthComponent::HandleHealthChanged);
// 监听者不需要知道"谁造成了变化"——ASC 自动把 Instigator/Causer/Spec 塞进回调
```

**3. GameplayTag 作为事件语言**：
```cpp
// 通过 ASC 的标签系统广播状态，而非直接通知各个监听者
ASC->SetLooseGameplayTagCount(Status_Death_Dying, 1);  // 濒死
ASC->SetLooseGameplayTagCount(Status_Death_Dead, 1);   // 确认死亡
```
UI、AI、技能系统无需持有 HealthComponent 引用，只需监听 ASC 上的 Tag 变化。

**4. 输入也汇入统一事件源**：
```cpp
// 输入不直接激活技能，而是把 Tag 交给 ASC 统一匹配和调度
void AbilityInputTagPressed(const FGameplayTag& InputTag)
{
    for (auto& Spec : ActivatableAbilities.Items)
        if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
            InputPressedSpecHandles.AddUnique(Spec.Handle);
}
```

**数据流全景**：
```
输入(Tag) ─┐
GE 应用 ────┤
                  ▼
           ┌──────────────┐
           │ ASC (统一事件源)│
           │ ├─ AttributeSet │
           │ ├─ GameplayTag  │
           │ ├─ GameplayEvent│
           │ └─ AbilitySpec  │
           └──────┬─────────┘
                  │
     ┌────────────┼──────────────┐
     ▼            ▼              ▼
 HealthComp    UI(WBP)       ComboComp
```

#### 与观察者模式的本质区别

| | 经典观察者模式 | GAS 统一事件源 |
|---|---|---|
| 关系 | 一对多，Subject 持有 Observer 列表 | 多对多，所有事件经过 ASC "总线" |
| 订阅方式 | `subject.AddObserver(this)` | 按 Tag 订阅：`ASC->RegisterGameplayTagEvent(Tag)` |
| 耦合度 | Observer 需知道具体 Subject 是谁 | 监听者只需知道 ASC + Tag |
| 事件格式 | 每个 Subject 自定义签名 | 统一签名（`FYMRPGAttributeEvent` 六参数） |
| 过滤机制 | 无内置过滤，Observer 自己判断 | Tag 天然就是过滤器，按语义订阅 |
| 网络 | 需自行处理 | ASC 内置 Prediction Key、ReplicatedEvent |

**核心差异：从"找对象"到"听广播"**。观察者模式需要知道向哪个对象注册；GAS 只需要知道 ASC 和一些 Tag，通过 Tag 抽象实现生产者和消费者完全解耦。这更像**事件总线（Event Bus）**或**中介者（Mediator）**模式。

**设计启发**：GameplayTag 同时承担三重角色——事件类型、过滤条件、状态标记。一套 Tag 系统避免为"属性变化通知""状态查询""技能触发条件"设计三套 API。

### 工厂模式在 UE/GAS 中的四层体现

**核心思想**：把"创建哪个具体类"的决策从调用方推迟到子类或配置层。调用方只依赖抽象接口，不关心具体实现。

**第一层 — UClass 作为元工厂（反射系统根基）**：
每个 `UClass` 本身就是该类型的工厂对象，`NewObject<T>()` / `SpawnActor<T>()` / `CreateDefaultSubobject<T>()` 底层都走到 `UClass`，不需要 switch-case。

**第二层 — `TSubclassOf<T>` 参数化工厂（将类型选择暴露给蓝图）**：
```cpp
// C++ 只约定"我需要一个 UUserWidget"，具体哪个 WBP 由蓝图配置决定
UPROPERTY(EditAnywhere)
TSubclassOf<UUserWidget> MobileControlsWidgetClass;

// 工厂调用，不关心具体类型
MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);
```
同理：`EnemyClass` → `SpawnActor<ACombatEnemy>()`、`CharacterClass` → `SpawnActor`。策划/美术在蓝图或 DataAsset 中配置，C++ 零改动。

**第三层 — GameMode 作为教科书级工厂方法模式**：
```cpp
AYMRPGGameMode::AYMRPGGameMode()
{
    DefaultPawnClass      = AYMRPGCharacter::StaticClass();
    PlayerStateClass      = AYMRPGPlayerState::StaticClass();
    HUDClass              = AYMRPGHUD::StaticClass();
    PlayerControllerClass = AYMRPGPlayerController::StaticClass();
    GameStateClass        = AYMRPGGameState::StaticClass();
}
```
父类 `AGameModeBase` 定义"需要创建什么"的框架流程，子类通过覆盖这五个属性决定"具体创建哪个类"。引擎调用 `SpawnDefaultPawnFor()` 时不需知道 Pawn 具体类型。

**第四层 — `GiveAbility` 作为技能实例工厂**：
```cpp
// ASC 是技能实例的工厂：类 → CDO（原型）→ Spec → 运行时实例
UYMRPGGameplayAbility* AbilityCDO = AbilityClass->GetDefaultObject<UYMRPGGameplayAbility>();
FGameplayAbilitySpec AbilitySpec(AbilityCDO, 1);
const FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(AbilitySpec);
```
CDO（Class Default Object）在此充当**原型模式（Prototype）**角色——用默认对象作模板复制出运行时实例，避免每次创建重新计算默认值。这是"工厂 + 原型"的混合设计。

**四层归纳**：

| 层级 | 工厂角色 | 项目体现 |
|---|---|---|
| UClass 元工厂 | `UClass` 本身 | 所有 `NewObject` / `SpawnActor` 底层 |
| `TSubclassOf` 参数化 | 类型选择暴露为可配置属性 | `MobileControlsWidgetClass`、`EnemyClass` |
| GameMode 工厂方法 | GameMode 决定框架类 | `DefaultPawnClass`、`HUDClass` 等五个属性 |
| ASC.GiveAbility | 从类+配置创建技能实例 | `GiveAbility(AbilitySpec)` |

**UE 不直接用 `new` 或手写工厂的原因**：
- **反射 + GC**：UE 对象必须走自己的分配器才能被垃圾回收追踪
- **蓝图可配置性**：`TSubclassOf` 让蓝图直接选类，C++ 不需为每个子类写 if-else
- **CDO（原型模式）**：`GetDefaultObject<T>()` 从类获取默认实例，创建时直接复制

</details>

---

<details>
<summary><b>2026-05-21</b></summary>

### UMG 四层架构与 Slate 即时模式

UMG 本质是一个四层栈，从上到下：

```
Widget Blueprint (WBP)        ← 设计师拖拽操作的面板
UUserWidget / UWidget         ← UObject 封装层，支持反射、GC、蓝图
SObjectWidget                 ← 桥接层，把 UObject 和纯 C++ SWidget 绑定
SWidget / Slate Core          ← 纯 C++ 即时模式 UI 框架，无 UObject 开销
```

**Slate 即时模式 vs UMG 保留模式**：

| | Slate (原生) | UMG |
|---|---|---|
| 编程模型 | 即时模式 — 每帧重新描述 UI | 保留模式 — Widget Tree 持久存在 |
| 状态管理 | 状态在控件外，每帧传入 | 状态在控件内（UProperty），自维护 |
| 性能 | 更高（无 UObject/蓝图开销） | 较低（反射 + GC + 蓝图 VM 桥接） |
| 谁写 | C++ 程序员 | 设计师/策划（蓝图拖拽） |

Slate 的理念是"每帧我告诉你界面长什么样"；UMG 的理念是"界面就在这里，修改属性时自己更新"。

### UMG 核心类体系

**继承链（5 个主干节点）**：

```
UObject
  └─ UVisual              ← 所有 UMG 可见元素的根：Widget 和 Slot 的共同基类
       ├─ UWidget         ← 所有控件的抽象基类
       │    ├─ UPanelWidget     ← 多子控件容器（Slots 数组）
       │    ├─ UContentWidget   ← 单子控件容器
       │    ├─ UTextLayoutWidget← 文本类基类
       │    ├─ UUserWidget      ← WBP 的 C++ 父类（拥有 WidgetTree）
       │    └─ ... 叶控件（UImage, UProgressBar 等）
       └─ UPanelSlot       ← 布局槽位（控制子控件在父容器中的位置/大小）
```

**UWidget**：所有控件的基类。每个 UWidget 内部持有 `TSharedPtr<SWidget> MyWidget`。核心职责：属性同步（`SynchronizeProperties`）、重建 Slate 树（`RebuildWidget`）、事件分发。

**UPanelWidget**：多子控件容器。内部维护 `TArray<UPanelSlot*> Slots`。核心接口：`AddChild/RemoveChild/GetChildAt/ClearChildren`。每个 Panel 类型都有对应的 Slot 子类：

| Panel | Slot 子类 | 存储什么 |
|---|---|---|
| CanvasPanel | UCanvasPanelSlot | Anchors, Position, Size, ZOrder |
| VerticalBox | UVerticalBoxSlot | Size（填充比例） |
| HorizontalBox | UHorizontalBoxSlot | Size |
| Overlay | UOverlaySlot | Padding, Alignment |
| GridPanel | UGridPanelSlot | Row, Column, RowSpan, ColumnSpan |

**关键设计**：Slot 对象被 UWidget 持有（而非 Panel），所以子控件换父容器时布局属性跟着走。

**UContentWidget**：逻辑上是 UPanelWidget 的子类，但强约束只有一个子控件。Button、Border、CheckBox、ScaleBox 等全部继承它。这些控件自身是"修饰器"——给唯一的子控件附加交互或视觉效果。

**UUserWidget**：Blueprint 创建 UI 时的 C++ 父类。特殊在它自己拥有一套控件树：`UWidgetTree* WidgetTree`。生命周期钩子：`NativePreConstruct()`（编辑器预览）→ `NativeConstruct()`（运行时初始化）→ `NativeTick()`（每帧）→ `NativeDestruct()`（清理）。

**UWidgetTree**：轻量管理类，负责序列化控件树、`ForEachWidget` 遍历、`FindWidget<T>()` 查找。

**Slate 层的对应关系**：

| UMG (UObject) | Slate (纯C++) | 子控件数 |
|---|---|---|
| 直接继承 UWidget 的叶控件 | `SLeafWidget` | 0 |
| UContentWidget (Button/Border等) | `SCompoundWidget` | 1 |
| UPanelWidget (Canvas/VBox/Overlay等) | `SPanel` | 多 |

**SObjectWidget 桥接**：当 UWidget 的 `TakeWidget()` 被调用时，不是创建赤裸裸的 SButton/STextBlock，而是在外面套一层 SObjectWidget 壳。这层壳让 Slate 的焦点系统、事件路由能反向找到对应的 UObject。

### UWidgetBlueprintGeneratedClass 与动画查找

**WBP 编译产物**：编译每个 Widget Blueprint 时生成一个 C++ 类，名字是 `WBP名_C`。这个类的类型是 `UWidgetBlueprintGeneratedClass`（继承链：`UWidgetGeneratedClass → UBlueprintGeneratedClass → UClass`）。

**Animations 数组**：编译时 Widget Blueprint 的动画轨道全部序列化到这个数组中。每个动画轨道对应一个 `UWidgetAnimation` 对象。这些是**模板（Template）**，不是运行时实例——`PlayAnimation(Anim)` 时 UUserWidget 内部用模板创建动画实例来播放。

**`_INST` 后缀**：动画原名 `"Anim"` 被存储为 `"Anim_INST"`——表示这是一个可实例化的模板对象。查找时需要拼 `(InAnimName + "_INST")`。

**GetNameWidgetAnimation 空指针风险**：`FindByPredicate` 找不到时返回 `nullptr`，直接 `return *Animation;` 是空指针解引用。安全写法：`return Animation ? *Animation : nullptr;`

### BindWidget / BindWidgetAnim 元数据

UMG 的自动绑定机制——按名称将蓝图控件/动画与 C++ 成员变量关联。

```cpp
UPROPERTY(meta = (BindWidget))          // 按名绑定控件
UTextBlock* Health;

UPROPERTY(meta = (BindWidgetAnim))      // 按名绑定动画
UWidgetAnimation* FadeIn;
```

**匹配规则**：变量名必须与蓝图中控件/动画名完全一致，类型也必须兼容。引擎在 `NativeConstruct()` 之前自动完成匹配。

**OptionalWidget**：默认是 Required（找不到报错并返回 null）。标记为可选后，子蓝图不放置该控件也不报错：
```cpp
UPROPERTY(meta = (BindWidget, OptionalWidget = true))
UTextBlock* Health;  // 子类可以不创建此控件
```

**与手动 FindWidget 对比**：`BindWidget` 把查找开销压在初始化阶段一次完成，后续访问就是直接指针解引用，零开销。而且蓝图中改名时编译报错，编译期安全。

### UI Tick 路径中的字符串性能优化

**问题**：在 `NativeTick` 中高频更新 UI 文本，典型写法产生大量临时对象：

```cpp
// 每帧 4 次堆分配
FString HealthString = FString::Printf(TEXT("%s/%s"),
    *FString::SanitizeFloat(HealthNum),      // 临时 FString → 分配
    *FString::SanitizeFloat(MaxHealthNum));  // 临时 FString → 分配
Health->SetText(FText::FromString(HealthString));  // FString→FText 再分配
```

每调用一次走完 FString → FString → TCHAR* → 临时析构 → Printf分配 → FText拷贝 全路径。

**根因**：`FString::SanitizeFloat` 按值返回 `FString`（不是 `const TCHAR*`），调用方必须构造临时对象接住它。`*` 取内部 TCHAR* 后临时对象活到语句末尾才析构——花一次堆分配换一个裸指针。

**改进**：绕过 FString，全程用 FText：

```cpp
// FText 内部分配，零 FString 中转
Health->SetText(FText::Format(
    FText::FromString(TEXT("{0}/{1}")),
    FText::AsNumber(AttributeSet->GetHealth()),
    FText::AsNumber(AttributeSet->GetMaxHealth())));
```

`FText::AsNumber` 内部做数值格式化，不产生中间 FString 临时对象。`FText::Format` 返回 FText，一步到位进 SetText。

**核心原则**：在 UI 更新路径里，能直接用 FText 就别经过 FString。两个类型的转换永远有拷贝成本。

**注意事项**：`FText::AsNumber` 默认不限制小数位数，如果属性值 `123.456789` 会显示完整精度。需要定点显示时可传 `FText::FTextAsNumberOptions`：
```cpp
FText::AsNumber(HealthNum, FText::FTextAsNumberOptions()
    .SetMaxFractionalDigits(1))  // 保留一位小数
```

**改动落地**：`UI_CharacterInfo.cpp` 中 6 个控件的文本更新全部从 `FString::Printf + SanitizeFloat` 改为 `FText::Format + AsNumber`，Attack/Defense 单值直接 `FText::AsNumber`。每帧从约 18 次堆分配降到 FText 内部合理水平。

</details>
