#----------- debug mode -------------------------------------------------------------------
USE_DEBUG ?= true
#------------------------------------------------------------------------------------------


#---------- archive libs ------------------------------------------------------------------
ARCHIVES_NAMES = tree tree_saver
ARCHIVES_DIRS  = ../tree ../tree_saver
#------------------------------------------------------------------------------------------


#---------- libs's make args (use postfix '_MAKE_FLAGS') ----------------------------------
TREE_MAKE_FLAGS 	  = LOGS_DIR=$(LOGS_DIR) GRAPH_DIR=$(GRAPH_DIR)
TREE_SAVER_MAKE_FLAGS =

SUBMAKES_FLAGS = $(foreach archive,$(ARCHIVES_NAMES), $(shell echo $(archive) | tr '[:lower:]' '[:upper:]')_MAKE_FLAGS)
#------------------------------------------------------------------------------------------



#------------ dirs ------------------------------------------------------------------------
SRC_DIR   = src
BUILD_DIR = build
OBJ_DIR   = $(BUILD_DIR)/obj
DEP_DIR   = $(BUILD_DIR)/.deps

LOGS_DIR	  	   = $(BUILD_DIR)/logs
GRAPH_DIR	  	   = $(LOGS_DIR)/graphs

RELEASE_DIRS_MK    = SRC_DIR BUILD_DIR OBJ_DIR DEP_DIR
DEBUG_DIRS_MK	   = LOGS_DIR GRAPH_DIR
#------------------------------------------------------------------------------------------


#----------- define variables -------------------------------------------------------------

DEFINE_VARS =
#------------------------------------------------------------------------------------------


ARCHIVES_DEBUG_FLAGS = $(foreach project,  $(ARCHIVES_NAMES),-D $(shell echo $(project) | tr '[:lower:]' '[:upper:]')_DEBUG)
ARCHIVE_NAMES_FLAGS  = $(addprefix -l,     $(ARCHIVES_NAMES))
ARCHIVES_DIRS_LIBS   = $(addsuffix /lib,   $(ARCHIVES_DIRS))
ARCHIVES_DIRS_FLAGS  = $(foreach arch_dir, $(ARCHIVES_DIRS), -L$(arch_dir)/$(BUILD_DIR))

DEFINE_VARS_FLAGS 	 = $(foreach var,$(DEFINE_VARS),-D $(var)=$($(var)))

DEBUG_FLAGS   = -D FRONTEND_DEBUG -D _DEBUG  -ggdb -Og -g3 -D_FORTIFY_SOURCES=3 $(SANITIZER) $(ARCHIVES_DEBUG_FLAGS)
RELEASE_FLAGS = -DNDEBUG -O3

INCLUDES      = lib $(ARCHIVES_DIRS_LIBS)
INCLUDE_FLAGS = $(addprefix -I ,$(INCLUDES))


THIS_PROJ_CPP  = $(wildcard $(SRC_DIR)/*.cpp)
THIS_PROJ_OBJ  = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(THIS_PROJ_CPP))
THIS_PROJ_DEP  = $(patsubst $(SRC_DIR)/%.cpp,$(DEP_DIR)/%.d,$(THIS_PROJ_CPP))

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d

EXECUTABLE ?= frontend
TARGET      = $(BUILD_DIR)/$(EXECUTABLE) 

FLAGS  = -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual                									\
 -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness               \
 -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion         \
 -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default            \
 -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing                  \
 -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer              \
 -Wlarger-than=81920 -Wstack-usage=81920 -no-pie -Werror=vla

SANITIZER = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,$\
			return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

ifeq ($(USE_DEBUG), true)
	FLAGS += $(DEBUG_FLAGS)
else
	FLAGS += $(RELEASE_FLAGS)
endif

FLAGS += $(DEFINE_VARS_FLAGS)


#-------------------- targets -------------------------------------------------------

.PHONY: all clean rebuild build_libs

all: $(TARGET) $(foreach dir, $(RELEASE_DIRS_MK), $($(dir)))  $(if $(USE_DEBUG), $(foreach dir, $(DEBUG_DIRS_MK), $($(dir))))

run:
	$(TARGET)

gdb:
	gdb $(TARGET)

clean:
	rm -rf $(BUILD_DIR)
	
rebuild: clean all

rebuild_all: rebuild_libs rebuild

build_libs:
	$(foreach i, $(shell seq 1 $(words $(ARCHIVES_DIRS))), 					\
		$(eval dir   := $(word $(i),   $(ARCHIVES_DIRS))) 					\
		$(eval flags := $(word $(i),   $(SUBMAKES_FLAGS)))  				\
		$(MAKE) -C $(dir) USE_DEBUG=$(USE_DEBUG) $($(flags)); 				\
	)

rebuild_libs:
	$(foreach i, $(shell seq 1 $(words $(ARCHIVES_DIRS))), 					\
		$(eval dir   := $(word $(i),   $(ARCHIVES_DIRS))) 					\
		$(eval flags := $(word $(i),   $(SUBMAKES_FLAGS)))  				\
		$(MAKE) rebuild -C $(dir) USE_DEBUG=$(USE_DEBUG) $($(flags));		\
	)


$(TARGET): $(THIS_PROJ_OBJ)
	$(CXX) $(CXXFLAGS) $(FLAGS) $(THIS_PROJ_OBJ) $(ARCHIVES_DIRS_FLAGS) $(ARCHIVE_NAMES_FLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR) $(DEP_DIR)
	$(CXX) $(FLAGS) $(addprefix -I ,$(INCLUDES)) $(DEPFLAGS) -c $< -o $@

-include $(THIS_PROJ_DEP)

$(foreach dir, $(RELEASE_DIRS_MK), $(eval $($(dir)): ; @mkdir -p $$@))
$(foreach dir, $(DEBUG_DIRS_MK),   $(eval $($(dir)): ; @mkdir -p $$@))

#-------------------- end ------------------------------------------------------------