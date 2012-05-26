/***-------------------------------------------------------
 ***  PEd Editor
 ***        KDE - Version
 ***  $Id:$
 ***
 ***  (C) - Copyright Werner Schweer 2005; ws@seh.de
 ***-------------------------------------------------------*/

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
      int enter_head;
      int enter_tail;
      int enter_n;
      int enter_max;
      QString enter_stack[ENTER_SIZE];
      QString enter_up();
      QString enter_down();
      void push();

      virtual bool event(QEvent* event);

   public:
      EnterEdit(QWidget*, Ped*);
      };

#endif

