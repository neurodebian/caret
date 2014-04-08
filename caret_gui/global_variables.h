/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/


#ifndef __VE_GLOBAL_VARIABLES_H__
#define __VE_GLOBAL_VARIABLES_H__

#ifdef CARET_MAIN_FLAG
#define CARET_EXTERN
#define CARET_INIT_VALUE(x)  = (x)
#define CARET_INIT_VALUE3(x,y,z)  = { (x), (y), (z) }
#else
#define CARET_EXTERN extern
#define CARET_INIT_VALUE(x)
#define CARET_INIT_VALUE3(x,y,z)
#endif

class GuiMainWindow;

/// THE main window
CARET_EXTERN GuiMainWindow* theMainWindow CARET_INIT_VALUE(NULL);

#endif // __VE_GLOBAL_VARIABLES_H__

