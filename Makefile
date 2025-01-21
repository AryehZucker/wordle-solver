TARGET := wordle-solver
SRCDIR ::= src
OBJDIR ::= obj
INCLUDEDIR ::= include

CXX := g++
# -MMD & -MP generate dependency files so make doesn't need to rebuild the whole package every time
CXXFLAGS := -MMD -MP -Wall -Ofast -I$(INCLUDEDIR)

SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
DEPS := $(OBJS:.o=.d)

#Compile everything
.PHONY: all
all: $(TARGET)

#Create the target executable
# $@ = left side of the ":"
# $^ = right side of the ":"
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

#Create obj files from their srcs
# also depends on the "HEADERS" files
# $< = first dependency
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $<	-o $@

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
