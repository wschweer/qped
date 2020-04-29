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

#include "editwin.h"
#include "ped.h"
#include "cmd.h"
#include "text.h"

//---------------------------------------------------------
//   EditWin
//---------------------------------------------------------

EditWin::EditWin(QWidget* parent, Ped* p, Editor* e)
   : QFrame(parent)
      {
      editor = e;
      ped    = p;
      cy     = -1;

      QPalette pa(palette());
      pa.setColor(QPalette::Window, bgColor);
      setPalette(pa);
      setAutoFillBackground(true);
      setFocusPolicy(Qt::StrongFocus);
      }

//---------------------------------------------------------
//   paintEvent
//---------------------------------------------------------

void EditWin::paintEvent(QPaintEvent* e)
      {
      Kontext* k = (Kontext*)(*(editor->kll));

      QRect r(e->rect());

      int y1 = r.y() - RAND;
      int y2 = r.y() + r.height() - RAND;

      QPainter p(this);
      p.setFont(ped->eefont);
      p.setRenderHint(QPainter::TextAntialiasing, true);

      int start   = y1 / ped->fh;
      int end     = (y2 + ped->fh - 1) / ped->fh;
      iLineList l = k->topLine(start);

      int cr, cg, cb;
      bgColor.getRgb(&cr, &cg, &cb);
      cr -= cr/8;
      cb -= cb/8;
      cg -= cg/8;
      const QColor mc(cr, cg, cb);

      bool cFile    = k->getFile()->type() == FILE_C;
      bool colorP   = (k->getMarkMode() == MARK_NONE) && cFile && ped->getParen();

      for (int i = start; i < end; ++i) {
            if (l == k->text->end()) {
                  p.eraseRect(0, i * ped->fh + RAND, width(), ped->fh);
                  continue;
                  }
            if (colorP) {
                  int sp, zl;
                  QChar cv  = k->curChar();
                  QChar cr  = k->leftChar();
                  if (cv == '(' || cv == '{') {
                        QChar ccc = k->colorBraceV(cv, sp, zl);
                        p.fillRect(sp * ped->fw() + RAND, zl * ped->fh + RAND, ped->fw(), ped->fh, Qt::red);
                        p.drawText(sp * ped->fw() + RAND, zl * ped->fh + RAND + ped->fa, QString(ccc));
                        }
                  if (cr == ')' || cr == '}') {
                        QChar ccc = k->colorBraceR(cr, sp, zl);
                        p.fillRect(sp * ped->fw() + RAND, zl * ped->fh + RAND, ped->fw(), ped->fh, Qt::red);
                        p.drawText(sp * ped->fw() + RAND, zl * ped->fh + RAND + ped->fa, QString(ccc));
                        }
                  }

            int sp1, sp2;
            if (k->isMarked(i, sp1, sp2)) {
                  int xp1 = RAND + sp1 * ped->fw();
                  int xp2 = RAND + sp2 * ped->fw() + ped->fw();
                  p.fillRect(xp1, i * ped->fh + RAND, xp2-xp1, ped->fh, mc);
                  }
            iLine il  = k->sl(&*l);
            int x2    = spalten();
            int y     = baseline(i);    // pixelposition
            QString s;
            for (int sp = 0; sp < x2; ++sp) {
                  QChar c(*il++);
                  if (c.isHighSurrogate()) {
                        s.append(c);
                        c = *il++;
                        }
                  if (c == QChar::Null)
                        break;
                  s.append(c);
                  }
            p.drawText(RAND, y, s);
            ++l;
            }

      editor->updateHScrollbar();
      editor->updateVScrollbar();

      if (!k->getMarkMode()) {
	      // draw cursor
	      int cx;
      	QString s(k->getCursorInfo(cx, cy));
	      cx = cx * ped->fw() + RAND;
      	cy = cy * ped->fh + RAND;
	      QRect rr(cx, cy, ped->fw(), ped->fh);
      	p.fillRect(rr, hasFocus() ? fgColor : QColor(160, 160, 160));
	      p.setPen(bgColor);
      	p.drawText(cx, cy + ped->fa, s);
            }
      p.end();
      }

//---------------------------------------------------------
//   keyPressEvent
//---------------------------------------------------------

