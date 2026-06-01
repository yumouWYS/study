# YMRPG 学习日志 — 2026 年 6 月

> UE 5.6 GAS 游戏玩法系统学习与实战开发

---

## 目录

</details>

---

## 6.1：UPrimaryDataAsset 继承链 / UHT Category 引号规则 / UBT 同名冲突 / 物品系统

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
