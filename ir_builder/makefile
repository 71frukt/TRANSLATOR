#----------- debug mode -------------------------------------------------------------------
USE_DEBUG ?= true
USE_LOGS  ?= true
#------------------------------------------------------------------------------------------


#---------- archive libs ------------------------------------------------------------------
ARCHIVES_NAMES = tree tree_saver logger
ARCHIVES_DIRS  = ../tree ../tree_saver ../logger

ARCHIVES_DEBUG_FLAGS = $(foreach project,  $(ARCHIVES_NAMES),-D $(shell echo $(project) | tr '[:lower:]' '[:upper:]')_DEBUG)  # FIXME USELESS ?
ARCHIVE_NAMES_FLAGS  = $(addprefix -l,     $(ARCHIVES_NAMES))

ARCHIVES_DIRS_INCLUDES    = ../tree/lib   ../tree_saver/lib   ../logger/lib
ARCHIVES_DIRS_LIB_FOLDERS = ../tree/build ../tree_saver/build ../logger/build
ARCHIVES_DIRS_FLAGS       = $(addprefix -L, $(ARCHIVES_DIRS_LIB_FOLDERS))
#------------------------------------------------------------------------------------------


#---------- libs's make args (use postfix '_MAKE_FLAGS') ----------------------------------
TREE_MAKE_FLAGS 	  = LOGS_DIR=$(LOGS_DIR) GRAPH_DIR=$(GRAPH_DIR)
TREE_SAVER_MAKE_FLAGS = 
LOGGER_MAKE_FLAGS     = LOGS_DIR=$(LOGS_DIR)

SUBMAKES_FLAGS = $(foreach archive,$(ARCHIVES_NAMES), $(shell echo $(archive) | tr '[:lower:]' '[:upper:]')_MAKE_FLAGS)
#------------------------------------------------------------------------------------------



#------------ dirs ------------------------------------------------------------------------
SRC_DIR   = src
BUILD_DIR = build
OBJ_DIR   = $(BUILD_DIR)/obj
DEP_DIR   = $(BUILD_DIR)/.deps
LOGS_DIR  = $(BUILD_DIR)/logs
GRAPH_DIR = $(LOGS_DIR)/graphs


RELEASE_DIRS_MK = SRC_DIR BUILD_DIR OBJ_DIR DEP_DIR GRAPH_DIR
DEBUG_DIRS_MK	=
LOGS_DIRS_MK    = LOGS_DIR
#------------------------------------------------------------------------------------------


#----------- define variables -------------------------------------------------------------
BASE_INPUT_AST_FILE_NAME ?= \"../res_tree.ast\"
LOGS_FOLDER_NAME  = \"$(LOGS_DIR)\"

DEFINE_VARS = BASE_INPUT_AST_FILE_NAME LOGS_FOLDER_NAME
#------------------------------------------------------------------------------------------


DEFINE_VARS_FLAGS 	 = $(foreach var,$(DEFINE_VARS),-D $(var)=$($(var)))

DEBUG_FLAGS    = -D IR_DEBUG -D _DEBUG  -ggdb -Og -g3 -D_FORTIFY_SOURCES=3 $(SANITIZER) $(ARCHIVES_DEBUG_FLAGS)
RELEASE_FLAGS  = -DNDEBUG -O3
USE_LOGS_FLAGS = -D USE_LOGS

INCLUDES      = lib $(ARCHIVES_DIRS_INCLUDES)
INCLUDE_FLAGS = $(addprefix -I ,$(INCLUDES))


THIS_PROJ_CPP  = $(wildcard $(SRC_DIR)/*.cpp)
THIS_PROJ_OBJ  = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(THIS_PROJ_CPP))
THIS_PROJ_DEP  = $(patsubst $(SRC_DIR)/%.cpp,$(DEP_DIR)/%.d,$(THIS_PROJ_CPP))

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d

# EXECUTABLE ?= ir_builder
# TARGET      = $(BUILD_DIR)/$(EXECUTABLE) 
TARGET_ARCHIVE = $(BUILD_DIR)/libir_builder.a 

FLAGS  = -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual                											\
 		 -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness              \
 		 -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion       \
 		 -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default         \
 		 -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing              \
 		 -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer         \
 		 -Wlarger-than=81920 -Wstack-usage=81920 -no-pie -Werror=vla

SANITIZER = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,$\
			return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

ifeq ($(USE_DEBUG), true)
	FLAGS += $(DEBUG_FLAGS)
else
	FLAGS += $(RELEASE_FLAGS)
endif

ifeq ($(USE_LOGS), true)
	FLAGS += $(USE_LOGS_FLAGS)
endif

FLAGS += $(DEFINE_VARS_FLAGS)


#-------------------- targets -------------------------------------------------------

.PHONY: all clean rebuild build_libs

all: $(TARGET_ARCHIVE) $(foreach dir, $(RELEASE_DIRS_MK), $($(dir))) $(if $(USE_DEBUG), $(foreach dir, $(DEBUG_DIRS_MK), $($(dir)))) $(if $(USE_LOGS), $(foreach dir, $(LOGS_DIRS_MK), $($(dir))))

$(TARGET_ARCHIVE): $(THIS_PROJ_OBJ)
	ar rcs $@ $^

gdb:
	gdb $(TARGET)

clean:
	rm -rf $(BUILD_DIR)
	
rebuild: clean all

rebuild_all: rebuild_libs rebuild

build_libs:
	$(foreach i, $(shell seq 1 $(words $(ARCHIVES_DIRS))), 										\
		$(eval dir   := $(word $(i),   $(ARCHIVES_DIRS))) 										\
		$(eval flags := $(word $(i),   $(SUBMAKES_FLAGS)))  									\
		$(MAKE) -C $(dir) USE_DEBUG=$(USE_DEBUG) USE_LOGS=$(USE_LOGS) $($(flags)); 				\
	)

rebuild_libs:
	$(foreach i, $(shell seq 1 $(words $(ARCHIVES_DIRS))), 										\
		$(eval dir   := $(word $(i),   $(ARCHIVES_DIRS))) 										\
		$(eval flags := $(word $(i),   $(SUBMAKES_FLAGS)))  									\
		$(MAKE) rebuild -C $(dir) USE_DEBUG=$(USE_DEBUG) USE_LOGS=$(USE_LOGS) $($(flags));		\
	)

$(TARGET): $(THIS_PROJ_OBJ)
	$(CXX) $(CXXFLAGS) $(FLAGS) $(THIS_PROJ_OBJ) $(ARCHIVES_DIRS_FLAGS) $(ARCHIVE_NAMES_FLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR) $(DEP_DIR)
	$(CXX) $(FLAGS) $(addprefix -I ,$(INCLUDES)) $(DEPFLAGS) -c $< -o $@

-include $(THIS_PROJ_DEP)

$(foreach dir, $(RELEASE_DIRS_MK), $(eval $($(dir)): ; @mkdir -p $$@))
$(foreach dir, $(DEBUG_DIRS_MK),   $(eval $($(dir)): ; @mkdir -p $$@))
$(foreach dir, $(LOGS_DIRS_MK),    $(eval $($(dir)): ; @mkdir -p $$@))

#-------------------- end ------------------------------------------------------------