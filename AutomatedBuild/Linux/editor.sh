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
        u) ue4=$OPTARG ;;
        h) usage && exit 0 ;;
    esac
done

[ -n "$ue4" -a -d "$ue4" ] || {
    echo "ERROR: provide a valid UE4 installation directory"
    usage
    exit 1
}

$ue4/Engine/Binaries/Linux/UE4Editor "$BIN/../../UEBox.uproject" "$@"
