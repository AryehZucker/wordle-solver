TARGET := wordle-solver
SRCDIR ::= src
OBJDIR ::= obj
INCLUDEDIR ::= include

CXX := g++
CXXFLAGS := -MMD -MP -g -Wall -Ofast -I$(INCLUDEDIR)

SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
DEPS := $(OBJS:.o=.d)

#Compile everything
all: $(TARGET)

#Create the target executable
# $@ = left side of the ":"
# $^ = right side of the ":"
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

#Create obj files from their srcs
# $< = first dependency
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $<	-o $@

#Create a directory for the object files
$(OBJDIR):
	mkdir $@

#Delete all object files
clean:
	rm -rfv $(TARGET) $(OBJDIR)

run: $(TARGET)
	./$(TARGET) words/answers.txt words/guesses.txt

#Stops make from doing stuff with a file called "clean" or "all"
.PHONY: all clean run

-include $(DEPS)
