TARGET=fabemd

CC=g++

CFLAGS=-Wall -ansi -pedantic -ffast-math -I /usr/X11R6/include -I ./CImg -O3
LDFLAGS=-lm -lpthread -I/usr/X11R6/include -L/usr/X11R6/lib -lm -lpthread -lX11

SRCDIR = src
OBJDIR = obj
BINDIR = bin

SOURCES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

all: $(OBJDIR) $(BINDIR) $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(CC) -o $@ $^ $(LDFLAGS)

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper dirs

$(BINDIR):
	@mkdir $(BINDIR)
	
$(OBJDIR):
	@mkdir $(DIR)

clean:
	@rm -rf $(OBJECTS)

mrproper: clean
	@rm -rf  $(BINDIR)/$(TARGET)
