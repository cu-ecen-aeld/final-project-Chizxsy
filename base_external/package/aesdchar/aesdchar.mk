##############################################################
#
# AESD-CHAR DRIVER
#
##############################################################

#TODO: Fill up the contents below in order to reference your assignment 3 git contents
AESDCHAR_VERSION = 31fe8e0074d1e0e2337fbd1fb71c321a9af47c46
# Note: Be sure to reference the *ssh* repository URL here (not https) to work properly
# with ssh keys and the automated build/test system.
# Your site should start with git@github.com:
AESDCHAR_SITE = git@github.com:cu-ecen-aeld/assignments-3-and-later-Chizxsy.git
AESDCHAR_SITE_METHOD = git
AESDCHAR_GIT_SUBMODULES = YES

AESDCHAR_MODULE_SUBDIRS = aesd-char-driver

define AESDCHAR_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 $(@D)/$(AESDCHAR_MODULE_SUBDIRS)/S97aesdchar.sh $(TARGET_DIR)/etc/init.d

	$(INSTALL) -m 0755 $(@D)/$(AESDCHAR_MODULE_SUBDIRS)/aesdchar_load $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 0755 $(@D)/$(AESDCHAR_MODULE_SUBDIRS)/aesdchar_unload $(TARGET_DIR)/usr/bin
endef


$(eval $(kernel-module))

