# FD 项目诊断日志参考手册

> 所有日志使用 `LogFD` 通道。编辑器 Output Log 中过滤 `LogFD` 即可查看全部。

## 启动与生命周期

| 日志 | 级别 | 出现时机 | 说明 |
|------|------|---------|------|
| `FD Module Started` | Log | 模块加载 | FD 游戏模块启动，确认 DLL 成功加载 |
| `FD Module Shutdown` | Log | 编辑器关闭/热重载 | 模块正常卸载，频繁出现可能是 Live Coding 触发热重载 |

---

## 相机系统 (Camera)

| 日志 | 级别 | 出现时机 | 排查要点 |
|------|------|---------|---------|
| `Camera: DefaultModeClass is null, camera not activated` | **Warning** | PC 激活相机时 `DefaultModeClass` 为空 | BP_Character → GameCamera 组件 → `Default Mode Class` 未设置 |
| `Camera: Pushed {ClassName}` | Log | 相机模式推入栈 | 确认相机模式正确激活。如果看不到此日志，相机未初始化 |
| `Camera: Popped, restored previous mode` | Log | PopCameraMode() 调用 | 从栈顶弹出当前模式，恢复上一层 |
| `Camera: TickComponent running, ActiveMode={Name}, CachedCamera={Name}` | Log | 首次 Tick | 仅打印一次。ActiveMode=null 表示相机未激活；CachedCamera=null 表示 Character 缺少 UCameraComponent |
| `FDGameCameraComponent: No UCameraComponent found on {Owner}` | **Warning** | BeginPlay 时 `FindComponentByClass<UCameraComponent>` 失败 | BP_Character 未刷新，缺少 `Camera` 组件 |

---

## 输入系统 (Input)

| 日志 | 级别 | 出现时机 | 排查要点 |
|------|------|---------|---------|
| `HeroComponent::InitializePlayerInput called` | Log | 角色 SetupPlayerInputComponent | 确认输入初始化入口被调用 |
| `HeroComponent: InputConfig is null, input binding skipped` | **Warning** | InputConfig 为空 | BP_Character → HeroComp → `Input Config` 未设置 |
| `HeroComponent: InputComponent is not UEnhancedInputComponent! Input binding failed` | **Error** | 非 Enhanced Input 环境 | 项目未启用 Enhanced Input 插件 |
| `HeroComponent: OwnerPawn={Name}, PC={Name}` | Log | 输入绑定时 | PC=null 表示 Controller 不是 AFDPlayerController |
| `HeroComponent: Bound InputTag.Move -> {IA_Name}` | Log | 移动绑定完成 | 确认 IA_Move 和 IA_ClickMove 各自正确映射到 Tag |
| `HeroComponent: Bound InputTag.Camera.Zoom -> {IA_Name}` | Log | 缩放绑定完成 | 缩放直接绑定到 CameraComponent |
| `HeroComponent: Bound InputTag.ClickMove -> {IA_Name}` | Log | 点击移动绑定完成 | 确认 IA_ClickMove 的 Tag 正确设为 `InputTag.ClickMove` |
| `PlayerController: HandleWASDMove triggered (first call)` | Log | 首次按 WASD | 仅打印一次。不出现 = IMC 按键映射或 Tag 配置有问题 |

---

## 技能输入 (预留)

| 日志 | 级别 | 出现时机 | 排查要点 |
|------|------|---------|---------|
| `Ability Pressed: {InputTag}` | Log | 技能键按下 | 当前为预留占位，后续阶段接入 ASC |
| `Ability Released: {InputTag}` | Log | 技能键释放 | 同上 |
| `Interact pressed` | Log | 交互键按下 | 当前为预留占位 |

---

## PlayerController

| 日志 | 级别 | 出现时机 | 排查要点 |
|------|------|---------|---------|
| `AFDPlayerController created` | Log | PC 构造 | 确认使用的 PlayerController 类型是 AFDPlayerController |
| `AFDPlayerController::OnPossess called, Pawn={Name}` | Log | Possess Pawn 时 | PC 接管 Pawn 的时机。相机激活和 IMC 注入都在此后 |

---

## InitState 状态机

| 日志 | 级别 | 出现时机 | 排查要点 |
|------|------|---------|---------|
| `PawnExtension InitState: {State}` | Log | 状态推进 | 正常流程：Spawned → DataAvailable → DataInitialized → GameplayReady。卡在某阶段说明缺少推进条件 |

---

## Experience 系统

| 日志 | 级别 | 出现时机 | 排查要点 |
|------|------|---------|---------|
| `SetCurrentExperience: Invalid Experience` | **Error** | Experience 参数无效 | 传入的 Experience 资产为 null 或无效 |
| `SetCurrentExperience: Already loading/loaded, ignoring` | **Warning** | 重复设置 Experience | 单次游戏只能加载一个 Experience |
| `Experience loading started: {Name}` | Log | 异步加载开始 | Experience 资产加载流程启动 |
| `Experience assets loaded, executing actions...` | Log | 资产加载完成 | Action 执行阶段开始 |
| `Experience fully loaded` | Log | Experience 就绪 | 所有 Action 执行完毕，广播 OnExperienceLoaded |

---

## 快速排障速查表

| 症状 | 关键日志 | 修复方向 |
|------|---------|---------|
| 相机无效果 | 找 `Camera: Pushed` 是否出现 | DefaultModeClass 未设置 / PC 未正确 Possess |
| 相机参数不生效 | 已有 `Camera: Pushed` 和 `TickComponent running` | BP_CM_TopDown 的序列化值覆盖了 C++ 默认值 |
| WASD 无反应 | 找 `HandleWASDMove triggered` | 不出现 → IMC_Default / IC_InputConfig 配置问题 |
| 点击无反应 | 找 `Bound InputTag.ClickMove` | 不出现 → IC_InputConfig 中 IA_ClickMove Tag 未改为 InputTag.ClickMove |
| 滚轮无效 | 找 `Bound InputTag.Camera.Zoom` | 不出现 → IC_InputConfig 缺少 Zoom 条目 |
| 所有输入无效 | 找 `InputConfig is null` | BP_Character → HeroComp 配置未设 |
| 角色移动异常 | — | 检查 CharacterMovement 俯视角配置是否生效（BP 刷新） |
