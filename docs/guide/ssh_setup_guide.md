# SSH免密登录配置指南

## CRC服务器SSH配置信息

**服务器**: crcfe01.crc.nd.edu  
**用户**: zchen27  
**配置日期**: 2025-07-17

### 现有SSH密钥信息

**公钥内容**:
```
ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAACAQCcr2+bI+1XV0+tN35vzJa7k9pQstbg0QwR1c1r3l8Ykc8a88wJEgKhpMg78lDtmizXcTYCMaqpKzfP+G9qfyxX6ledHQnWlC/Dz5JV91ZNu5pUjEXCF67z7B7WWwOMnXrP2svLbTi1d+A84qMb+8kv7l1MkY2AjtNSZDO5V0NpYnhsndfLjMm2dJt+s8DQ432i7VsH3qf5wKMi5+0mP0n4JuF1PAeN/IxjpHhBa0hBCcsxdDxieLmlt6TCLkCBwDgpsR9B8FqSPJkr+mo7OXTJSCzHEX2qaiacWiAwb80Xtpb+VUzjwea/8NVo6hrfDmvzbb67gkKKVNGoOU1HjMW7NQ4kWIWDa2e4fJcGUTithYO9PRv5lsy2ZkRLfVa1v23HPut5n6soKVW7chAZkOcX7URChJ/gNrxEZhhkzZz/RuZ39G9NZ9JBIB1MfWZJM73Z/XiuyxA5kPowUPGFiXtg84jwEPGMth0hKhz1soQvMSEJHeZkAKuE6UMwsdslKZAzaiC+E+tWqgTEBhXnNTW7nhHqlL0oCRqbZGUQcB+oapsucqScXWOb0ocluWXHfIDuYPSEz9I8K5/R9uOR4LFPxvYFa1u9RRZjWt0mqnyPPuX790VwsWr1Vhc7HBRKNqKbh7svUP+sjGCynlxBe/JzqVKcOVE/i5ADHsvjgVjQ0w== zchen27@crcfe01.crc.nd.edu
```

## 本地电脑到CRC免密登录配置

### 步骤1: 在本地生成SSH密钥
```bash
ssh-keygen -t rsa -b 4096 -C "your_email@example.com"
```

### 步骤2: 上传公钥到CRC
```bash
# 方法A: 自动上传
ssh-copy-id zchen27@crcfe01.crc.nd.edu

# 方法B: 手动添加
# 将本地公钥内容添加到 /users/zchen27/.ssh/authorized_keys
```

### 步骤3: 配置SSH客户端 (~/.ssh/config)
```
Host crc
    HostName crcfe01.crc.nd.edu
    User zchen27
    Port 22
    IdentityFile ~/.ssh/id_rsa
    ServerAliveInterval 60
    ServerAliveCountMax 3
    KeepAlive yes
    
Host crc-gpu
    HostName crcfe01.crc.nd.edu
    User zchen27
    Port 22
    IdentityFile ~/.ssh/id_rsa
    LocalForward 8888 localhost:8888
    LocalForward 6006 localhost:6006
    ServerAliveInterval 60
    ServerAliveCountMax 3
    KeepAlive yes
```

### 步骤4: 测试连接
```bash
ssh crc
ssh crc-gpu  # 带端口转发的连接
```

## 从CRC连接到其他服务器

### 使用现有密钥连接其他服务器:
```bash
# 将CRC公钥添加到目标服务器的authorized_keys
ssh-copy-id -i ~/.ssh/id_rsa.pub user@target-server.com

# 或手动复制公钥内容到目标服务器
cat ~/.ssh/id_rsa.pub
```

## 安全性建议

1. **私钥保护**: 设置密码保护私钥
2. **权限设置**: 确保SSH目录权限正确
   ```bash
   chmod 700 ~/.ssh
   chmod 600 ~/.ssh/id_rsa
   chmod 644 ~/.ssh/id_rsa.pub
   chmod 600 ~/.ssh/authorized_keys
   ```
3. **定期更新**: 定期更换SSH密钥
4. **备份密钥**: 安全备份私钥文件

## 高级配置

### SSH代理转发 (Agent Forwarding)
```
Host crc
    HostName crcfe01.crc.nd.edu
    User zchen27
    ForwardAgent yes
```

### 多跳连接 (Jump Host)
```
Host target-server
    HostName target.example.com
    User your_user
    ProxyJump crc
```

### 端口转发示例
```
Host crc-jupyter
    HostName crcfe01.crc.nd.edu
    User zchen27
    LocalForward 8888 localhost:8888
    LocalForward 6006 localhost:6006
    LocalForward 8080 localhost:8080
``` 