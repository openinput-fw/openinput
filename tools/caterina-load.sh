#!/usr/bin/env bash

# wait for port to spawn
while :; do
	sleep 0.5
	[ -c "${UPLOAD_PORT}" ] && break
done

# reset to bootloader (magic baudrate)
stty -F "${UPLOAD_PORT}" 1200

# wait for port to respawn
while :; do
	sleep 0.5
	[ -c "${UPLOAD_PORT}" ] && break
done

# upload
avrdude -v -patmega32u4 -cavr109 -P${UPLOAD_PORT} -b57600 -D -Uflash:w:${HEX_FILE}
