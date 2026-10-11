BR_DIR      := $(CURDIR)/buildroot
EXT_DIR     := $(CURDIR)
OUT_DIR     := $(CURDIR)/output
LINUX_DIR   := $(CURDIR)/linux

ARCH          := arm
CROSS_COMPILE := $(OUT_DIR)/host/bin/arm-buildroot-uclinux-uclibcgnueabi-
LINUX_CONF    := $(EXT_DIR)/board/mt62xx/linux.config

.PHONY: all buildroot linux br-menuconfig linux-menuconfig clean savedefconfig defconfig

all: buildroot linux

buildroot:
	@echo ">>> Building Buildroot (Toolchain + RootFS)..."
	@$(MAKE) -C $(BR_DIR) BR2_EXTERNAL=$(EXT_DIR) O=$(OUT_DIR)

linux:
	@echo ">>> Building Linux Kernel..."
	@$(MAKE) -C $(LINUX_DIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) KCONFIG_CONFIG=$(LINUX_CONF) HOSTCC="gcc -fcommon" -j$(shell nproc)
	@cp $(LINUX_DIR)/arch/arm/boot/Image $(OUT_DIR)/
	@echo ">>> Kernel Image is ready in output/"
	
defconfig:
	@$(MAKE) -C $(BR_DIR) BR2_EXTERNAL=$(EXT_DIR) O=$(OUT_DIR) mt62xx_defconfig

br-menuconfig:
	@$(MAKE) -C $(BR_DIR) BR2_EXTERNAL=$(EXT_DIR) O=$(OUT_DIR) menuconfig

br-savedefconfig:
	@$(MAKE) -C $(BR_DIR) BR2_EXTERNAL=$(EXT_DIR) O=$(OUT_DIR) savedefconfig

linux-menuconfig:
	@$(MAKE) -C $(LINUX_DIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) KCONFIG_CONFIG=$(LINUX_CONF) menuconfig

linux-savedefconfig:
	@$(MAKE) -C $(LINUX_DIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) KCONFIG_CONFIG=$(LINUX_CONF) savedefconfig

clean:
	@echo ">>> Cleaning Buildroot output..."
	@$(MAKE) -C $(BR_DIR) BR2_EXTERNAL=$(EXT_DIR) O=$(OUT_DIR) clean
	@echo ">>> Cleaning Linux build..."
	@$(MAKE) -C $(LINUX_DIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) clean

%:
	@$(MAKE) -C $(BR_DIR) BR2_EXTERNAL=$(EXT_DIR) O=$(OUT_DIR) $@
