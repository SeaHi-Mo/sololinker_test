CURRENT_PATH:=/home/seahi/Desktop/rv1106_sdk_ubuntu_lvgl/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin

build:
	arm-linux-gnueabihf-gcc  gpio_rgb.c -o gpio

send:build
	scp gpio root@192.168.16.115:work/
