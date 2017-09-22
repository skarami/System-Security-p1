##############################################################################
#   WARNING: If you change how the targets are compiled in any way, your     #
#            exploits may not work when we test them. So don't do that!      #
##############################################################################

# tools
CC := gcc
CXX := g++
RM := rm -f
CP := cp

# flags
CFLAGS := -std=gnu99 -w -g -O1 -zexecstack -fno-stack-protector -U_FORTIFY_SOURCE
CXXFLAGS := $(CFLAGS)
LDFLAGS :=
LDLIBS := -lcrypt

# sources
targets := target1 target2 target3 target4 target5 target6
installed_targets := $(addprefix /tmp/,$(targets))

# gmake magic
.PHONY:  all clean install

#targets
all: $(targets)

target4: target4.cc
	$(CXX) $(CXXFLAGS) -fstack-protector $(LDFLAGS) -o $@ $<

install: $(targets)
	sudo $(CP) $(targets) /tmp
	sudo chown root:root $(installed_targets)
	sudo chmod 04755 $(installed_targets)

clean:
	$(RM) $(targets)
	sudo $(RM) $(installed_targets)


#dependencies
target5: smalloc.c
