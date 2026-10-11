BR_DIR := $(CURDIR)/buildroot
EXT_DIR := $(CURDIR)
OUT_DIR := $(CURDIR)/output

.PHONY: all menuconfig linux-menuconfig clean savedefconfig

all:
	@$(MAKE) -C $(BR_DIR) BR2_EXTERNAL=$(EXT_DIR) O=$(OUT_DIR)

defconfig:
	@$(MAKE) -C $(BR_DIR) BR2_EXTERNAL=$(EXT_DIR) O=$(OUT_DIR) mt62xx_defconfig

menuconfig:
	@$(MAKE) -C $(BR_DIR) BR2_EXTERNAL=$(EXT_DIR) O=$(OUT_DIR) menuconfig

linux-menuconfig:
	@$(MAKE) -C $(BR_DIR) BR2_EXTERNAL=$(EXT_DIR) O=$(OUT_DIR) linux-menuconfig

savedefconfig:
	@$(MAKE) -C $(BR_DIR) BR2_EXTERNAL=$(EXT_DIR) O=$(OUT_DIR) savedefconfig

clean:
	@$(MAKE) -C $(BR_DIR) BR2_EXTERNAL=$(EXT_DIR) O=$(OUT_DIR) clean