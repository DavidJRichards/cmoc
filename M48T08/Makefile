# make / makepp

SREC=srec_cat
ASM=../a09
CCC=cmoc

MYFILES=  M48T08-test.srec 

all: $(CC-C) $(MYFILES)

%.srec: %.c
	$(CCC) --intdir=output --srec -i $<

#%.srec:	%.bin
#	$(SREC) -data_only $< -binary -offset 0x400 -o $@

%.hex:	%.bin
	$(SREC)  $< -binary -o $@ -Intel

%.bin:	%.asm
	$(ASM) -o $@ -l $@.lst $<
	
%:	%.asm
	$(ASM) -o $@ -l $@.lst $<

%.ccc:	%.bin
	$(SREC) $< -binary -o $@ -C-array


# convert binary to s-records
#bin2dec.srec:	bin2dec.bin
#	$(SREC) -data_only $< -binary -offset 0x400 -o $@
#	$(SREC) -data_only bin2dec.bin -binary -offset 0x400 -o bin2dec.srec
	
# assemble source code to binary
#bin2dec.bin:	bin2dec.asm
#	$(ASM) -o $@ -l $@.lst $<
#	../a09 -o bin2dec.bin -l bin2dec.lst bin2dec.asm

# convert binary to c data array to include in rom source
#bin2dec.inc:	bin2dec.bin
#	$(SREC) $< -binary -o $@ -C-array
#	srec-cat simon09.bin -binary -o simon09.inc -C-array	

# Link command:
#simon09.srec:	simon09.asm
#	$(SREC) -data_only simon09.bin -binary -offset 0x400 -o simon09.srec

# Compilation commands: 
#simon09.bin:	simon09.asm
#	../a09 -o simon09.bin -l simon09.lst simon09.asm

clean:
	rm *.srec; rm *.bin; rm *.lst; rm *.ccc
