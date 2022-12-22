[ "${BASH_SOURCE[0]}" -ef "$0" ] && {
    echo "This script provides logging functions and should be sourced in a shell"
    exit 1
}

red='\033[0;31m'
orange='\033[0;33m'
lightgray='\033[0;37m'
noColour='\033[0m' 

log() {
    local color=$1
    local type=$2
    shift 2

    echo -e "${color}[$(date --rfc-3339=seconds)] ${type}: $@${noColour}"
}

info() {
    log $lightgray "INFO" "$@"
}

warning() {
    log $orange "WARNING" "$@"
}

error() {
    log $red "ERROR" "$@"
}

fatal() {
    log $red "FATAL" "$@"
    exit 1
}

