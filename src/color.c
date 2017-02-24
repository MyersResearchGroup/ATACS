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
** File Name     : color.c
** Author        : Scott R. Little
** E-mail        : little@cs.utah.edu
** Date Created  : 02/26/2003
**
** Description   : The implementation file for colored output to a terminal.
**
** Assumptions   :
**
** ToDo          :
**
******************************************************************************/

#include "color.h"
#include "stdio.h"

#define __COLOR__

void cSetAll(int attr, int fgColor, int bgColor)
{
  char str[12];
  snprintf(str,12,"\033[%d;%d;%dm",attr, fgColor+30, bgColor+40);
  printf("%s", str);
}

void cSetAttr(int attr)
{
#ifdef __COLOR__
  char str[6];
  snprintf(str,6,"\033[%dm",attr);
  printf("%s", str);
#endif
}

void cSetFg(int fgColor)
{
#ifdef __COLOR__
  char str[6];
  snprintf(str,6,"\033[%dm",fgColor+30);
  printf("%s", str);
#endif
}

void cSetBg(int bgColor)
{
  char str[6];
  snprintf(str,6,"\033[%dm",bgColor+40);
  printf("%s", str);
}

void cSetFgBg(int fgColor, int bgColor)
{
  char str[9];
  snprintf(str,9,"\033[%d;%dm",fgColor+30,bgColor+40);
  printf("%s", str);
}

void cSetAttrFg(int attr, int fgColor)
{
  char str[9];
  snprintf(str,9,"\033[%d;%dm",attr,fgColor+30);
  printf("%s", str);
}

void cSetAttrBg(int attr, int bgColor)
{
  char str[9];
  snprintf(str,9,"\033[%d;%dm",attr,bgColor+40);
  printf("%s", str);
}
