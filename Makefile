ENV=seeed_xiao_esp32c3
# ENV=esp32-c3-devkitm-1

build:
	pio run -e $(ENV)

upload:
	pio run -e $(ENV) -t upload

monitor:
	pio device monitor -b 115200

clean:
	pio run -t clean

fullclean:
	rm -rf .pio
	rm src/CMakeLists.txt CMakelists.txt sdkconfig.seeed_xiao_esp32c3 sdkconfig.seeed_xiao_esp32c3.old sdkconfig.esp32-c3-devkitm-1 sdkconfig.esp32-c3-devkitm-1.old

test: clean
	pio test -vvv

rebuild: clean build

all: build upload monitor

call: fullclean build upload monitor
