qemu-system-x86_64 \
	-device ide-drive,drive=Disk \
	-drive id=Disk,if=none,file=os-image.bin,format=raw \
	-device ide-drive,drive=OS \
	-drive id=OS,if=none,file=disk.raw,format=raw
