deps_config := \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/app_trace/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/aws_iot/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/bt/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/driver/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/efuse/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/esp32/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/esp_adc_cal/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/esp_event/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/esp_http_client/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/esp_http_server/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/esp_https_ota/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/espcoredump/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/ethernet/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/fatfs/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/freemodbus/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/freertos/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/heap/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/libsodium/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/log/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/lwip/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/mbedtls/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/mdns/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/mqtt/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/nvs_flash/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/openssl/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/pthread/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/spi_flash/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/spiffs/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/tcpip_adapter/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/unity/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/vfs/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/wear_levelling/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/wifi_provisioning/Kconfig \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/app_update/Kconfig.projbuild \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/bootloader/Kconfig.projbuild \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/esptool_py/Kconfig.projbuild \
	/mnt/c/Users/popura/linux_home/research/onolab/blinky/firmware/blinky_ota/firmware/main/Kconfig.projbuild \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/components/partition_table/Kconfig.projbuild \
	/mnt/c/Users/popura/linux_home/research/onolab/esp/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)

ifneq "$(IDF_TARGET)" "esp32"
include/config/auto.conf: FORCE
endif
ifneq "$(IDF_CMAKE)" "n"
include/config/auto.conf: FORCE
endif

$(deps_config): ;
