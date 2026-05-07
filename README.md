# 学习仓库 — UE 5.6

一个配合教程系统学习 Unreal Engine 5.6 的个人仓库，使用 **Claude Code + DeepSeek-v4** 辅助学习，**不依赖 AI 生成项目代码**。

---

## 项目

### Project_YM_WebP — 第三方库集成

将 libwebp 编译为 UE 插件，学习从原生 C/C++ 库到 UE 蓝图暴露的完整封装链路。

**完成进度：**
- [x] 搭建标准第三方库空白模板 + 集成 libwebp
- [x] 单张静态 WebP 生成
- [x] 动态/动画 WebP 生成与读取
- [x] 视口像素数据获取 + 截图功能
- [x] GameInstanceSubsystem 封装 → 蓝图暴露
- [ ] 蓝图端完整工作流演示

### YMRPG — 多玩法 RPG 游戏

包含三种 gameplay 变体的 RPG 项目，学习 AI、动画、UI、网络同步、GAS 等游戏玩法系统。

**完成进度：**
- [x] 三种变体基础代码（Combat / Platforming / SideScrolling）
- [x] 搭建 GAS 项目类继承体系（Character / GameMode / GameState / PlayerState / HUD）
- [ ] GAS（GameplayAbilitySystem）核心功能实现
- [ ] 网络同步与 DSM 部署
- [ ] 完整战斗/物品/UI 系统

---

## 环境

- **引擎**：UE 5.6（Launcher + 源码版）
- **IDE**：Visual Studio 2022
- **版本控制**：Git + LFS
- **OS**：Windows 11
