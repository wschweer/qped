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

#ifndef __enter_h__
#define __enter_h__

#define ENTER_SIZE      32

class Ped;

//---------------------------------------------------------
//   EnterEdit
//---------------------------------------------------------

class EnterEdit : public QLineEdit {
      Q_OBJECT

      Ped* ped;
      int enter_head    { 0 };
      int enter_tail;
      int enter_n       { 0 };
      int enter_max     { 0 };
      QString enter_stack[ENTER_SIZE];

//      virtual bool event(QEvent* event);

   public:
      void push();
      QString enter_up();
      QString enter_down();
      EnterEdit(QWidget*, Ped*);
      };

#endif

