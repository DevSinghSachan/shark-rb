
SHELL = /bin/sh

# V=0 quiet, V=1 verbose.  other values don't work.
V = 0
Q1 = $(V:1=)
Q = $(Q1:0=@)
n=$(NULLCMD)
ECHO1 = $(V:1=@$n)
ECHO = $(ECHO1:0=@echo)

#### Start of system configuration section. ####

srcdir = .
topdir = /Users/jonathanraiman/.rvm/rubies/ruby-1.9.3-p484/include/ruby-1.9.1
hdrdir = /Users/jonathanraiman/.rvm/rubies/ruby-1.9.3-p484/include/ruby-1.9.1
arch_hdrdir = /Users/jonathanraiman/.rvm/rubies/ruby-1.9.3-p484/include/ruby-1.9.1/$(arch)
VPATH = $(srcdir):$(arch_hdrdir)/ruby:$(hdrdir)/ruby
prefix = $(DESTDIR)/Users/jonathanraiman/.rvm/rubies/ruby-1.9.3-p484
rubylibprefix = $(libdir)/$(RUBY_BASE_NAME)
exec_prefix = $(prefix)
vendorhdrdir = $(rubyhdrdir)/vendor_ruby
sitehdrdir = $(rubyhdrdir)/site_ruby
rubyhdrdir = $(includedir)/$(RUBY_BASE_NAME)-$(ruby_version)
vendordir = $(rubylibprefix)/vendor_ruby
sitedir = $(rubylibprefix)/site_ruby
ridir = $(datarootdir)/$(RI_BASE_NAME)
mandir = $(datarootdir)/man
localedir = $(datarootdir)/locale
libdir = $(exec_prefix)/lib
psdir = $(docdir)
pdfdir = $(docdir)
dvidir = $(docdir)
htmldir = $(docdir)
infodir = $(datarootdir)/info
docdir = $(datarootdir)/doc/$(PACKAGE)
oldincludedir = $(DESTDIR)/usr/include
includedir = $(prefix)/include
localstatedir = $(prefix)/var
sharedstatedir = $(prefix)/com
sysconfdir = $(prefix)/etc
datadir = $(datarootdir)
datarootdir = $(prefix)/share
libexecdir = $(exec_prefix)/libexec
sbindir = $(exec_prefix)/sbin
bindir = $(exec_prefix)/bin
rubylibdir = $(rubylibprefix)/$(ruby_version)
archdir = $(rubylibdir)/$(arch)
sitelibdir = $(sitedir)/$(ruby_version)
sitearchdir = $(sitelibdir)/$(sitearch)
vendorlibdir = $(vendordir)/$(ruby_version)
vendorarchdir = $(vendorlibdir)/$(sitearch)

NULLCMD = :

CC = /usr/local/opt/gcc46/bin/gcc-4.6
CXX = g++
LIBRUBY = $(LIBRUBY_SO)
LIBRUBY_A = lib$(RUBY_SO_NAME)-static.a
LIBRUBYARG_SHARED = -l$(RUBY_SO_NAME)
LIBRUBYARG_STATIC = -l$(RUBY_SO_NAME)-static
empty =
OUTFLAG = -o $(empty)
COUTFLAG = -o $(empty)

RUBY_EXTCONF_H = 
cflags   =  $(optflags) $(debugflags) $(warnflags)
optflags = -O3
debugflags = -ggdb
warnflags = -Wall -Wextra -Wno-unused-parameter -Wno-parentheses -Wno-long-long -Wno-missing-field-initializers -Wpointer-arith -Wwrite-strings -Wdeclaration-after-statement -Wimplicit-function-declaration
CFLAGS   = -fno-common $(cflags)  -fno-common -pipe $(ARCH_FLAG)
INCFLAGS = -I. -I$(arch_hdrdir) -I$(hdrdir)/ruby/backward -I$(hdrdir) -I$(srcdir)
DEFS     = 
CPPFLAGS = -DHAVE_RUBY_H -D_XOPEN_SOURCE -D_DARWIN_C_SOURCE $(DEFS) $(cppflags) -I/usr/local/Cellar/libyaml/0.1.4/include -I/usr/local/opt/readline/include -I/usr/local/opt/libksba/include -I/usr/local/opt/openssl/include
CXXFLAGS = $(CFLAGS) $(cxxflags)
ldflags  = -L. -L/usr/local/lib -L/usr/local/Cellar/libyaml/0.1.4/lib -L/usr/local/opt/readline/lib -L/usr/local/opt/libksba/lib -L/usr/local/opt/openssl/lib 
dldflags = -Wl,-undefined,dynamic_lookup -Wl,-multiply_defined,suppress -Wl,-flat_namespace -L/usr/local/Cellar/libyaml/0.1.4/lib -L/usr/local/opt/readline/lib -L/usr/local/opt/libksba/lib -L/usr/local/opt/openssl/lib  
ARCH_FLAG = 
DLDFLAGS = $(ldflags) $(dldflags) $(ARCH_FLAG)
LDSHARED = $(CC) -dynamic -bundle
LDSHAREDXX = $(CXX) -dynamic -bundle
AR = ar
EXEEXT = 

