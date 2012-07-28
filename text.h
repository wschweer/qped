//=============================================================================
//  PEd Editor
//
//  Copyright (C) 1997-2011 Werner Schweer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENCE.GPL
//=============================================================================

#ifndef __text_h__
#define __text_h__

#include "line.h"

//---------------------------------------------------------
//    text
//    Textstruktur für 'pick/put' und 'del/undelete'
//---------------------------------------------------------

class Text {
      LineList l;             // list of textlines
      bool column_type;

   public:
      Text()     {}
      LineList* read(bool& coltype);
      void write(LineList*, iLineList, iLineList, int, int, bool);
      };

extern Text pick_buffer;

#define RAND  4
#define RAND2 8

#endif

