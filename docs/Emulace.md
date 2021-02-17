# Emulace Raspberry Pi s QEMU
- Emulátor pro QEMU - https://github.com/dhruvvyas90/qemu-rpi-kernel
````shell
./qemu-system-arm -kernel kernel-qemu-5.4.51-buster -cpu arm1176 -m 256         
  -M versatilepb -no-reboot -serial stdio                                     
  -append "root=/dev/sda2 panic=1 rootfstype=ext4 rw init=/bin/bash"          
  -drive "file=2021-01-11-raspios-buster-armhf-lite.img,index=0,media=disk,format=raw"  
  -redir tcp:2222::22
````
