add_executable(VersaTerm
	main.c
        framebuf.c
        framebuf_vga.cpp
        framebuf_dvi.c
        font.c
        terminal.c
        keyboard.c
        keyboard_usb.c
        keyboard_ps2.c
        config.c
        xmodem.c
        flash.c
        serial.c
        serial_uart.c
        serial_cdc.c
		sound.c
		device.c
		key.c
		led.c
	tmds_encode_font_2bpp.S
	tmds_encode_font_2bpp.h

	boards/extdisp/device.c
	boards/extdisp/serial_uart_pio.c
)

target_compile_definitions(VersaTerm PRIVATE
	DVI_VERTICAL_REPEAT=1
	)

target_include_directories(VersaTerm PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

# We have a lot in SRAM4 (particularly TMDS LUT) but don't need much stack on
# core 1. Probably even 256 bytes would be fine.
#
# Select board:
# - 1 - original VersaTerm
# - 2 - PicoPad external display module v1
target_compile_definitions(VersaTerm PRIVATE
	BOARD=1
	PICO_CORE1_STACK_SIZE=0x200
	)

target_link_libraries(VersaTerm
	pico_stdlib
	pico_multicore
	hardware_flash
	libdvi
        PicoVGA
        tinyusb_host
        tinyusb_device
)

# create map/bin/hex file etc.
pico_add_extra_outputs(VersaTerm)
