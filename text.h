/***-------------------------------------------------------
 ***  New PEd Editor
 ***  $Id: text.h 1.2 Fri, 12 Jan 2001 12:35:26 +0100 ws $
 ***
 ***  (C) - Copyright Werner Schweer 1997; ws@seh.de
 ***-------------------------------------------------------*/

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
extern Text delete_buffer;

#define RAND  4
#define RAND2 8

#endif

