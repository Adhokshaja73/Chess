Requirements To run:
	-> Windows OS
	-> MSYS
	-> Gtk3 (install through MSYS)

To compile:
	-> CodeBlocks IDE (Recommended)
	-> Add this to "Other Compiler Options": 
		`pkg-config --cflags gtk+-3.0`
	-> Add this to "Linker Settings"
		`pkg-config --cflags --libs gtk+-3.0`


Follow this tutorial to setup gtk:
	https://www.youtube.com/watch?v=rUJFYOCbuDg&list=PLN_MPJUQgPVopEfWiFX88_zUzk08ZdrIc