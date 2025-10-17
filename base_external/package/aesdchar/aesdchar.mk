##############################################################
#
# AESD-CHAR DRIVER
#
##############################################################

AESDCHAR_VERSION = 2cf2e47fdf1bc9cc6301f198bcdf03a2853a430f
AESDCHAR_SITE = git@github.com:cu-ecen-aeld/assignments-3-and-later-Chizxsy.git
AESDCHAR_SITE_METHOD = git
AESDCHAR_GIT_SUBMODULES = YES

AESDCHAR_MODULE_SUBDIRS = aesd-char-driver

define AESDCHAR_INSTALL_TARGET_CMDS
    
    # Install driver load/unload scripts
    $(INSTALL) -D -m 0755 $(@D)/aesd-char-driver/aesdchar_load $(TARGET_DIR)/usr/bin/aesdchar_load
    $(INSTALL) -D -m 0755 $(@D)/aesd-char-driver/aesdchar_unload $(TARGET_DIR)/usr/bin/aesdchar_unload
    
    # Fix paths in load script to use installed module location
    sed -i 's|\./${module}.ko|/lib/modules/$$(uname -r)/extra/${module}.ko|g' $(TARGET_DIR)/usr/bin/aesdchar_load
    sed -i 's|insmod ./$module.ko|insmod /lib/modules/$$(uname -r)/extra/$module.ko|g' $(TARGET_DIR)/usr/bin/aesdchar_load
    
endef

$(eval $(kernel-module))
$(eval $(generic-package))
