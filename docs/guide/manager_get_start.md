# GPU 管理器快速上手指令集

> 新手友好的命令行指令集，直接复制粘贴即可使用！

## 🚀 基础操作（90%的情况都在用这些）

### 1. 查看当前集群状态

```bash
python scripts/gpu_manager/gpu_manager.py --status
```

**作用**：一次性查看 GPU 集群状态、可用资源、你的作业情况

### 2. 启动实时监控

```bash
tmux new-session -d -s gpu_monitor 'python scripts/gpu_manager/gpu_manager.py'
```

**作用**：后台启动实时监控，随时查看集群变化

### 3. 进入监控界面

```bash
tmux attach -t gpu_monitor
```

**作用**：查看实时 GPU 状态（按 Ctrl+B 再按 D 退出但保持运行）

### 4. 申请 GPU 资源

```bash
# 申请1个GPU
python scripts/gpu_manager/gpu_manager.py --request 1

# 申请2个GPU
python scripts/gpu_manager/gpu_manager.py --request 2

# 申请4个GPU
python scripts/gpu_manager/gpu_manager.py --request 4
```

**作用**：手动申请指定数量的 GPU

### 5. 查看我的作业

```bash
qstat -u $USER
```

**作用**：查看你提交的所有作业状态

## 🤖 自动化操作

### 6. 启动自动管理模式

```bash
tmux new-session -d -s gpu_auto 'python scripts/gpu_manager/gpu_manager.py --manage'
```

**作用**：自动维持 1-2 个 GPU，有资源时自动申请

### 7. 进入自动管理界面

```bash
tmux attach -t gpu_auto
```

**作用**：查看自动管理状态

## 🛠️ 管理操作

### 8. 释放作业

```bash
# 先查看作业ID
qstat -u $USER

# 释放指定作业（替换1234567为实际ID）
python scripts/gpu_manager/gpu_manager.py --release 1234567
```

**作用**：释放不需要的 GPU 作业

### 9. 批量释放作业

```bash
# 释放所有NDFS开头的作业
qdel $(qstat -u $USER | grep "NDFS" | awk '{print $1}')
```

**作用**：一键清理管理器申请的作业

### 10. 关闭所有 tmux 会话

```bash
tmux kill-server
```

**作用**：关闭所有后台监控和管理进程

## 📋 常用组合操作

### 快速开始流程

```bash
# 1. 查看集群状态
python scripts/gpu_manager/gpu_manager.py --status

# 2. 启动监控
tmux new-session -d -s gpu_monitor 'python scripts/gpu_manager/gpu_manager.py'

# 3. 申请资源（根据需要选择数量）
python scripts/gpu_manager/gpu_manager.py --request 2
```

### 日常使用流程

```bash
# 1. 进入监控界面
tmux attach -t gpu_monitor

# 2. 查看作业状态
qstat -u $USER

# 3. 根据需要申请或释放资源
```

### 完全清理流程

```bash
# 1. 释放所有作业
qdel $(qstat -u $USER | grep "NDFS" | awk '{print $1}')

# 2. 关闭所有会话
tmux kill-server

# 3. 确认清理完成
qstat -u $USER
```

## 💡 使用技巧

1. **tmux 会话管理**：

   - `tmux list-sessions` - 查看所有会话
   - `tmux attach -t 会话名` - 进入会话
   - `Ctrl+B, D` - 退出但保持会话运行
   - `tmux kill-session -t 会话名` - 关闭指定会话

2. **优先级顺序**：

   - 先用 `--status` 查看集群状态
   - 有空闲 GPU 时用 `--request` 申请
   - 需要持续监控时用 tmux 后台运行

3. **节约资源**：
   - 不用时及时释放 GPU：`--release 作业ID`
   - 定期检查作业状态：`qstat -u $USER`
   - 避免申请过多 GPU 资源

## ⚠️ 注意事项

- 首次使用前确保在 `/users/zchen27/ND-Flexible-Sensor` 目录下
- 申请 GPU 前先查看集群状态，避免无效申请
- 及时释放不使用的 GPU 资源，方便其他用户
- tmux 会话会持续运行直到手动关闭

---

**🎯 新手建议**：先从 `--status` 命令开始，熟悉界面后再尝试其他功能！



