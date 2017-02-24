/******************************************************************************
** Copyright (c) 2003 Scott Little <little@cs.utah.edu>
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** To get a copy of the GNU General Public License, write to the 
** Free Software Foundation, Inc., 59 Temple Place - Suite 330, 
** Boston, MA 02111-1307, USA.
**
** File Name     : color.h
** Author        : Scott R. Little
** E-mail        : little@cs.utah.edu
** Date Created  : 01/24/2003
**
** Description   : A header file to help make colored terminal output easier.
**
** Assumptions   :
**
** ToDo          :
**
******************************************************************************/

/*** Usage Notes ***/
/*
  Why use this?  Well, I know that I could do this type of stuff using the
  curses libraries, but the main purpose of this header file is to help
  me when debugging using printf.  Yep, it isn't the most efficient debugging
  mechanism, but it does work.  I can use caps to help draw my attention to
  certain output, but color is so much more efficient so I created this
  little hack.  If you want something that is full featured then I would
  suggest using the curses libs or something like it.

  These notes were created after some short experimentation by me while I
  was creating this header file.  They many not be the same for every
  machine, but it is at least a starting point.  You can use the different
  functions to setup the colors that you would like.  The problem is that
  once you change a setting it will stay that way until it is manually
  changed.  The easiest way to change the setings back to normal I have
  found is cSetAttr(NONE);
*/

#ifndef __color_h__
#define __color_h__

/* attributes for the color selection */
/* Note: I think that DIM is no longer supported, but some places
   I looked said it exists so I added it here.  If it doesn't work
   then it may just no longer be supported. */
#define NONE 00
#define BOLD 01
#define DIM 02
#define UNDERLINE 04
#define BLINK 05
#define REVERSE 07
#define HIDDEN 08

/* available colors for fg and bg */
#define BLACK 0
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5
#define CYAN 6
#define WHITE 7

/*
  The functions should be quite self explanatory.  They should allow you
  to set all combinations of the foreground color, background color, and
  text attribute.
*/
void cSetAll(int attr, int fgColor, int bgColor);
void cSetAttr(int attr);
void cSetFg(int fgColor);
void cSetBg(int bgColor);
void cSetFgBg(int fgColor, int bgColor);
void cSetAttrFg(int attr, int fgColor);
void cSetAttrBg(int attr, int bgColor);

#endif /* color.h */
