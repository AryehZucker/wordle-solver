TARGET := firstword

CC := gcc
# -MMD & -MP generate dependency files so make doesn't need to rebuild the whole package every time
CFLAGS := -MMD -MP -Wall -Ofast

OBJDIR ::= obj

SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=$(OBJDIR)/%.o)
DEPS := $(OBJS:.o=.d)
HEADERS := $(wildcard *.h)

#Compile everything
.PHONY: all
all: $(TARGET)

#Create the target executable
# $@ = left side of the ":"
# $^ = right side of the ":"
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

#Create obj files from their srcs
# also depends on the "HEADERS" files
# $< = first dependency
$(OBJDIR)/%.o: %.c $(OBJDIR) $(HEADERS)
	$(CC) $(CFLAGS) -c $<	-o $@

#Create a directory for the object files
$(OBJDIR):
	mkdir $@

#Stops make from doing stuff with a file called "clean"
.PHONY: clean
#Delete all object files
clean:
	rm -rv $(TARGET) $(OBJDIR)

#Include dependencies
-include $(DEPS)
