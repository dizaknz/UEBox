#!/bin/bash

set -e

cleanup() {
    find /tmp -type d -name "pulse-*" -prune -exec rm -rvf {} \; || true

    local machineID=$(cat /etc/machine-id)
    local runtimeLink=$HOME/.config/pulse/$machineID-runtime
    [ -L $runtimeLink ] && rm -vf $runtimeLink
}

run() {
    pulseaudio -D --log-level=4 --log-target=stderr --exit-idle-time=-1 --use-pid-file=true --fail=true
    ./UEBox.sh "$@"
}

shutdown() {
    # check if daemon is running
    pulseaudio --log-level=4 --check || return 0

    pulseaudio --log-level=4 --kill

    # wait for shutdown or time out
    wait=0
    while true; do
        let "wait+=1"
        pulseaudio --log-level=4 --check && {
            [ $wait -eq 3 ] && break
            sleep 1
            continue
        } || {
            break
        }
    done
}

onExit() {
    shutdown
    cleanup
}

trap onExit EXIT

run "$@"
