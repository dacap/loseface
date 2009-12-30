DOXYGEN = "$(PROGRAMFILES)/doxygen/bin/doxygen.exe"

default: all

all: html

_norule:

html: _norule
	-mkdir html
	$(DOXYGEN) Doxyfile

clean:
	-$(RM) -r html
