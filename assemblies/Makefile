CC := gcc

# all assembly files
FILES := $(patsubst %.c, %.s, $(filter %.c, $(shell ls)))

all: $(FILES)

%.s: %.c
	$(CC) -O0 -S $^ -o $@

clean:
	rm $(FILES) *~
