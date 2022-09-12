BUILD_DIR=build


all: $(BUILD_DIR)
	meson compile -C $(BUILD_DIR) -v

$(BUILD_DIR):
	meson setup $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
