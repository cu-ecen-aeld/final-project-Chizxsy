##############################################################
#
# AESD-ASSIGNMENTS
#
##############################################################

AESD_ASSIGNMENTS_VERSION = 2cf2e47fdf1bc9cc6301f198bcdf03a2853a430f
AESD_ASSIGNMENTS_SITE = git@github.com:cu-ecen-aeld/assignments-3-and-later-Chizxsy.git
AESD_ASSIGNMENTS_SITE_METHOD = git
AESD_ASSIGNMENTS_GIT_SUBMODULES = YES

define AESD_ASSIGNMENTS_BUILD_CMDS
    $(TARGET_CC) $(TARGET_CFLAGS) -DUSE_AESD_CHAR_DEVICE=1 -o $(@D)/server/aesdsocket $(@D)/server/aesdsocket.c -lpthread
endef

define AESD_ASSIGNMENTS_INSTALL_TARGET_CMDS
	$(INSTALL) -d 0755 $(@D)/conf/ $(TARGET_DIR)/etc/finder-app/conf/
	$(INSTALL) -m 0755 $(@D)/conf/* $(TARGET_DIR)/etc/finder-app/conf/
	$(INSTALL) -m 0755 $(@D)/assignment-autotest/test/assignment5-buildroot/* $(TARGET_DIR)/bin
	$(INSTALL) -m 0755 $(@D)/server/aesdsocket $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 0755 $(@D)/server/aesdsocket-start-stop $(TARGET_DIR)/etc/init.d/S99aesdsocket
endef

$(eval $(generic-package))
