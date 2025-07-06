#!/bin/bash

# pt_log.sh - 彩色日志打印函数库
# 用法：source pt_log.sh 后调用 pt_log <level> <message>

# 默认日志级别
DEFAULT_LOG_LEVEL="DEBUG"

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
NC='\033[0m' # No Color

# 定义日志级别
declare -A LOG_LEVELS=(
    ["DEBUG"]=0
    ["INFO"]=1
    ["WARN"]=2
    ["ERROR"]=3
)

# 获取当前时间戳
function _get_timestamp() {
    echo "$(date '+%Y-%m-%d %H:%M:%S')"
}


# 日志打印函数
# 参数: $1=日志级别 $2=日志内容
function pt_log()
{
    # 参数个数检查
    [ $# -lt 2 ] && return

    local level=${1^^}
    local message=$2

    # 检查日志级别是否有效
    if [ -z ${LOG_LEVELS[$level]} ]; then
        echo "[$(_get_timestamp)] [ERROR] invalid log level:$level"
        return 1
    fi

    # 检查是否达到输出级别
    if [ ${LOG_LEVELS[$level]} -lt ${LOG_LEVELS[$DEFAULT_LOG_LEVEL]} ]; then
        return 0
    fi

    # 根据级别设置颜色
    case $level in
        "DEBUG")
            color=$BLUE
            ;;
        "INFO")
            color=$GREEN
            ;;
        "WARN")
            color=$YELLOW
            ;;
        "ERROR")
            color=$RED
            ;;
        *)
            color=$NC
            ;;
    esac

    echo -e "[$(_get_timestamp)] [${color}${level}${NC}] ${message}"
}

# 测试代码（当直接运行脚本时执行）
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    echo "=== pt_log 测试 ==="
    pt_log "DEBUG" "这是一条调试信息"
    pt_log "INFO" "这是一条普通信息"
    pt_log "WARN" "这是一条警告信息"
    pt_log "ERROR" "这是一条错误信息"
fi