.PHONY: all clean run

CC        = cc
TARGET    = bin\WireWorld

SRCDIR    = src
INCDIR    = include
LIBDIR    = lib
BLDDIR    = build

CFLAGS    = -I$(INCDIR) -fopenmp -O3 -Wall -g
LDFLAGS   = -static -lopengl32 -L$(LIBDIR) -lglfw3

SRC       = $(wildcard   $(SRCDIR)/*.c)
OBJ       = $(subst      $(SRCDIR)/,$(BLDDIR)/,$(SRC:.c=.o))

$(BLDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) -o $@ $^
all: $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) $(LDFLAGS) -o $(TARGET)

native:
	$(MAKE) CFLAGS="$(CFLAGS) -march=native" $(MAKEFLAGS)
run:
	$(TARGET)
clean:
	del /S /F /Q $(BLDDIR)\*
	del $(TARGET).exe