void EditWin::keyPressEvent(QKeyEvent* e)
      {
      QString s(e->text());
      Qt::KeyboardModifiers stat = e->modifiers();
      QChar c = 0;
//      printf("==key 0x%x %d <%s>\n", e->key(), e->key(), qPrintable(s));

      if (!s.isEmpty() && ((stat & (Qt::CTRL | Qt::ALT)) == 0))
            c = s[0];

      // handle dead keys:           ´
      switch (e->key()) {
            case Qt::Key_Dead_Acute:
                  ped->edit_cmd(CMD_INPUT_STRING, QString(QChar(Qt::Key_acute)));
                  return;
            case Qt::Key_Dead_Grave:
                  ped->edit_cmd(CMD_INPUT_STRING, QString(QChar(Qt::Key_QuoteLeft)));
                  return;
            default:
                  break;
            }
      //
      // Umlaute aktivieren
      //
      if (e->key() == -1) {
            switch(e->nativeVirtualKey()) {
                  case 214:
                  case 246:
                  case 252:
                  case 220:
                  case 223:
                  case 228:
                  case 196:
                        c = QLatin1Char(e->nativeVirtualKey());
                        break;
                  }
            }
      // Sonderbehandlung:
      //    zweite Alt-Taste aktivieren:
      if (stat & Qt::ALT) {
            switch (e->key()) {
                  case '7':  c = QLatin1Char('{');  break;
                  case '8':  c = QLatin1Char('[');  break;
                  case '9':  c = QLatin1Char(']');  break;
                  case '0':  c = QLatin1Char('}');  break;
                  case 0xdf: c = QLatin1Char('\\'); break;
                  case '+':  c = QLatin1Char('~');  break;
                  case '<':  c = QLatin1Char('|');  break;
                  case 'q':  c = QLatin1Char('@');  break;
                  }
            }
      if (c.isPrint() || c == QLatin1Char(0x9))
            ped->edit_cmd(CMD_INPUT_STRING, QString(c));
      }

//---------------------------------------------------------
//   wheelEvent
//---------------------------------------------------------

void EditWin::wheelEvent(QWheelEvent* we)
      {
      int amount = we->modifiers() & Qt::ShiftModifier ? 8 : 2;
      for (int i = 0; i < amount; ++i) {
            ped->edit_cmd(we->angleDelta().y() > 0 ? CMD_SCROLL_DOWN : CMD_SCROLL_UP);
            (*(editor->kll))->register_update(UPDATE_ALL);
            }
      }

//---------------------------------------------------------
//   mouseReleaseEvent
//---------------------------------------------------------

void EditWin::mouseReleaseEvent(QMouseEvent* e)
      {
      mouseButton = e->button();
      if (mouseButton == Qt::RightButton) {
            if (ped->aboutItem) {
                  delete ped->aboutItem;
                  ped->aboutItem = 0;
                  }
            }
      }

//---------------------------------------------------------
//   mousePressEvent
//---------------------------------------------------------

void EditWin::mousePressEvent(QMouseEvent* e)
      {
      mouseButton = e->button();
      int x = e->x();
      int y = e->y();
      if (mouseButton == Qt::MidButton) {
            QClipboard* cb = QApplication::clipboard();
            QString txt = cb->text(QClipboard::Clipboard);
            if (!txt.isEmpty()) {
                  // put selection
                  (*(editor->kll))->buttonpress(x, y, false, 0);
                  ped->edit_print(txt);
                  ped->update();
                  }
            }
      else {
            (*(editor->kll))->buttonpress(x, y, false,
               mouseButton == (Qt::LeftButton) ? 0 : 2);
            }
      }

//---------------------------------------------------------
//   mouseDoubleClickEvent
//---------------------------------------------------------

void EditWin::mouseDoubleClickEvent(QMouseEvent* e)
      {
      mouseButton = e->button();
      int x = e->x();
      int y = e->y();
      if (mouseButton == Qt::LeftButton)
            (*(editor->kll))->buttonpress(x, y, true, 0);
      }

//---------------------------------------------------------
//    mouseMoveEvent
//---------------------------------------------------------

void EditWin::mouseMoveEvent(QMouseEvent* e)
      {
      //
      //   liefert keinen Button-Status!
      //
      if (mouseButton == Qt::LeftButton) {
      	(*(editor->kll))->buttonmotion(e->x(), e->y());
            }
      }

void EditWin::focusInEvent(QFocusEvent*)
      {
      ped->cur_editor = editor;
      }

int EditWin::zeilen() const
      {
      return (height() - RAND2) / ped->fh;
      }
int EditWin::spalten() const
      {
      return (width() - RAND2) / ped->fw();
      }
int EditWin::yoffset() const
      {
      return (RAND + ped->fa);
      }

int EditWin::xoffset() const
      {
      return RAND;
      }

//---------------------------------------------------------
//   pos2xy
//---------------------------------------------------------

void EditWin::pos2xy(int x, int y, int* z, int* s) const
      {
      *z = (y - yoffset() + ped->fa) / ped->fh;
      *s = (x - xoffset()) / ped->fw();
      }

//---------------------------------------------------------
//   baseline
//---------------------------------------------------------

int EditWin::baseline(int y) const
      {
      return y * ped->fh + RAND + ped->fa;
      }

