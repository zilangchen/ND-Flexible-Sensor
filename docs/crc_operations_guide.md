# Notre Dame CRC 操作指南

本文档整理了在 Notre Dame CRC 集群上进行科研计算的常用命令，每个命令都配有详细说明和使用场景。

---

## 📋 目录
- [账户与存储](#账户与存储)
- [软件环境管理](#软件环境管理)
- [GPU 作业管理](#gpu-作业管理)
- [作业监控与调试](#作业监控与调试)
- [文件传输](#文件传输)
- [实用别名设置](#实用别名设置)
- [故障排除](#故障排除)

---

## 🔐 账户与存储

### 登录与配额管理

| 命令 | 作用 | 使用场景 |
|------|------|----------|
| `ssh zchen27@crcfe01.crc.nd.edu` | SSH 登录 CRC 前端节点 | 每次开始工作时使用 |
| `quota` | 查看 AFS 存储配额使用情况 | 检查个人存储空间是否充足 |
| `pan_df -H /scratch365/zchen27` | 查看 /scratch365 临时存储配额 | 检查临时存储空间，适合大数据处理 |

**说明：**
- AFS 是永久存储，适合代码和重要数据
- /scratch365 是临时存储，速度快但容量有限，适合中间结果

---

## 🐍 软件环境管理

### Conda 环境操作

| 命令 | 作用 | 使用场景 |
|------|------|----------|
| `module load conda` | 加载 Conda 模块 | 每次新开终端都需要执行 |
| `conda create -n myenv python=3.10 -y` | 创建新的 Python 虚拟环境 | 为新项目创建独立环境 |
| `conda activate myenv` | 激活指定的虚拟环境 | 开始使用特定环境 |
| `conda deactivate` | 退出当前虚拟环境 | 切换回系统环境 |
| `conda list` | 查看当前环境安装的包 | 检查依赖包版本 |
| `conda env list` | 查看所有可用的环境 | 管理多个项目环境 |

### 包管理

| 命令 | 作用 | 使用场景 |
|------|------|----------|
| `conda install numpy pandas matplotlib -y` | 安装 Python 包 | 添加项目依赖 |
| `pip install package_name` | 安装 PyPI 包 | 安装 conda 中没有的包 |
| `conda env export --from-history > environment.yml` | 导出环境配置 | 备份环境供团队共享 |

---

## 🚀 GPU 作业管理

### 交互式 GPU 会话

| 命令 | 作用 | 使用场景 |
|------|------|----------|
| `qrsh -q gpu -l gpu_card=1 -pe smp 1` | 申请交互式 GPU 会话 | 调试代码、快速测试 |
| `exit` | 退出 GPU 节点，释放资源 | 使用完毕后必须执行 |

**参数说明：**
- `-q gpu`：指定 GPU 队列
- `-l gpu_card=1`：申请 1 张 GPU 卡
- `-pe smp 1`：申请 1 个 CPU 核心

### 批处理作业

#### 作业脚本模板 (gpu_job.sh)
```bash
#!/bin/bash
#$ -M zchen27@nd.edu      # 邮件通知地址
#$ -m abe                 # 邮件通知时机 (begin/end/abort)
#$ -pe smp 4              # 申请 4 个 CPU 核心
#$ -q gpu                 # 使用 GPU 队列
#$ -l gpu_card=1          # 申请 1 张 GPU 卡
#$ -N my_gpu_job          # 作业名称

# 加载软件环境
module load conda
conda activate myenv

# 执行程序
python train.py
```

#### 作业提交与管理

| 命令 | 作用 | 使用场景 |
|------|------|----------|
| `qsub gpu_job.sh` | 提交批处理作业 | 运行长时间训练任务 |
| `qstat -u zchen27` | 查看自己的所有作业 | 监控作业状态 |
| `qstat -j <job_id>` | 查看特定作业的详细信息 | 调试作业问题 |
| `qdel <job_id>` | 删除指定的作业 | 取消不需要的作业 |

---

## 📊 作业监控与调试

### GPU 资源监控

| 命令 | 作用 | 使用场景 |
|------|------|----------|
| `free_gpus.sh @crc_gpu` | 查看所有 GPU 节点的空闲情况 | 选择有资源的节点 |
| `free_nodes.sh -G` | 查看空闲的 CPU 节点 | 申请 CPU 资源 |
| `nvidia-smi` | 查看当前节点的 GPU 使用情况 | 在 GPU 节点上检查资源 |
| `qhost -h <hostname>` | 查看特定节点的资源信息 | 了解节点配置 |

### 进程监控

| 命令 | 作用 | 使用场景 |
|------|------|----------|
| `ps aux | grep zchen27` | 查看自己的进程 | 检查程序是否在运行 |
| `top` | 实时查看系统资源使用 | 监控 CPU 和内存使用 |
| `htop` | 交互式系统监控 | 更直观的资源监控 |

---

## 📁 文件传输

### 本地与 CRC 之间的传输

| 命令 | 作用 | 使用场景 |
|------|------|----------|
| `scp local_file.zip zchen27@crcfe02.crc.nd.edu:~/` | 上传文件到 CRC | 传输代码或数据 |
| `scp zchen27@crcfe02.crc.nd.edu:~/remote_file.zip ./` | 从 CRC 下载文件 | 获取计算结果 |
| `rsync -avP local_dir/ zchen27@crcfe02.crc.nd.edu:~/remote_dir/` | 同步目录 | 批量传输大量文件 |
| `rsync -avP zchen27@crcfe02.crc.nd.edu:~/remote_dir/ ./local_dir/` | 从 CRC 同步目录 | 下载计算结果 |

**参数说明：**
- `-a`：归档模式，保持文件属性
- `-v`：详细输出
- `-P`：显示进度条

---

## ⚡ 实用别名设置

将以下内容添加到 `~/.bashrc` 文件中，可以简化常用命令：

```bash
# GPU 和作业监控
alias gpu-free='free_gpus.sh @crc_gpu'
alias node-free='free_nodes.sh -G'
alias myjobs='qstat -u zchen27'

# 快速提交 GPU 作业
alias gpu-submit='qsub gpu_job.sh'

# 交互式 GPU 调试
alias gpu-qrsh="qrsh -q gpu -l gpu_card=1 -pe smp 1"

# 配额查看
alias afs-quota='quota'
alias scratch-quota='pan_df -H /scratch365/zchen27'

# Conda 环境管理
alias cda='conda activate'
alias cdl='conda deactivate'
alias cdenv='conda env list'
```

添加后执行 `source ~/.bashrc` 使别名生效。

---

## 🔧 故障排除

### 常见问题及解决方案

| 问题 | 可能原因 | 解决方案 |
|------|----------|----------|
| `conda: command not found` | 未加载 conda 模块 | 执行 `module load conda` |
| GPU 作业无法提交 | 资源不足或参数错误 | 检查 `free_gpus.sh @crc_gpu` 和作业脚本 |
| 文件传输失败 | 网络问题或权限不足 | 检查网络连接和文件权限 |
| 作业被杀死 | 超出资源限制 | 检查内存和 CPU 使用情况 |

### 获取帮助

| 命令 | 作用 |
|------|------|
| `module help` | 查看模块系统帮助 |
| `qstat -help` | 查看作业管理帮助 |
| `man command_name` | 查看命令手册 |

---

## 📝 最佳实践

### 1. 资源使用
- 使用完毕后及时释放 GPU 资源 (`exit`)
- 合理设置作业资源需求，避免过度申请
- 定期清理临时文件

### 2. 环境管理
- 为每个项目创建独立的 conda 环境
- 定期导出环境配置文件
- 使用 `environment.yml` 确保环境可重现

### 3. 文件管理
- 重要数据存储在 AFS，临时数据使用 /scratch365
- 定期备份重要文件
- 使用有意义的文件名和目录结构

### 4. 作业管理
- 为作业设置合理的邮件通知
- 使用描述性的作业名称
- 监控作业状态，及时处理异常

---

## 🎯 快速开始清单

新用户建议按以下顺序熟悉 CRC：

1. **登录和基础操作**
   ```bash
   ssh zchen27@crcfe02.crc.nd.edu
   quota
   module load conda
   ```

2. **创建项目环境**
   ```bash
   conda create -n myproject python=3.10 -y
   conda activate myproject
   conda install numpy pandas matplotlib -y
   ```

3. **测试 GPU 环境**
   ```bash
   gpu-qrsh  # 使用别名
   nvidia-smi
   exit
   ```

4. **提交第一个作业**
   ```bash
   # 创建作业脚本
   vim gpu_job.sh
   # 提交作业
   qsub gpu_job.sh
   # 监控作业
   myjobs  # 使用别名
   ```

---

*最后更新：2025-07-16*
*维护者：zchen27* 