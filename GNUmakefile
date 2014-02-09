# vim:ft=make
#
_bootstrap_Makefile := $(wildcard [M]akefile)
_bootstrap_config-status := $(wildcard config.status)

ALL_RECURSIVE_TARGETS=

ifneq ($(_bootstrap_Makefile),)
  include Makefile
else
  ifneq ($(_bt_config-status),)
    $(srcdir)/config.status
    $(MAKE) $(AM_MAKEFLAGS) configure
  endif

.DEFAULT_GOAL:= basic_build
srcdir= .

configure: bootstrap.sh
	@$(srcdir)/bootstrap.sh -a

Makefile: configure
	@$(srcdir)/bootstrap.sh -c

.PHONY: basic_build
basic_build: Makefile
	@$(MAKE) $(AM_MAKEFLAGS)
endif

ALL_RECURSIVE_TARGETS+= $(AM_RECURSIVE_TARGETS)

ifneq ($(word 2, $(MAKECMDGOALS)), )
ifneq ($(filter $(ALL_RECURSIVE_TARGETS), $(MAKECMDGOALS)), )
.NOTPARALLEL:
endif
endif
