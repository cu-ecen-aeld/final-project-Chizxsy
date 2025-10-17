# LDD3 Package

LDD_VERSION = 3abe1e0b1b8bf6ce926ab664bbf3ea38a9680ed0

LDD_SITE = git@github.com:cu-ecen-aeld/assignment-7-Chizxsy.git
LDD_SITE_METHOD = git
LDD_GIT_SUBMODULES = YES
LDD_LICENSE = GPL-2.0
LDD_LICENSE_FILES = COPYING

LDD_MODULE_SUBDIRS = scull misc-modules
LDD_MODULE_MAKE_OPTS = KVERSION=$(LINUX_VERSION_PROBED)


define LDD_MODULES_INSTALL_TARGET_CMDS
    $(MAKE) -C $(@D) INSTALL_MOD_PATH=$(TARGET_DIR) modules_install
endef

define LDD_INSTALL_INIT_SCRIPT
	$(INSTALL) -m 0755 $(@D)/scull/scull_load $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 0755 $(@D)/scull/scull_unload $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 0755 $(@D)/misc-modules/module_load $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 0755 $(@D)/misc-modules/module_unload $(TARGET_DIR)/usr/bin

endef

LDD_POST_INSTALL_TARGET_HOOKS += LDD_INSTALL_INIT_SCRIPT

$(eval $(kernel-module))
$(eval $(generic-package))
