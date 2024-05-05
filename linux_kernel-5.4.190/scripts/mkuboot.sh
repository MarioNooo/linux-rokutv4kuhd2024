#!/bin/bash
# SPDX-License-Identifier: GPL-2.0

#
# Build U-Boot image when `mkimage' tool is available.
#
DEST=$(echo ${@} | sed -e 's,^.*-d \([^ ]*\).*$,\1,g')
DIR=$(dirname ${DEST})

${CROSS_COMPILE}objcopy -O binary --remove-section=.bss_noinit vmlinux ${DIR}/vmlinux0.bin
gzip --best --force ${DIR}/vmlinux0.bin
rm -f ${DEST}
mv ${DIR}/vmlinux0.bin.gz ${DEST}

MKIMAGE=$(type -path "${CROSS_COMPILE}mkimage")

if [ -z "${MKIMAGE}" ]; then
	MKIMAGE=$(type -path mkimage)
	if [ -z "${MKIMAGE}" ]; then
		# Doesn't exist
		echo '"mkimage" command not found - U-Boot images will not be built' >&2
		exit 1;
	fi
fi

ARGS=$(echo ${@} | sed -e 's,-C [^ ]*,-C gzip,g')

# Call "mkimage" to create U-Boot image
${MKIMAGE} ${ARGS}
