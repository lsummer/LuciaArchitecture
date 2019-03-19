
include config.mk
all:
	@for dir in $(MAKE_PATH); \
	do \
		make -C $$dir; \
	done

clean:
	-rm -f lucia
	-rm -f $(BUILD_ROOT)/app/dlink_obj/*.d $(BUILD_ROOT)/app/oobj/*.o
