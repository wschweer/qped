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
#include "config.h"

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

      QFontMetrics fm(p->eefont);
      fw = fm.averageCharWidth(); // fm.widt('x');
      fh = fm.height();
      fa = fm.ascent();
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
      p.setRenderHint(QPainter::TextAntialiasing, true);

      int start   = y1 / fh;
      int end     = (y2 + fh - 1) / fh;
      iLineList l = k->topLine(start);

      int cr, cg, cb;
      bgColor.getRgb(&cr, &cg, &cb);
      cr -= cr/8;
      cb -= cb/8;
      cg -= cg/8;
      const QColor mc(cr, cg, cb);

      bool cFile    = k->getFile()->type() == FILE_C;
//      bool colorify = cFile && ped->getColorify();
      bool colorP   = (k->getMarkMode() == MARK_NONE) && cFile && ped->getParen();

      for (int i = start; i < end; ++i) {
            if (l == k->text->end()) {
                  p.eraseRect(0, i * fh + RAND, width(), fh);
                  continue;
                  }
            if (colorP) {
                  int sp, zl;
                  QChar cv  = k->curChar();
                  QChar cr  = k->leftChar();
                  if (cv == '(' || cv == '{') {
                        QChar ccc = k->colorBraceV(cv, sp, zl);
                        p.fillRect(sp * fw + RAND, zl * fh + RAND, fw, fh, Qt::red);
                        p.drawText(sp * fw + RAND, zl * fh + RAND + fa, QString(ccc));
                        }
                  if (cr == ')' || cr == '}') {
                        QChar ccc = k->colorBraceR(cr, sp, zl);
                        p.fillRect(sp * fw + RAND, zl * fh + RAND, fw, fh, Qt::red);
                        p.drawText(sp * fw + RAND, zl * fh + RAND + fa, QString(ccc));
                        }
                  }

            int sp1, sp2;
            if (k->isMarked(i, sp1, sp2)) {
                  int xp1 = RAND + sp1 * fw;
                  int xp2 = RAND + sp2 * fw + fw;
                  p.fillRect(xp1, i * fh + RAND, xp2-xp1, fh, mc);
                  }
            iLine il  = k->sl(&*l);
            int x2    = spalten();
            int y     = baseline(i);    // pixelposition
#if 0
            int sp;
            QChar* dst;
            QChar buffer[x2 + 1];
            if (colorify) {
                  sp = 0;
                  while (sp < x2 && *il != QChar::Null) {
                        dst = buffer;
                        // wortanfang suchen
                        int start = sp;
                        for (; sp < x2; ++sp) {
                              QChar c = *il;
                              if (c == QChar::Null || !is_delim(c))
                                    break;
                              *dst++ = c;
                              ++il;
                              }
                        if (dst != buffer) {
                              *dst = 0;
                              p.drawText(start * fw + RAND, y, buffer);
                              }
                        int n = is_keyword(il.txt());
                        if (n) {
                              p.setPen(Qt::blue);
                              QString s(il.txt());
                              p.drawText(sp * fw + RAND, y, s.left(n));
                              p.setPen(fgColor);
                              il += n;
                              sp += n;
                              }
                        else {
                              // wortende suchen
                              dst = buffer;
                              start = sp;
                              for (; sp < x2; ++sp) {
                                    char c = *il;
                                    if (c == 0 || is_delim(c))
                                          break;
                                    *dst++ = c;
                                    ++il;
                                    }
                              if (dst != buffer) {
                                    *dst = 0;
                                    p.drawText(start * fw + RAND, y, buffer);
                                    }
                              }
                        }
                  }
            else
#endif
                  {
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
                  }
            ++l;
            }

      editor->updateHScrollbar();
      editor->updateVScrollbar();

      if (!k->getMarkMode()) {
	      // draw cursor
	      int cx;
      	QString s(k->getCursorInfo(cx, cy));
	      cx = cx * fw + RAND;
      	cy = cy * fh + RAND;
	      QRect rr(cx, cy, fw, fh);
      	p.fillRect(rr, hasFocus() ? fgColor : QColor(160, 160, 160));
	      p.setPen(bgColor);
      	p.drawText(cx, cy + fa, s);
            }
      p.end();
      }

//---------------------------------------------------------
//   event
//---------------------------------------------------------

bool EditWin::event(QEvent* event)
      {
      if (event->type() != QEvent::KeyPress)
            return QWidget::event(event);

      QKeyEvent* e = (QKeyEvent*)event;

      QString s(e->text());
      Qt::KeyboardModifiers stat = e->modifiers();
      QChar c;

      if ((stat & (Qt::CTRL | Qt::ALT)) == 0)
            c = s[0];
#if 0
printf("key %x ext %d vkey %d\n", e->key(), e->hasExtendedInfo(), e->nativeVirtualKey());
      if (s.size()) {
printf("  text %x\n", c.unicode());
            }
#endif

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
#if defined(Q_WS_MAC)
      // Sonderbehandlung:
      //    zweite Alt-Taste aktivieren:
      if (stat & Qt::ALT) {
            switch (e->key()) {
                  case '8':  c = QLatin1Char('{');  break;
                  case '5':  c = QLatin1Char('[');  break;
                  case '6':  c = QLatin1Char(']');  break;
                  case '9':  c = QLatin1Char('}');  break;
                  case Qt::Key_Y:  c = QLatin1Char('\\'); break;
                  case Qt::Key_N:  c = QLatin1Char('~');  break;
                  case Qt::Key_7:  c = QLatin1Char('|');  break;
                  case Qt::Key_L:  c = QLatin1Char('@');  break;
                  }
            }
#else
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
#endif
      if (c.isPrint() || c == QLatin1Char(0x9)) {
	      (*(editor->kll))->editChar(c);
            editor->update();
            return true;
            }
      return false;
      }

//---------------------------------------------------------
//   wheelEvent
//---------------------------------------------------------

void EditWin::wheelEvent(QWheelEvent* we)
      {
      int amount = we->modifiers() & Qt::ShiftModifier ? 8 : 2;
      for (int i = 0; i < amount; ++i) {
            ped->edit_cmd(we->delta() > 0 ? CMD_SCROLL_DOWN : CMD_SCROLL_UP);
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
      return (height() - RAND2) / fontMetrics().height();
      }
int EditWin::spalten() const
      {
      return (width() - RAND2) / fontMetrics().width('x');
      }
int EditWin::yoffset() const
      {
      return (RAND + fontMetrics().ascent());
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
      *z = (y - yoffset() + fontMetrics().ascent()) / fontMetrics().height();
      *s = (x - xoffset()) / fontMetrics().width('x');
      }

//---------------------------------------------------------
//   baseline
//---------------------------------------------------------

int EditWin::baseline(int y) const
      {
      return y * fontMetrics().height() + RAND + fontMetrics().ascent();
      }


