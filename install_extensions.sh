#!/bin/bash

# 安装VS Code/Cursor扩展的脚本
# 使用方法: ./install_extensions.sh

EXTENSIONS_DIR="$HOME/.cursor-server/extensions"
TEMP_DIR="$HOME/.cursor-server/extensions-manual"

# 创建临时目录
mkdir -p "$TEMP_DIR"
cd "$TEMP_DIR"

# 常用扩展列表
extensions=(
    "ms-python.python"           # Python扩展
    "ms-python.black-formatter"  # Python代码格式化
    "ms-python.flake8"           # Python代码检查
    "ms-vscode.vscode-json"      # JSON支持
    "ms-vscode.vscode-markdown"  # Markdown支持
    "eamodio.gitlens"            # Git增强
    "ms-vscode.vscode-git"       # Git支持
    "ms-vscode.vscode-docker"    # Docker支持
    "ms-vscode.vscode-yaml"      # YAML支持
    "ms-vscode.vscode-toml"      # TOML支持
    "ms-vscode.vscode-ini"       # INI文件支持
    "ms-vscode.vscode-csv"       # CSV文件支持
    "ms-vscode.vscode-xml"       # XML支持
    "ms-vscode.vscode-html"      # HTML支持
    "ms-vscode.vscode-css"       # CSS支持
    "ms-vscode.vscode-javascript" # JavaScript支持
    "ms-vscode.vscode-typescript" # TypeScript支持
    "ms-vscode.vscode-java"      # Java支持
    "ms-vscode.vscode-cpptools"  # C/C++支持
    "ms-vscode.vscode-go"        # Go支持
    "ms-vscode.vscode-rust"      # Rust支持
    "ms-vscode.vscode-julia"     # Julia支持
    "ms-vscode.vscode-matlab"    # MATLAB支持
    "ms-vscode.vscode-r"         # R支持
    "ms-vscode.vscode-sql"       # SQL支持
    "ms-vscode.vscode-powershell" # PowerShell支持
    "ms-vscode.vscode-bash"      # Bash支持
    "ms-vscode.vscode-shell"     # Shell支持
    "ms-vscode.vscode-terminal"  # 终端支持
    "ms-vscode.vscode-debug"     # 调试支持
    "ms-vscode.vscode-test"      # 测试支持
    "ms-vscode.vscode-tasks"     # 任务支持
    "ms-vscode.vscode-snippets"  # 代码片段
    "ms-vscode.vscode-emmet"     # Emmet支持
    "ms-vscode.vscode-eslint"    # ESLint支持
    "ms-vscode.vscode-prettier"  # Prettier支持
    "ms-vscode.vscode-autoprefixer" # Autoprefixer支持
    "ms-vscode.vscode-babel"     # Babel支持
    "ms-vscode.vscode-webpack"   # Webpack支持
    "ms-vscode.vscode-npm"       # NPM支持
    "ms-vscode.vscode-yarn"      # Yarn支持
    "ms-vscode.vscode-pip"       # Pip支持
    "ms-vscode.vscode-conda"     # Conda支持
    "ms-vscode.vscode-jupyter"   # Jupyter支持
    "ms-vscode.vscode-notebook"  # Notebook支持
    "ms-vscode.vscode-plotly"    # Plotly支持
    "ms-vscode.vscode-matplotlib" # Matplotlib支持
    "ms-vscode.vscode-seaborn"   # Seaborn支持
    "ms-vscode.vscode-pandas"    # Pandas支持
    "ms-vscode.vscode-numpy"     # NumPy支持
    "ms-vscode.vscode-scipy"     # SciPy支持
    "ms-vscode.vscode-sklearn"   # Scikit-learn支持
    "ms-vscode.vscode-tensorflow" # TensorFlow支持
    "ms-vscode.vscode-pytorch"   # PyTorch支持
    "ms-vscode.vscode-keras"     # Keras支持
    "ms-vscode.vscode-opencv"    # OpenCV支持
    "ms-vscode.vscode-pillow"    # Pillow支持
    "ms-vscode.vscode-requests"  # Requests支持
    "ms-vscode.vscode-beautifulsoup" # BeautifulSoup支持
    "ms-vscode.vscode-selenium"  # Selenium支持
    "ms-vscode.vscode-flask"     # Flask支持
    "ms-vscode.vscode-django"    # Django支持
    "ms-vscode.vscode-fastapi"   # FastAPI支持
    "ms-vscode.vscode-sqlalchemy" # SQLAlchemy支持
    "ms-vscode.vscode-pytest"    # Pytest支持
    "ms-vscode.vscode-unittest"  # Unittest支持
    "ms-vscode.vscode-coverage"  # 代码覆盖率支持
    "ms-vscode.vscode-mypy"      # MyPy支持
    "ms-vscode.vscode-bandit"    # Bandit支持
    "ms-vscode.vscode-safety"    # Safety支持
    "ms-vscode.vscode-black"     # Black支持
    "ms-vscode.vscode-isort"     # isort支持
    "ms-vscode.vscode-flake8"    # Flake8支持
    "ms-vscode.vscode-pylint"    # Pylint支持
    "ms-vscode.vscode-autopep8"  # autopep8支持
    "ms-vscode.vscode-yapf"      # YAPF支持
    "ms-vscode.vscode-pycodestyle" # pycodestyle支持
    "ms-vscode.vscode-pydocstyle" # pydocstyle支持
    "ms-vscode.vscode-radon"     # Radon支持
    "ms-vscode.vscode-mccabe"    # McCabe支持
    "ms-vscode.vscode-xenon"     # Xenon支持
    "ms-vscode.vscode-prospector" # Prospector支持
    "ms-vscode.vscode-pylama"    # Pylama支持
    "ms-vscode.vscode-pylint"    # Pylint支持
    "ms-vscode.vscode-pycodestyle" # pycodestyle支持
    "ms-vscode.vscode-pydocstyle" # pydocstyle支持
    "ms-vscode.vscode-bandit"    # Bandit支持
    "ms-vscode.vscode-safety"    # Safety支持
    "ms-vscode.vscode-black"     # Black支持
    "ms-vscode.vscode-isort"     # isort支持
    "ms-vscode.vscode-flake8"    # Flake8支持
    "ms-vscode.vscode-autopep8"  # autopep8支持
    "ms-vscode.vscode-yapf"      # YAPF支持
    "ms-vscode.vscode-pycodestyle" # pycodestyle支持
    "ms-vscode.vscode-pydocstyle" # pydocstyle支持
    "ms-vscode.vscode-radon"     # Radon支持
    "ms-vscode.vscode-mccabe"    # McCabe支持
    "ms-vscode.vscode-xenon"     # Xenon支持
    "ms-vscode.vscode-prospector" # Prospector支持
    "ms-vscode.vscode-pylama"    # Pylama支持
)

echo "开始安装扩展..."
echo "扩展目录: $EXTENSIONS_DIR"
echo "临时目录: $TEMP_DIR"
echo ""

# 检查Cursor CLI是否可用
CURSOR_CLI="$HOME/.cursor-server/bin/a8e95743c5268be73767c46944a71f4465d05c90/bin/remote-cli/cursor"

if [ -f "$CURSOR_CLI" ]; then
    echo "找到Cursor CLI: $CURSOR_CLI"
    echo "使用Cursor CLI安装扩展..."
    
    for ext in "${extensions[@]}"; do
        echo "正在安装: $ext"
        "$CURSOR_CLI" --install-extension "$ext" || echo "安装失败: $ext"
    done
else
    echo "未找到Cursor CLI，尝试手动安装..."
    echo "请使用Cursor的自动同步功能或手动安装扩展"
fi

echo ""
echo "安装完成！"
echo "请重新启动Cursor以加载新安装的扩展" 