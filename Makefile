.PHONY: all clean run

CC        = cc
TARGET    = a

SRCDIR    = src
INCDIR    = include
LIBDIR    = lib
BLDDIR    = build

CFLAGS    = -I$(INCDIR) -O3 -Wall -g
LDFLAGS   = -static -L$(LIBDIR)

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