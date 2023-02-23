#! /bin/bash
#

BIN="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

usage() {
    cat <<EOF
$(basename $0) - run UE4 editor for project

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
    echo "ERROR: provide a valid UE4 installation directory"
    usage
    exit 1
}

$ueDir/Engine/Binaries/Linux/UE4Editor "$BIN/../../UEBox.uproject" "$@"
