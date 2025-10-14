##############################################################
#
# AESD-ASSIGNMENTS
#
##############################################################

AESD_ASSIGNMENTS_VERSION = a29311ee2d07ce05fb967c192223989a30ea132c
AESD_ASSIGNMENTS_SITE = git@github.com:cu-ecen-aeld/assignments-3-and-later-Chizxsy.git
AESD_ASSIGNMENTS_SITE_METHOD = git
AESD_ASSIGNMENTS_GIT_SUBMODULES = YES

define AESD_ASSIGNMENTS_BUILD_CMDS
    $(MAKE) -C $(@D)/finder-app all
    $(MAKE) -C $(@D)/server all
endef

define AESD_ASSIGNMENTS_INSTALL_TARGET_CMDS
    # Install finder scripts and writer utility
    $(INSTALL) -m 0755 $(@D)/finder-app/finder.sh $(TARGET_DIR)/usr/bin
    $(INSTALL) -m 0755 $(@D)/finder-app/finder-test.sh $(TARGET_DIR)/usr/bin
    $(INSTALL) -m 0755 $(@D)/finder-app/writer $(TARGET_DIR)/usr/bin

    # Install files for aesdsocket application
    $(INSTALL) -m 0755 $(@D)/server/aesdsocket $(TARGET_DIR)/usr/bin
    $(INSTALL) -m 0755 $(@D)/server/aesdsocket-start-stop.sh $(TARGET_DIR)/etc/init.d/S99aesdsocket
endef

$(eval $(generic-package))
