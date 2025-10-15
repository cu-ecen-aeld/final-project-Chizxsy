##############################################################
#
# AESD-CHAR DRIVER
#
##############################################################

AESDCHAR_VERSION = 4f1fb9a2deca4fb6d2beba0f782b027ac9fe2f69
AESDCHAR_SITE = git@github.com:cu-ecen-aeld/assignments-3-and-later-Chizxsy.git
AESDCHAR_SITE_METHOD = git
AESDCHAR_GIT_SUBMODULES = YES

AESDCHAR_MODULE_SUBDIRS = aesd-char-driver

define AESDCHAR_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) ARCH=$(KERNEL_ARCH) CROSS_COMPILE=$(TARGET_CROSS) -C $(@D)/aesd-char-driver all
endef

define AESDCHAR_INSTALL_TARGET_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) ARCH=$(KERNEL_ARCH) CROSS_COMPILE=$(TARGET_CROSS) -C $(@D)/aesd-char-driver INSTALL_MOD_PATH=$(TARGET_DIR) modules_install
	$(INSTALL) -m 0755 $(@D)/$(AESDCHAR_MODULE_SUBDIRS)/S97aesdchar.sh $(TARGET_DIR)/etc/init.d

	$(INSTALL) -m 0755 $(@D)/$(AESDCHAR_MODULE_SUBDIRS)/aesdchar_load $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 0755 $(@D)/$(AESDCHAR_MODULE_SUBDIRS)/aesdchar_unload $(TARGET_DIR)/usr/bin
endef

$(eval $(kernel-module))

$(eval $(generic-package))
