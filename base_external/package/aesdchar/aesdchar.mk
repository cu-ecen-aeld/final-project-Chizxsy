##############################################################
#
# AESD-CHAR DRIVER
#
##############################################################

#TODO: Fill up the contents below in order to reference your assignment 3 git contents
AESDCHAR_VERSION = a29311ee2d07ce05fb967c192223989a30ea132c
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

