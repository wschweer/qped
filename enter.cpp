/***-------------------------------------------------------
 ***  PEd Editor
 ***        KDE - Version
 ***  $Id:$
 ***
 ***  (C) - Copyright Werner Schweer 2005; ws@seh.de
 ***-------------------------------------------------------*/

#include "ped.h"
#include "enter.h"
#include "cmd.h"
#include "utils.h"

//---------------------------------------------------------
//   EnterEdit
//---------------------------------------------------------

EnterEdit::EnterEdit(QWidget* parent, Ped* p)
   : QLineEdit(parent)
      {
      ped = p;
      enter_head = 0;
      enter_n = 0;
      enter_max = 0;
      }

//---------------------------------------------------------
//   enter_up
//---------------------------------------------------------

QString EnterEdit::enter_up()
      {
      if (enter_n == 0)       // wenn keine historie
            return QString("");

	enter_stack[enter_head] = text();

      --enter_head;
      --enter_n;
      enter_head %= ENTER_SIZE;
      return enter_stack[enter_head];
      }

//---------------------------------------------------------
//   enter_down
//---------------------------------------------------------

QString EnterEdit::enter_down()
      {
	if (enter_max != enter_head) {
	      enter_stack[enter_head] = text();
            ++enter_head;
            ++enter_n;
            enter_head %= ENTER_SIZE;
            return enter_stack[enter_head];
            }
      return QString("");
      }

//---------------------------------------------------------
//   enter_push
//---------------------------------------------------------

void EnterEdit::push()
      {
      QString input(text());
      if (input.isEmpty())
            return;
	enter_stack[enter_head++] = input;
      enter_head %= ENTER_SIZE;
      if (enter_n < ENTER_SIZE)
            ++enter_n;
      enter_max = enter_head;
      }

//---------------------------------------------------------
//   event
//    keyPressEvent does not deliver TAB and cannot
//    be used
//---------------------------------------------------------

bool EnterEdit::event(QEvent* event)
      {
      if (event->type() != QEvent::KeyPress)
            return QWidget::event(event);
      QKeyEvent* e = (QKeyEvent*)event;
      int key = e->key();
      int cmd = 0;

      bool ctrl = e->modifiers() & Qt::ControlModifier;
      if (ctrl) {
            switch(key) {
                  case Qt::Key_M:
                        cmd = CMD_MAN;
                        break;

                  case Qt::Key_V:
                        cmd = CMD_ENTER_KOLLAPS;
                        break;

                  case Qt::Key_F:
                        cmd = CMD_FUNCTION;
                        break;

                  case Qt::Key_G:
                        cmd = CMD_GOTO;
                        break;

                  case Qt::Key_K:
                        cmd = CMD_SHELL;
                        break;

                  case Qt::Key_S:
//                        cursorLeft(false, 1);
                        break;

                  case Qt::Key_D:
//                        cursorRight(false, 1);
                        break;

                  default:
                        QLineEdit::keyPressEvent(e);
                        break;
                  }
            }
      else {
            switch (key) {
                  case Qt::Key_Escape:
                        cmd = ESCAPE;
                        break;

                  case Qt::Key_Tab:
                        {
                        char* pp = tab_expand(text().toLatin1().data(), ped->getCurDir().toLatin1().data());
                        setText(pp);
                        delete pp;
                        }
                        return true;

//                  case Qt::Key_F3:
//                        cmd = CMD_NEW_ALTFIL;
//                        break;

                  case Qt::Key_F7:
                        cmd = CMD_SEARCH_F;
                        break;

                  case Qt::Key_Up:
                        {
                        QString s(enter_up());
                        if (!s.isEmpty())
                              setText(s);
                        }
                        break;
                  case Qt::Key_Down:
                        {
                        QString s(enter_down());
                        if (!s.isEmpty())
                              setText(s);
                        }
                        break;

                  case Qt::Key_F8:
                        cmd = CMD_STREAM;
                        break;

                  default:
                        QLineEdit::keyPressEvent(e);
                        break;
                  }
            }
      if (cmd) {
            push();
            ped->leaveEnterInput(cmd);
            return true;
            }
      return false;
      }

