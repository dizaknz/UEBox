#! /bin/bash

BIN="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

usage() {
    cat <<EOF
editor.sh - open project in editor

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

(
    LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64:/opt/cuda/lib64:$ue4/Engine/Binaries/Linux:$ue4/Engine/Binaries/ThirdParty/Qualcomm/Linux:$ue4/Engine/Binaries/ThirdParty/OpenVR/OpenVRv1_5_17/linux64:$ue4/Engine/Binaries/ThirdParty/PhysX3/Linux/x86_64-unknown-linux-gnu:$ue4/Engine/Binaries/ThirdParty/PhysX3/Linux/x86_64-unknown-linux-gnu \
    __NV_PRIME_RENDER_OFFLOAD=1 \
    __VK_LAYER_NV_optimus=NVIDIA_only \
    __NV_PRIME_RENDER_OFFLOAD_PROVIDER=NVIDIA-G0 \
    __GLX_VENDOR_LIBRARY_NAME=nvidia \
    VK_ICD_FILENAMES=/usr/share/vulkan/icd.d/nvidia_icd.json \
    CUDA_VISIBLE_DEVICES=0 \
    $ue4/Engine/Binaries/Linux/UE4Editor \
    "$BIN/UEBox.uproject" "$@"
)

