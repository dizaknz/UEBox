#!/bin/bash

set -eo pipefail
BIN="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source $BIN/logging.sh

usage() {
    cat <<EOF
setup.sh - setup project files

  -u Unreal Engine installation directory
  -h help

EOF
}

while getopts 'u:h' c; do
    case $c in
        u) ueDir=$OPTARG ;;
        h) usage && exit 0 ;;
    esac
done

[ -n "$ueDir" -a -d "$ueDir" ] || {
    error "provide a valid UE4 installation directory"
    usage
    exit 1
}

info "Generating project files"
$ueDir/Engine/Build/BatchFiles/Linux/GenerateProjectFiles.sh -project="$PWD/UEBox.uproject" -game -engine -makefile -vscode -cmakefile
