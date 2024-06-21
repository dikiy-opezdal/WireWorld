.PHONY: all clean

CC        = cc
TARGET    = WireWorld

SRCDIR    = src
INCDIR    = include
LIBDIR    = lib
BLDDIR    = build

CFLAGS    = -I$(INCDIR) -fopenmp -O3 -Wall -g
LDFLAGS   = -static -lopengl32 -L$(LIBDIR) -lglfw3

SRC       = $(wildcard   $(SRCDIR)/*.c)
OBJ       = $(subst      $(SRCDIR)/,$(BLDDIR)/,$(SRC:.c=.o))

$(BLDDIR)/%.o: $(SRCDIR)/%.c | $(BLDDIR)
	$(CC) -c $(CFLAGS) -o $@ $^
all: $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) $(LDFLAGS) -o $(BLDDIR)/$(TARGET)

clean:
	del /Q $(BLDDIR)
$(BLDDIR):
	mkdir $(BLDDIR)