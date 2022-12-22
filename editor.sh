#! /bin/bash

set -eo pipefail
BIN="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source $BIN/logging.sh

usage() {
    cat <<EOF
editor.sh - open project in editor

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

(
    info "Starting Unreal editor"
    LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64:/opt/cuda/lib64:$ueDir/Engine/Binaries/Linux:$ueDir/Engine/Binaries/ThirdParty/Qualcomm/Linux:$ueDir/Engine/Binaries/ThirdParty/OpenVR/OpenVRv1_5_17/linux64:$ueDir/Engine/Binaries/ThirdParty/PhysX3/Linux/x86_64-unknown-linux-gnu:$ueDir/Engine/Binaries/ThirdParty/PhysX3/Linux/x86_64-unknown-linux-gnu \
    __NV_PRIME_RENDER_OFFLOAD=1 \
    __VK_LAYER_NV_optimus=NVIDIA_only \
    __NV_PRIME_RENDER_OFFLOAD_PROVIDER=NVIDIA-G0 \
    __GLX_VENDOR_LIBRARY_NAME=nvidia \
    VK_ICD_FILENAMES=/usr/share/vulkan/icd.d/nvidia_icd.json \
    CUDA_VISIBLE_DEVICES=0 \
    $ueDir/Engine/Binaries/Linux/UE4Editor \
    "$BIN/UEBox.uproject" "$@"
)

