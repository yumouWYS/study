# 学习日志

记录每日有技术价值的提问与对应知识点，按项目与日期归档，方便检索复习。

## 目录

- [2026-05-08](#ymrpg-2026-05-08) — UCLASS 说明符、UBT/UHT 构建系统、GA 生命周期与四项策略、FObjectInitializer、CurrentActorInfo 解耦、网络模型、GameplayTag 体系

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
