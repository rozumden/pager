CC=gcc
CFLAGS=-std=gnu99 -pthread

test: pci.o pci_lcd.o klient.o
	$(CC) $(CFLAGS) $^ -o $@

klient.o: klient.c klient.h
	$(CC) $(CFLAGS) -c $^

pci.o: pci.c pci.h
	$(CC) $(CFLAGS) -c $^

pci_lcd.o: pci_lcd.c pci_lcd.h chmod_lcd.h kbd_hw.h
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f pci.o pci.h.gch
	rm -f pci_lcd.o pci_lcd.h.gch
	rm -f chmod_lcd.h.gch kbd_hw.h.gch
	rm -f klient.o klient.h.gch

cleanall:
	make clean
	rm -f test
