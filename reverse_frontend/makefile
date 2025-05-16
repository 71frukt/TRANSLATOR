FLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs \
		-Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security \
		-Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo \
		-Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE

SOURCES    = $(wildcard src/*.cpp)
TREE_LIB   = $(wildcard ../tree/src/*.o)
TREE_SAVER = $(wildcard ../tree_saver/*.o)
INCLUDE    = -I lib
BUILD_DIR  = build
EXECUTABLE = reverse_frontend
OBJECTS    = $(SOURCES:.cpp=.o) $(TREE_LIB) $(TREE_SAVER)

all: clean $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	g++ $(FLAGS) $^ -o $(BUILD_DIR)/$@

%.o : %.cpp
	g++ -c $(FLAGS) $(INCLUDE) $< -o $@

clean:
	del src\*.o

rebuild: clean all