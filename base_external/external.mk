AESD_ASSIGNMENTS_y += aesd-assignments
LDD_MODULES_y += ldd

include $(sort $(wildcard $(BR2_EXTERNAL_project_base_PATH)/package/*/*.mk))
