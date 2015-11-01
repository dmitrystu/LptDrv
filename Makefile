VER_MAJOR = 3
VER_MINOR = 0
VER_REVSN = 1
VER_VER   = 1


CC = gcc.exe
CXX = g++.exe
AR = ar.exe
LD = g++.exe
WINDRES = windres.exe

INC =
CFLAGS = -Wall -m32 -O2
DFLAGS = -DDRV_MAJOR=$(VER_MAJOR) -DDRV_MINOR=$(VER_MINOR) -DDRV_RELEASE=$(VER_REVSN) -DDRV_VER=$(VER_VER)
RFLAGS = -F pe-i386 -J rc -O coff
LFLAGS = -m32 -shared -Wl,--dll -s


OBJDIR = .\obj
BINDIR = .\bin
SRCDIR  = sources

OUTFILE = $(BINDIR)\LptDrv.dll
SRCFILE = driver.c misc.c main.c
RESFILE = resources.rc


SOURCES = $(addprefix $(SRCDIR)\, $(SRCFILE) $(RESFILE))
OBJECTS = $(addprefix $(OBJDIR)\, $(SRCFILE:.c=.o) $(RESFILE:.rc=.res))

all: pkg pkg-src

release: $(OUTFILE)


pkg: $(OUTFILE) readme.md
	7z.exe a LptDrv_$(VER_MAJOR)$(VER_MINOR)$(VER_REVSN)$(VER_VER).zip $^

pkg-src:
	7z.exe a -r LptDrv_$(VER_MAJOR)$(VER_MINOR)$(VER_REVSN)$(VER_VER)_src.zip readme.md Makefile sources\


$(OUTFILE): $(OBJDIR) $(BINDIR) $(OBJECTS)
	$(LD) $(LFLAGS) $(OBJECTS) -o $@

clean cleanrelease:
	cmd /c if exist $(OBJDIR) del /S/Q $(OBJECTS) > nul
	cmd /c if exist $(BINDIR) del /S/Q $(OUTFILE) > nul


$(OBJDIR)%.o: $(SRCDIR)%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

$(OBJDIR)%.res: $(SRCDIR)%.rc
	$(WINDRES) $(RFLAGS) $(DFLAGS) -i $< -o $@

$(OBJDIR) $(BINDIR):
	cmd /c if not exist $@ md $@


.PHONY: release pkg cleanrelease clean