RUBY_BASE_NAME = ruby
RUBY_INSTALL_NAME = ruby
RUBY_SO_NAME = ruby.1.9.1
arch = x86_64-darwin13.0.0
sitearch = $(arch)
ruby_version = 1.9.1
ruby = /Users/jonathanraiman/.rvm/rubies/ruby-1.9.3-p484/bin/ruby
RUBY = $(ruby)
RM = rm -f
RM_RF = $(RUBY) -run -e rm -- -rf
RMDIRS = rmdir -p
MAKEDIRS = /usr/local/bin/gmkdir -p
INSTALL = /usr/bin/install -c
INSTALL_PROG = $(INSTALL) -m 0755
INSTALL_DATA = $(INSTALL) -m 644
COPY = cp
TOUCH = exit >

#### End of system configuration section. ####

preload = 

libpath = . $(libdir) /usr/local/Cellar/libyaml/0.1.4/lib /usr/local/opt/readline/lib /usr/local/opt/libksba/lib /usr/local/opt/openssl/lib
LIBPATH =  -L. -L$(libdir) -L/usr/local/Cellar/libyaml/0.1.4/lib -L/usr/local/opt/readline/lib -L/usr/local/opt/libksba/lib -L/usr/local/opt/openssl/lib
DEFFILE = 

CLEANFILES = mkmf.log
DISTCLEANFILES = 
DISTCLEANDIRS = 

extout = 
extout_prefix = 
target_prefix = 
LOCAL_LIBS = 
LIBS = $(LIBRUBYARG_SHARED) -lstdc++ -lboost_serialization-mt -lboost_system-mt -lboost_filesystem-mt -lboost_thread-mt -lblas -lshark  -lpthread -ldl -lobjc 
SRCS = Optimizer.cpp rb_BarsAndStripes.cpp rb_BinaryCD.cpp rb_BinaryRBM.cpp rb_conversions.cpp rb_ExactGradient.cpp rb_GaussianBinaryRBM.cpp rb_LBFGS.cpp rb_LinearModel.cpp rb_PCA.cpp rb_RBM_Analytics.cpp rb_RBMBinaryLayer.cpp rb_RBMGaussianLayer.cpp rb_RealMatrix.cpp rb_RealMatrixColumn.cpp rb_RealMatrixReference.cpp rb_RealMatrixRow.cpp rb_RealVector.cpp rb_RealVectorReference.cpp rb_RegressionDataset.cpp rb_RNG.cpp rb_RNG_Bernoulli.cpp rb_RNG_Binomial.cpp rb_RNG_Cauchy.cpp rb_RNG_DiffGeometric.cpp rb_RNG_Dirichlet.cpp rb_RNG_DiscreteUniform.cpp rb_RNG_Erlang.cpp rb_RNG_Geometric.cpp rb_RNG_LogNormal.cpp rb_RNG_NegExponential.cpp rb_RNG_Normal.cpp rb_RNG_Poisson.cpp rb_RNG_TruncatedExponential.cpp rb_RNG_Uniform.cpp rb_RNG_Weibull.cpp rb_shark.cpp rb_Softmax.cpp rb_SolutionSet.cpp rb_SteepestDescent.cpp rb_UnlabeledData.cpp
OBJS = Optimizer.o rb_BarsAndStripes.o rb_BinaryCD.o rb_BinaryRBM.o rb_conversions.o rb_ExactGradient.o rb_GaussianBinaryRBM.o rb_LBFGS.o rb_LinearModel.o rb_PCA.o rb_RBM_Analytics.o rb_RBMBinaryLayer.o rb_RBMGaussianLayer.o rb_RealMatrix.o rb_RealMatrixColumn.o rb_RealMatrixReference.o rb_RealMatrixRow.o rb_RealVector.o rb_RealVectorReference.o rb_RegressionDataset.o rb_RNG.o rb_RNG_Bernoulli.o rb_RNG_Binomial.o rb_RNG_Cauchy.o rb_RNG_DiffGeometric.o rb_RNG_Dirichlet.o rb_RNG_DiscreteUniform.o rb_RNG_Erlang.o rb_RNG_Geometric.o rb_RNG_LogNormal.o rb_RNG_NegExponential.o rb_RNG_Normal.o rb_RNG_Poisson.o rb_RNG_TruncatedExponential.o rb_RNG_Uniform.o rb_RNG_Weibull.o rb_shark.o rb_Softmax.o rb_SolutionSet.o rb_SteepestDescent.o rb_UnlabeledData.o
TARGET = rb_shark
TARGET_NAME = rb_shark
TARGET_ENTRY = Init_$(TARGET_NAME)
DLLIB = $(TARGET).bundle
EXTSTATIC = 
STATIC_LIB = 

