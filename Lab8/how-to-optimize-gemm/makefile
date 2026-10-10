OLD  := MMult0
NEW  := MMult0

CC         := gcc
LINKER     := $(CC) 

OPT        ?= -O0
CFLAGS     := $(OPT) -Wall -Werror -Wno-unused-result -Wno-unused-value -Wno-unused-variable -g -fdiagnostics-color=auto -fopenmp
LDFLAGS    := -lm -fopenmp
MAKEFLAGS += --no-print-directory    # 关闭Entering directory/Leaving directory 提示

# 1: run naive gemm as reference, 0: only run MY_MMult func defined in $(NEW).c
RUN_REF ?= 0
ifeq ($(RUN_REF), 1)
    CFLAGS += -DRUN_REF
endif

ifeq ($(RUN_REF),1)
  RUN_REF_MSG := RUN_REF = 1, naive GEMM will run as reference
else
  RUN_REF_MSG := RUN_REF = 0, only MY_MMult ($(NEW)) will run
endif


DATA_DIR = _data
BUILD_DIR = _build
OBJS  := $(BUILD_DIR)/util.o $(BUILD_DIR)/REF_MMult.o $(BUILD_DIR)/test_MMult.o $(BUILD_DIR)/$(NEW).o

RED = \033[1;31m
GREEN := \033[1;32m
CYAN  := \033[1;36m
COLOR_RESET  = \033[0m


$(shell mkdir -p $(BUILD_DIR) $(DATA_DIR))

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@) && echo + CC $<
	$(CC) -std=gnu11 $(CFLAGS) -c $< -o $@ 

$(BUILD_DIR)/test_MMult.x: $(OBJS) defs.h
	$(LINKER) $(OBJS) $(LDFLAGS) -o $@  || { printf "$(RED)link FAILED!!!$(COLOR_RESET)\n" >&2; exit 1; }


export OMP_NUM_THREADS=4

.PHONY: all run clean cleanall

all:
	+make clean;
	+make $(BUILD_DIR)/test_MMult.x 

run:
	@# make前面的`+`是为了处理warning: jobserver unavailable: using -j1.  Add '+' to parent make rule
	+make all
	@echo "$(CYAN)$(RUN_REF_MSG)"
	@echo OMP_NUM_THREADS is : $$OMP_NUM_THREADS
	@echo "date = '`date`';" > $(DATA_DIR)/output_$(NEW).m
	@echo "version = '$(NEW)';" >> $(DATA_DIR)/output_$(NEW).m
	$(BUILD_DIR)/test_MMult.x >> $(DATA_DIR)/output_$(NEW).m
	@if [ ! -f $(DATA_DIR)/output_old.m ] || [ $(OLD) != $(NEW) ]; then \
        cp $(DATA_DIR)/output_$(OLD).m $(DATA_DIR)/output_old.m; \
    fi

	@cp $(DATA_DIR)/output_$(NEW).m $(DATA_DIR)/output_new.m

clean:
	rm -rf _build *.jpg

cleanall:
	rm -rf _build _data *.jpg *.png

.PHONY: perf

perf:
	+make all
	perf stat -e \
	cycles,instructions,\
	L1-dcache-loads,\
	L1-dcache-load-misses,\
	L1-icache-load-misses,\
	l2_rqsts.references,\
	l2_rqsts.miss,\
	cache-references,\
	cache-misses,\
	branches,branch-misses \
	$(BUILD_DIR)/test_MMult.x
