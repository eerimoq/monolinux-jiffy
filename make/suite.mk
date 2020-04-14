BUILD = build
EXE = $(BUILD)/suite
CFLAGS += -fno-omit-frame-pointer
ifeq ($(SANITIZE), yes)
CFLAGS += -fsanitize=address
CFLAGS += -fsanitize=undefined
endif
CFLAGS += -coverage
CFLAGS += -Wall -Wextra -std=gnu11
CFLAGS += -g -O0
CFLAGS += -DUNIT_TEST
CFLAGS += -no-pie
CFLAGS += -DCURL_DISABLE_TYPECHECK
COVERAGE_FILTERS +=
INC += $(ML_JIFFY_ROOT)/include
INC += $(shell $(NALA) include_dir)
INC += $(ML_JIFFY_ROOT)/3pp/curl/include
INC += $(ML_JIFFY_ROOT)/3pp/monolinux-c-library/include
INC += $(ML_JIFFY_ROOT)/3pp/punchboot/src/include
INC += $(BUILD)
SRC += $(shell $(NALA) c_sources)
SRC += $(ML_JIFFY_ROOT)/src/http_get.c
SRC += $(ML_JIFFY_ROOT)/src/pbconfig.c
SRC += $(BUILD)/nala_mocks.c
SRC += $(TESTS)
TESTS ?= main
TESTS_C = $(BUILD)/suites.c
NALA ?= nala
LSAN_OPTIONS = \
	suppressions=$(ML_JIFFY_ROOT)/make/lsan-suppressions.txt \
	print_suppressions=0

OBJ = $(patsubst %,$(BUILD)%,$(abspath $(SRC:%.c=%.o)))

CFLAGS += $(INC:%=-I%)
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -D_GNU_SOURCE=1
CFLAGS += $(CFLAGS_EXTRA)

LDFLAGS += -Wl,--gc-sections

DEPSDIR = $(BUILD)/deps

.PHONY: all run build coverage clean

all: run

build:
	$(MAKE) $(BUILD)/nala_mocks.ldflags
	$(MAKE) $(EXE)

run: build
	LSAN_OPTIONS="$(LSAN_OPTIONS)" $(EXE) $(ARGS)

test: run
	$(MAKE) coverage

$(BUILD)/nala_mocks.ldflags: $(TESTS)
	echo "MOCKGEN $^"
	mkdir -p $(BUILD)
	[ -f $(BUILD)/nala_mocks.h ] || touch $(BUILD)/nala_mocks.h
	cat $(TESTS) > $(TESTS_C)
	$(CC) $(INC:%=-I%) \
	    -D_GNU_SOURCE=1 -DCURL_DISABLE_TYPECHECK -DNALA_GENERATE_MOCKS \
	    -E $(TESTS_C) | $(NALA) generate_mocks \
					$(NO_IMPLEMENTATION:%=-n %) \
					-o $(BUILD)
	touch $@

coverage:
	gcovr --root ../.. \
	    --exclude-directories ".*tst.*" $(COVERAGE_FILTERS:%=-f %) \
	    --html-details --output index.html $(BUILD)
	mkdir -p $(BUILD)/coverage
	mv index.* $(BUILD)/coverage
	@echo
	@echo "Code coverage report: $$(readlink -f $(BUILD)/coverage/index.html)"
	@echo

clean:
	rm -rf $(BUILD)

$(EXE): $(OBJ)
	@echo "LD $@"
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) @$(BUILD)/nala_mocks.ldflags -lpthread -o $@

define COMPILE_template
-include $(patsubst %.c,$(DEPSDIR)%.o.dep,$(abspath $1))
$(patsubst %.c,$(BUILD)%.o,$(abspath $1)): $1
	@echo "CC $1"
	mkdir -p $(BUILD)$(abspath $(dir $1))
	mkdir -p $(DEPSDIR)$(abspath $(dir $1))
	$$(CC) $$(CFLAGS) -c -o $$@ $$<
	gcc -MM -MT $$@ $$(CFLAGS) -o $(patsubst %.c,$(DEPSDIR)%.o.dep,$(abspath $1)) $$<
endef
$(foreach file,$(SRC),$(eval $(call COMPILE_template,$(file))))
