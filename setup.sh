#!/bin/bash

usage() {
    cat <<EOF
setup.sh - setup demo project files

  -u Unreal Engine installation directory
  -h help

EOF
}

while getopts 'u:h' c; do
    case $c in
        u) ue4=$OPTARG ;;
        h) usage && exit 0 ;;
    esac
done

[ -n "$ue4" -a -d "$ue4" ] || {
    echo "ERROR: provide a valid UE4 installation directory"
    usage
    exit 1
}

$ue4/Engine/Build/BatchFiles/Linux/GenerateProjectFiles.sh -project="$PWD/UEBox.uproject" -game -engine -makefile -vscode -cmakefile
