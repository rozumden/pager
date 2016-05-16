CC=gcc
CFLAGS=-std=gnu99 -Wall

test: pci.o pci_lcd.o
	$(CC) $(CFLAGS) $^ -o $@
	
pci.o: pci.c pci.h
	$(CC) $(CFLAGS) -c $^

pci_lcd.o: pci_lcd.c pci_lcd.h chmod_lcd.h kbd_hw.h
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f pci.o pci_lcd.o
	rm -f pci.h.gch pci_lcd.h.gch
	rm -f chmod_lcd.h.gch kbd_hw.h.gch

cleanall:
	make clean
	rm -f test
