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
BIBTEX = bibtex
DVI2PS = dvips
DVIFLAGS = -t A4
VPATH = 

FIGS := $(wildcard *.fig) 
EPS := $(addsuffix .eps, $(notdir $(basename $(FIGS))))
PDFFIG := $(addsuffix .pdf, $(notdir $(basename $(FIGS))))

all: hpn.ps

hpn.ps: hpn.tex hpn.bib $(EPS)
	$(LATEX) hpn
	$(BIBTEX) hpn
	$(LATEX) hpn
	$(LATEX) hpn
	$(DVI2PS) $(DVIFLAGS) -o hpn.ps hpn

pdf: hpn.tex $(PDFFIG) $(EPS)
	$(PDFLATEX) hpn
	$(PDFLATEX) hpn

%.eps: %.fig 
	fig2dev -L eps $< > $@

%.pdf: %.eps
	epstopdf $<

clean:
	rm -f hpn.ps hpn.aux hpn.toc hpn.log hpn.dvi \
	$(EPS) hpn.pdf $(PDFFIG)