BINDIR        = $(bindir)
RUBYCOMMONDIR = $(sitedir)$(target_prefix)
RUBYLIBDIR    = $(sitelibdir)$(target_prefix)
RUBYARCHDIR   = $(sitearchdir)$(target_prefix)
HDRDIR        = $(rubyhdrdir)/ruby$(target_prefix)
ARCHHDRDIR    = $(rubyhdrdir)/$(arch)/ruby$(target_prefix)

TARGET_SO     = $(DLLIB)
CLEANLIBS     = $(TARGET).bundle 
CLEANOBJS     = *.o  *.bak

all:    $(DLLIB)
static: $(STATIC_LIB)
.PHONY: all install static install-so install-rb
.PHONY: clean clean-so clean-rb

clean-static::
clean-rb-default::
clean-rb::
clean-so::
clean: clean-so clean-static clean-rb-default clean-rb
		-$(Q)$(RM) $(CLEANLIBS) $(CLEANOBJS) $(CLEANFILES) .*.time

distclean-rb-default::
distclean-rb::
distclean-so::
distclean: clean distclean-so distclean-rb-default distclean-rb
		@-$(RM) Makefile $(RUBY_EXTCONF_H) conftest.* mkmf.log
		@-$(RM) core ruby$(EXEEXT) *~ $(DISTCLEANFILES)
		@-$(RMDIRS) $(DISTCLEANDIRS) 2> /dev/null || true

realclean: distclean
install: install-so install-rb

install-so: $(RUBYARCHDIR)/$(DLLIB)
$(RUBYARCHDIR)/$(DLLIB): $(DLLIB)
	-$(Q)$(MAKEDIRS) $(@D)
	$(INSTALL_PROG) $(DLLIB) $(@D)
clean-static::
	-$(Q)$(RM) $(STATIC_LIB)
install-rb: pre-install-rb install-rb-default
install-rb-default: pre-install-rb-default
pre-install-rb: Makefile
pre-install-rb-default: Makefile
pre-install-rb-default:
	$(ECHO) installing default rb_shark libraries
./.RUBYARCHDIR.time:
	$(Q) $(MAKEDIRS) $(RUBYARCHDIR)
	$(Q) $(TOUCH) $@

site-install: site-install-so site-install-rb
site-install-so: install-so
site-install-rb: install-rb

.SUFFIXES: .c .m .cc .mm .cxx .cpp .C .o

.cc.o:
	$(ECHO) compiling $(<)
	$(Q) $(CXX) $(INCFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(COUTFLAG)$@ -c $<

.mm.o:
	$(ECHO) compiling $(<)
	$(Q) $(CXX) $(INCFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(COUTFLAG)$@ -c $<

.cxx.o:
	$(ECHO) compiling $(<)
	$(Q) $(CXX) $(INCFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(COUTFLAG)$@ -c $<

.cpp.o:
	$(ECHO) compiling $(<)
	$(Q) $(CXX) $(INCFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(COUTFLAG)$@ -c $<

.C.o:
	$(ECHO) compiling $(<)
	$(Q) $(CXX) $(INCFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(COUTFLAG)$@ -c $<

.c.o:
	$(ECHO) compiling $(<)
	$(Q) $(CC) $(INCFLAGS) $(CPPFLAGS) $(CFLAGS) $(COUTFLAG)$@ -c $<

.m.o:
	$(ECHO) compiling $(<)
	$(Q) $(CC) $(INCFLAGS) $(CPPFLAGS) $(CFLAGS) $(COUTFLAG)$@ -c $<

$(DLLIB): $(OBJS) Makefile
	$(ECHO) linking shared-object $(DLLIB)
	-$(Q)$(RM) $(@)
	$(Q) $(LDSHAREDXX) -o $@ $(OBJS) $(LIBPATH) $(DLDFLAGS) $(LOCAL_LIBS) $(LIBS)



$(OBJS): $(hdrdir)/ruby.h $(hdrdir)/ruby/defines.h $(arch_hdrdir)/ruby/config.h
