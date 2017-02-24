###############################################################################
## File Name     : Makefile
## Author        : Scott R. Little
## E-mail        : little@cs.utah.edu
## Date Created  : 11/13/2002
##
## Description   : A Makefile for the ATACS user's and developer's manuals.
##
## Assumptions   :
##
## ToDo          :
##
###############################################################################

LATEX = latex
PDFLATEX = pdflatex
DVI2PS = dvips
DVIFLAGS = -t letter
VPATH = xform/

FIGS := $(wildcard xform/*.fig) 
EPS := $(addsuffix .eps, $(notdir $(basename $(FIGS))))
PDFFIG := $(addsuffix .pdf, $(notdir $(basename $(FIGS))))

all: user.ps developer.ps

user.ps: user.tex 
	$(LATEX) user
	$(LATEX) user
	$(DVI2PS) $(DVIFLAGS) -o user.ps user

developer.ps: developer.tex xform.tex $(EPS)
	$(LATEX) developer
	$(LATEX) developer
	$(DVI2PS) $(DVIFLAGS) -o developer.ps developer

pdf: user.tex developer.tex xform.tex $(PDFFIG) $(EPS)
	$(PDFLATEX) user
	$(PDFLATEX) user
	$(PDFLATEX) developer
	$(PDFLATEX) developer

%.eps: %.fig 
	fig2dev -L eps $< > $@

%.pdf: %.eps
	epstopdf $<

clean:
	rm -f user.ps user.aux user.toc user.log user.dvi developer.ps \
	developer.aux developer.toc developer.log developer.dvi \
	$(EPS) developer.pdf user.pdf $(PDFFIG)
