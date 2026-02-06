SRC_FILES = $(wildcard src/*.c)
EXECUTABLE = exec

# raylib
RAYLIB_DIR = thirdparty/raylib
RAYGUI_DIR = thirdparty/raygui
RAYLIB_LIB = $(RAYLIB_DIR)/src/libraylib.a
CFLAGS = -Iinclude -I$(RAYLIB_DIR)/src -I$(RAYGUI_DIR)
LDFLAGS = -L$(RAYLIB_DIR)/src -lraylib -lm -lpthread -ldl -lrt -lX11

build: $(RAYLIB_LIB)
	@gcc -o $(EXECUTABLE) $(SRC_FILES) $(CFLAGS) $(LDFLAGS)

$(RAYLIB_LIB):
	@$(MAKE) -C $(RAYLIB_DIR)/src PLATFORM=PLATFORM_DESKTOP
