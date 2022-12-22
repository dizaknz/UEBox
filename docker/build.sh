#! /bin/bash

set -e

BIN="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

usage() {
    cat <<EOF
build.sh - build docker image for Unreal Engine using local checkout

  -u Unreal Engine source directory
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
    echo "ERROR: provide a valid Unreal Engine source directory"
    usage
    exit 1
}

cat $ueDir/Engine/Build/Build.version
version=$(jq -r '.MajorVersion,.MinorVersion,.PatchVersion' $ueDir/Engine/Build/Build.version | tr -s "\n" "." | sed -e 's/\.$//')
  
[ -f "$ueDir/.dockerignore" ] || echo ".git" > $ueDir/.dockerignore

time docker buildx build \
    --progress=plain \
    --compress \
    --build-context ue_source_dir=$ueDir \
    -t ue:$version .
