#!/bin/sh -e
: ${inputdev:=/dev/input/event0} \
  ${low:=10} \
  ${high:=50} \
  ${timeout:=15} \
  ${bl:=/sys/class/backlight/backlight}

type -p mkfifo || export PATH=@coreutils@/bin

daemon_pid=

restart_daemon() {
    [[ -n "$daemon_pid" ]] && kill -HUP "$daemon_pid"
    wait
    export low high timeout
    @brightnessd@ 5<$inputdev 6>$bl/brightness &
    daemon_pid=$!
}

cleanup() {
    rm -f /run/brightnessd
    kill -HUP "$daemon_pid"
}

trap "cleanup" exit

restart_daemon
mkfifo -m 0620 /run/brightnessd
chown root:users /run/brightnessd
while : ; do
    exec 4</run/brightnessd
    while read -t 1 -u 4 name value ; do
        case "$name" in
            low) export low=$value ;;
            high) export high=$value ;;
            timeout) export timeout=$value ;;
        esac
    done
    restart_daemon
done
