# GPU 资源管理器使用指南

## 概述

GPU 资源管理器是一个智能的 GPU 资源管理系统，可以：
- 实时监控 GPU 可用性
- 动态获取空闲 GPU
- 支持多 GPU 协作
- 自动扩展运行中的程序

## 核心功能

### 1. 动态 GPU 获取
- 监控所有 GPU 节点的空闲状态
- 当检测到空闲 GPU 时自动获取
- 支持多 GPU 协作运行程序

### 2. 智能资源管理
- 自动选择最优的 GPU 节点
- 跟踪 GPU 分配和使用情况
- 支持资源的动态释放

### 3. 程序自动扩展
- 当新的 GPU 可用时，自动扩展运行中的程序
- 支持多 GPU 并行计算
- 智能负载均衡

## 使用方法

### 基本命令

```bash
# 启动监控模式（推荐）
python scripts/gpu_resource_manager.py

# 查看当前状态
python scripts/gpu_resource_manager.py --status

# 为特定任务分配 GPU
python scripts/gpu_resource_manager.py --allocate my_task

# 释放任务的 GPU
python scripts/gpu_resource_manager.py --release my_task

# 启动程序
python scripts/gpu_resource_manager.py --start-program my_task
```

### 完整工作流程示例

```bash
# 1. 启动资源管理器
python scripts/gpu_resource_manager.py &

# 2. 分配 GPU 给任务
python scripts/gpu_resource_manager.py --allocate training_task_1

# 3. 启动训练程序
python scripts/gpu_resource_manager.py --start-program training_task_1

# 4. 监控状态
python scripts/gpu_resource_manager.py --status

# 5. 完成后释放资源
python scripts/gpu_resource_manager.py --release training_task_1
```

### 手动申请 GPU

在自动保底外，如需临时占用更多 GPU，可运行：

```bash
# 申请 4 张 GPU，任务名 exp_train
python scripts/gpu_resource_manager.py --allocate exp_train --gpus 4
```

说明：
1. `--allocate <task_id>` 指定作业名称；脚本会自动在名称前加前缀 `NDFS_`，避免被误判为占位。
2. `--gpus <N>` 申请 GPU 张数，受以下限制：
   • 全局硬上限 `max_total_gpus=8`；
   • 自动模式最多 2 张，手动可再要 `max_manual_gpus=6`；
3. 如果需求超过可用额度，脚本会给出警告并拒绝提交。

> 建议在提交前用 `system_monitor.py` 查看当前 Running/Pending 数，保持资源占用合理。

## 配置说明

### 配置文件：`gpu_manager_config.json`

```json
{
  "monitor_interval": 5,           // 监控间隔（秒）
  "max_gpus_per_task": 4,         // 每个任务最大 GPU 数
  "auto_scale": true,             // 是否启用自动扩展
  "program_template": {            // 程序启动模板
    "command": "python train.py",
    "args": ["--gpus", "{gpu_list}"],
    "env_vars": {
      "CUDA_VISIBLE_DEVICES": "{gpu_list}"
    }
  },
  "scaling": {                     // 扩展配置
    "enabled": true,
    "min_gpus": 1,
    "max_gpus": 4,
    "scale_up_threshold": 0.8,
    "scale_down_threshold": 0.3
  }
}
```

## 高级用法

### 1. 多任务管理

```bash
# 同时运行多个任务
python scripts/gpu_resource_manager.py --allocate task1
python scripts/gpu_resource_manager.py --allocate task2
python scripts/gpu_resource_manager.py --start-program task1
python scripts/gpu_resource_manager.py --start-program task2
```

### 2. 自定义程序配置

修改 `gpu_manager_config.json` 中的 `program_template`：

```json
{
  "program_template": {
    "command": "python my_custom_script.py",
    "args": [
      "--model", "resnet50",
      "--data", "/path/to/data",
      "--gpus", "{gpu_list}",
      "--epochs", "200"
    ],
    "env_vars": {
      "CUDA_VISIBLE_DEVICES": "{gpu_list}",
      "PYTHONPATH": "/users/zchen27/ND-Flexible-Sensor",
      "CUDA_LAUNCH_BLOCKING": "1"
    }
  }
}
```

### 3. 自动扩展配置

```json
{
  "scaling": {
    "enabled": true,
    "min_gpus": 1,              // 最少 GPU 数
    "max_gpus": 8,              // 最大 GPU 数
    "scale_up_threshold": 0.8,  // GPU 使用率超过 80% 时扩展
    "scale_down_threshold": 0.3 // GPU 使用率低于 30% 时收缩
  }
}
```

## 监控和日志

### 状态监控

```bash
# 实时查看状态
python scripts/gpu_resource_manager.py --status

# 输出示例：
# ============================================================
# GPU Resource Manager Status
# ============================================================
# Total Nodes: 11
# Total GPUs: 44
# Free GPUs: 2
# Allocated GPUs: 42
# Active Tasks: 3
# 
# Node Details:
# ----------------------------------------
# qa-a10-023: 0/4 free (4 allocated)
# qa-a10-024: 1/4 free (3 allocated)
# qa-a10-025: 1/4 free (3 allocated)
# 
# Task Allocations:
# ----------------------------------------
# Task training_task_1: 2 GPUs on qa-a10-024 (running)
# Task training_task_2: 1 GPU on qa-a10-025 (running)
```

### 日志文件

- 日志文件：`gpu_manager.log`
- 包含所有操作记录和错误信息
- 支持日志轮转和大小限制

## 最佳实践

### 1. 资源规划
- 根据任务需求合理设置 GPU 数量
- 避免过度分配导致资源浪费
- 使用自动扩展功能优化资源使用

### 2. 任务管理
- 为每个任务使用唯一的任务 ID
- 及时释放不再使用的资源
- 监控任务状态和性能

### 3. 错误处理
- 检查日志文件了解错误原因
- 确保程序支持多 GPU 运行
- 配置合适的重试机制

### 4. 性能优化
- 调整监控间隔平衡性能和资源消耗
- 使用合适的扩展阈值
- 优化程序的多 GPU 支持

## 故障排除

### 常见问题

1. **无法分配 GPU**
   - 检查是否有空闲 GPU
   - 确认节点状态正常
   - 查看日志文件了解详细错误

2. **程序启动失败**
   - 检查程序路径和参数
   - 确认环境变量设置正确
   - 验证 GPU 驱动和 CUDA 版本

3. **自动扩展不工作**
   - 确认 `auto_scale` 设置为 `true`
   - 检查扩展阈值配置
   - 验证程序支持动态 GPU 扩展

### 调试命令

```bash
# 查看详细日志
tail -f gpu_manager.log

# 检查 GPU 状态
free_gpus.sh @crc_gpu

# 查看进程状态
ps aux | grep gpu_resource_manager

# 检查配置文件
cat gpu_manager_config.json
```

## 扩展功能

### 1. 邮件通知
配置邮件通知功能：

```json
{
  "notification": {
    "enabled": true,
    "email": "zchen27@nd.edu",
    "gpu_available_threshold": 1
  }
}
```

### 2. 自定义监控
可以扩展监控功能：
- 添加性能指标监控
- 集成其他集群管理工具
- 支持更多资源类型

### 3. API 接口
未来可以添加 REST API 接口：
- 支持远程管理
- 集成到 Web 界面
- 支持自动化脚本调用

---

*最后更新：2025-07-16*
*维护者：zchen27* 