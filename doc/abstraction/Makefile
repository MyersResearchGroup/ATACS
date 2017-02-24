###############################################################################
## File Name     : Makefile
## Author        : Kevin Jones
## E-mail        : kevinj@eng.utah.edu
## Date Created  : 05/19/2009
##
## Description   : 
##
## Assumptions   :
##
## ToDo          :
##
###############################################################################

LATEX = latex
PDFLATEX = pdflatex
DVI2PS = dvips
DVIFLAGS = -Ppdf -t letter
VPATH = figs/
FIGDIR = figs/

FIGS := $(wildcard $(FIGDIR)*.fig) 
EPS := $(addsuffix .eps, $(notdir $(basename $(FIGS))))
EPSFIG := $(addsuffix .eps, $(basename $(FIGS)))
PDFFIG := $(addsuffix .pdf, $(basename $(FIGS)))
PDFEPS := $(addsuffix .pdf, $(basename $(EPS)))
FIGTEX := $(addsuffix .figtex, $(basename $(FIGS)))

all: pdf ps

#user.ps: user.tex 
#	$(LATEX) user
#	$(LATEX) user
#	$(DVI2PS) $(DVIFLAGS) -o user.ps user

ps: abstraction.tex $(EPS)
	$(LATEX) abstraction
	$(LATEX) abstraction
	$(DVI2PS) $(DVIFLAGS) -o abstraction.ps abstraction

pdf: abstraction.tex $(PDFFIG) $(EPS) $(FIGTEX)
	$(PDFLATEX) abstraction
	$(PDFLATEX) abstraction

%.eps: %.fig 
	fig2dev -L pstex $< > $@

%.pdf: %.eps
	epstopdf $<

%.figtex: %.fig
	fig2dev -L pstex_t -p $* $< > $@

clean:
	rm -f abstraction.ps \
	abstraction.aux abstraction.toc abstraction.log abstraction.dvi \
	abstraction.pdf $(PDFFIG)
#$(EPS)
