XBE_TITLE=kernel\ test\ suite
recursivewildcard=$(foreach d,$(wildcard $(1:=/*)),$(call recursivewildcard,$d,$2) $(filter $(subst *,%,$2),$d))
SRCS = $(call recursivewildcard,src,*.c)
NXDK_CFLAGS = -I$(CURDIR)/src
NXDK_DISABLE_AUTOMOUNT_D = y
include $(NXDK_DIR)/Makefile
