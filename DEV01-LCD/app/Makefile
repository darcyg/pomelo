#Makefile

WORK_DIR := $(shell pwd)
SUBMK_FILE := $(WORK_DIR)/subdir.mk
export WORK_DIR SUBMK_FILE

LINKFLAG := 
SHARELIBS := -lpthread -lm -ldl
OBJS := main.o version.o startarg.o
MAINOUT := whsfjzq.out
XZIPOUT := whsfjzq.xzip
SUBDIR := sys lib debug param uplink cenmet monitor plcmet screen 
LOCALLIBS := $(addsuffix /built-in.o,$(SUBDIR)) $(libLCD_API) 
SUBMAKE := $(addsuffix /Makefile, $(SUBDIR))
LOCALLIBS_CLEAN := $(dir $(LOCALLIBS))

all: information $(SUBMAKE) $(MAINOUT)

-include rules.mk

.PHONY: information
information:
	@echo "compiler = "$(CC)

$(MAINOUT): $(OBJS) $(LOCALLIBS)
	@echo "  make  "$@"..."
	@../utils/mkdate ./mkdate.c
	@$(CC) $(CFLAGS) $(DFLAGS) -c mkdate.c -o mkdate.o
	@$(CC) $(OBJS) $(LOCALLIBS) mkdate.o $(LINKFLAG) $(SHARELIBS) -o $@
	@rm -f mkdate.c mkdate.o
	@echo "  make  "$@" ok"
	@../utils/xzip z 3699 $(MAINOUT) $(XZIPOUT)

.PHONY: $(SUBMAKE)
$(SUBMAKE):
#	@echo $(MAKE) "-C" $(dir $@)
	@$(MAKE) -s -C $(dir $@)

.PHONY: $(LOCALLIBS_CLEAN)
$(LOCALLIBS_CLEAN):
	@echo $(MAKE) "-C" $@ "clean"
	@$(MAKE) -s -C $@ clean

.PHONY: clean
clean: $(LOCALLIBS_CLEAN)
	@rm -f $(MAINOUT) $(XZIPOUT) $(OBJS) $(DEPS) *.o .*.c.dep
	@echo remove objs ok

.PHONY: relclean
relclean: $(LOCALLIBS_CLEAN)
	@rm -f $(OBJS) $(DEPS) *.o .*.c.dep
	@echo remove objs ok

