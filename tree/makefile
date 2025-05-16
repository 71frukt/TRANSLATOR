#----------- debug mode -------------------------------------------------------------------
USE_DEBUG ?= true
#------------------------------------------------------------------------------------------


#---------- archive libs ------------------------------------------------------------------
ARCHIVES_NAMES =
ARCHIVES_DIRS  =
#------------------------------------------------------------------------------------------


#------------ dirs ------------------------------------------------------------------------
SRC_DIR   	= src
BUILD_DIR 	= build
OBJ_DIR   	= $(BUILD_DIR)/obj
DEP_DIR   	= $(BUILD_DIR)/.deps

LOGS_DIR	  	?= $(BUILD_DIR)/logs
GRAPH_DIR	  	?= $(LOGS_DIR)/graphs

RELEASE_DIRS_MK = SRC_DIR BUILD_DIR OBJ_DIR DEP_DIR
DEBUG_DIRS_MK	=
#------------------------------------------------------------------------------------------


#----------- define variables -------------------------------------------------------------
BUILD_FOLDER_NAME = \"$(BUILD_DIR)\"

LOGS_FOLDER_NAME  = \"$(LOGS_DIR)\"
GRAPH_FOLDER_NAME = \"$(GRAPH_DIR)\"

DEFINE_VARS = BUILD_FOLDER_NAME LOGS_FOLDER_NAME GRAPH_FOLDER_NAME
#------------------------------------------------------------------------------------------


ARCHIVES_DEBUG_FLAGS = $(foreach project,$(ARCHIVES_NAMES),-D $(shell echo $(project) | tr '[:lower:]' '[:upper:]')_DEBUG)
ARCHIVE_NAMES_FLAGS  = $(addprefix -l,   $(ARCHIVES_NAMES))
ARCHIVES_DIRS_FLAGS  = $(foreach arch_dir, $(ARCHIVES_DIRS), -L$(arch_dir)/$(FRONTEND_OBJ_DIR))
ARCHIVES_DIRS_LIBS   = $(addsuffix /lib, $(ARCHIVES_DIRS))

DEFINE_VARS_FLAGS 	 = $(foreach var,$(DEFINE_VARS),-D $(var)=$($(var)))

DEBUG_FLAGS   = -D TREE_DEBUG -D _DEBUG  -ggdb -Og -g3 -D_FORTIFY_SOURCES=3 $(SANITIZER) $(ARCHIVES_DEBUG_FLAGS)
RELEASE_FLAGS = -DNDEBUG -O3

INCLUDES      = lib $(ARCHIVES_DIRS_LIBS)
INCLUDE_FLAGS = $(addprefix -I ,$(INCLUDES))


THIS_PROJ_CPP = $(wildcard $(SRC_DIR)/*.cpp)
THIS_PROJ_OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(THIS_PROJ_CPP))
THIS_PROJ_DEP = $(patsubst $(SRC_DIR)/%.cpp,$(DEP_DIR)/%.d,$(THIS_PROJ_CPP))

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d

TARGET_ARCHIVE = $(BUILD_DIR)/libtree.a 

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

.PHONY: all clean rebuild

all: $(TARGET_ARCHIVE) $(foreach dir, $(RELEASE_DIRS_MK), $($(dir))) $(if $(USE_DEBUG), $(foreach dir, $(DEBUG_DIRS_MK), $($(dir))))

$(TARGET_ARCHIVE): $(THIS_PROJ_OBJ)
	ar rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR) $(DEP_DIR)
	$(CXX) $(FLAGS) $(addprefix -I ,$(INCLUDES)) $(DEPFLAGS) -c $< -o $@

-include $(THIS_PROJ_DEP)

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean all

$(foreach dir, $(RELEASE_DIRS_MK), $(eval $($(dir)): ; @mkdir -p $$@))
$(foreach dir, $(DEBUG_DIRS_MK),   $(eval $($(dir)): ; @mkdir -p $$@))

#-------------------- end ------------------------------------------------------------