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

#include "ped.h"
#include "line.h"
#include "text.h"
#include "cmd.h"

//---------------------------------------------------------
//    UndoList::start_new
//---------------------------------------------------------

void UndoList::start_new(const Position* p)
      {
      erase(cur_undo, end());
      cur_undo = insert(end(), new Undo(p));
      }

//---------------------------------------------------------
//    UndoList::end_new
//---------------------------------------------------------

void UndoList::end_new(const Position* p)
      {
      if ((*cur_undo)->type == UNDO_NOTHING) {
            erase(cur_undo);
            cur_undo = end();
            }
      else {
            *(*cur_undo)->end_pos = *p;
            ++cur_undo;
            }
      }

//---------------------------------------------------------
//    UndoList::append_line
//---------------------------------------------------------

void UndoList::append_line(const Line* l)
      {
      (*cur_undo)->l.push_back(new Line(*l));
      }

//---------------------------------------------------------
//   do_replace
//---------------------------------------------------------

void Kontext::do_replace(Line* line, const LinePos& p1,
   const LinePos& p2, const char* txt)
      {
      f->undo_start(&pos);
      f->undo_type(UNDO_CHANGE_LINE);
      f->replace(line, p1.cur_iline(), p2.cur_iline(), txt);
      f->undo_end(&pos);
      }

//---------------------------------------------------------
//    delete_char
//---------------------------------------------------------

void Kontext::delete_char()
      {
      f->undo_type(UNDO_CHANGE_LINE);
      f->delete_char(&*cursor, pos.spalte);
      register_update(UPDATE_LINE);
      }

//---------------------------------------------------------
//    rubout
//---------------------------------------------------------

void Kontext::rubout()
      {
      if (pos.spalte == 0) {
            if (pos.zeile == 0)
                  return;
            f->undo_type(UNDO_LINEUNBREAK);
            // append line to previous line
            iLineList l = cursor;
            ryposition(-1);
            rxposition(cursor->columns());
            f->append_line(cursor, l);
            f->delete_lines(l, 1);
            register_update(UPDATE_ALL);
            return;
            }

      f->undo_type(UNDO_CHANGE_LINE);
      rxposition(-1);
      f->delete_char(&*cursor, pos.spalte);
      register_update(UPDATE_LINE);
      }

//---------------------------------------------------------
//    newline
//---------------------------------------------------------

void Kontext::newline()
      {
      if (!insert_flag) {
            ryposition(1);
            xposition(0);
            return;
            }
      register_update(UPDATE_ALL);
      f->undo_type(UNDO_LINEBREAK);
      iLineList l = cursor;
      ++l;
      Line* cur_line = &*cursor;
      if (cur_line == 0) {
            printf("fatal 133\n");
            return;
            }
      Line* splitline = Line::splitLine(cur_line, pos.spalte);

      f->undo_line(splitline);

      int indent = 0;
      if (auto_indent) {
            int t = f->type();
            if (t == FILE_C || t == FILE_H || t == FILE_QML) {
                  //
                  // wenn letztes Zeichen von *cursor nicht
                  // '{' ist und die Zeile mit 'if', 'for' etc.
                  // beginnt, dann indent++
                  //
                  indent = c_compound();
                  if (indent < 0)
                        indent = 0;
                  iLine il(&*cursor, 0);
                  int n;
                  for (n = 0; !(*il).isNull(); ++il, ++n)
                        ;
                  QString p;
                  il.setcol(0);
                  for (;!il.eol(); ++il)
                        p.append(*il);
                  n = p.size();
                  if (n) {
                        while (--n) {
                              if (p[n] != ' ')
                                    break;
                              }
                        if (p[n] != '{') {
                              n = 0;
                              while (p[n] == ' ')
                                    ++n;
                              if ((p.mid(n, 2) == "if")
                                 || (p.mid(n, 3) == "for")
                                 || (p.mid(n, 2) == "do")
                                 || (p.mid(n, 4) == "else")
                                 || (p.mid(n, 5) == "while")) {
                                    if (pos.spalte > n)
                                          ++indent;
                                    }
                              }
                        }
                  indent *= tabl();
                  }
            else {
                  indent = cursor->getindent(INDENT_C);
                  if (indent >= pos.spalte)
            	      indent = pos.spalte;
                  }

            if (indent && splitline->size()) {
                  iLine il(splitline, 0);
                  while (*il == ' ') {
                        --indent;
                        ++il;
                        }
                  if (indent < 0)
                        indent = 0;
                  splitline->indent(indent);
                  }
            }
      f->insert_line(l, splitline);
      f->delete_restline(cursor, pos.spalte);
      ryposition(1);
      rxposition(indent - pos.spalte);
      }

/*---------------------------------------------------------
 *    del_restline
 *---------------------------------------------------------*/

void Kontext::del_restline()
      {
      f->undo_type(UNDO_CHANGE_LINE);
      f->undo_line(&*cursor);
      f->delete_restline(cursor, pos.spalte);
      register_update(UPDATE_LINE);
      }

/*---------------------------------------------------------
 *    del_word
 *---------------------------------------------------------*/

void Kontext::del_word()
      {
      f->undo_type(UNDO_CHANGE_LINE);
      f->undo_line(&*cursor);
      f->del_word(&*cursor, pos.spalte);
      register_update(UPDATE_LINE);
      }

//---------------------------------------------------------
//    Kontext::undo
//---------------------------------------------------------

void Kontext::undo()
      {
      Position* p = f->undo(this);
      if (p == 0)
            return;
      pos = *p;
      adjust_cursor();
      register_update(UPDATE_ALL);
      }

//---------------------------------------------------------
//    Kontext::redo
//---------------------------------------------------------

void Kontext::redo()
      {
      Position* p = f->redo(this);
      if (p == 0)
            return;
      pos = *p;
      adjust_cursor();
      register_update(UPDATE_ALL);
      }

void Kontext::cmd_put()
      {
      put(pick_buffer);
      register_update(UPDATE_ALL);
      }

//---------------------------------------------------------
//    Kontext::put
//    inserts a LineList at the cursor position
//---------------------------------------------------------

void Kontext::put(Text& stext)
      {
      bool coltype;
      LineList* sl = stext.read(coltype);
      iLineList line = cursor;

      if (coltype) {
      	f->undo_type(UNDO_CHANGE_LINE);
            for (iLineList src = sl->begin(); src != sl->end(); ++src) {
                  if (line == text->end())
                        break;
                  f->insert_column(&*line, &*src, pos.spalte);
                  ++line;
                  }
            }
      else {
      	f->undo_type(UNDO_DELETE_LINE, sl->size());
            for (iLineList src = sl->begin(); src != sl->end(); ++src)
                  f->insert_line(line, src);
            move_line(cursor, -(sl->size()));
            }
      register_update(UPDATE_ALL);
	}

//---------------------------------------------------------
//    Kontext::insert_tab
//---------------------------------------------------------

void Kontext::insert_tab()
      {
      f->undo_type(UNDO_CHANGE_LINE);
      f->undo_line(&*cursor);
      f->insert_char(&*cursor, pos.spalte, QChar(9));
	rxposition(((pos.spalte  + tabl()) / tabl()) * tabl() - pos.spalte);
//      printf("INSERT TAB\n");
      register_update(UPDATE_LINE);
      }

//---------------------------------------------------------
//    Kontext::insert_special
//---------------------------------------------------------

void Kontext::insert_special(int code)
      {
      printf("insert %d\n", code);
      }

/*---------------------------------------------------------
 *    Kontext::insert_char
 *---------------------------------------------------------*/

void Kontext::insert_char(const QChar& c)
      {
      if (cursor == text->end()) {
            text->push_back(new Line(""));
            cursor = text->end();
            --cursor;
            }
      f->undo_type(UNDO_CHANGE_LINE);
      register_update(UPDATE_LINE);
      f->undo_line(&*cursor);
      int xstep = 1;
      if (c == QLatin1Char(0x9)) {
            do {
                  f->insert_char(&*cursor, pos.spalte, ' ');
                  rxposition(1);
                  } while (pos.spalte % tabl());
            }
      else {
            f->insert_char(&*cursor, pos.spalte, c);
            rxposition(xstep);
            }
      }

/*---------------------------------------------------------
 *    Kontext::put_char
 *---------------------------------------------------------*/

void Kontext::put_char(const QChar& c)
      {
      if (cursor == text->end()) {
            text->push_back(new Line(""));
            cursor = text->end();
            --cursor;
            }
      f->undo_type(UNDO_CHANGE_LINE);
      register_update(UPDATE_LINE);
      f->undo_line(&*cursor);
      if (c == QLatin1Char(0x9)) {
	      rxposition(((pos.spalte  + tabl()) / tabl()) * tabl() - pos.spalte);
            return;
            }
      cursor->delete_char(pos.spalte);
      f->insert_char(&*cursor, pos.spalte, c);
      rxposition(1);
      }

//---------------------------------------------------------
//    Kontext::delete_line
//    delete current line
//---------------------------------------------------------

void Kontext::delete_line()
      {
      iLineList sl, el;
      int n = mark_interval(sl, el);

      if (n == -1)
            return;
      pos.yoffset -= n;
      if (pos.yoffset < 0)
            pos.yoffset = 0;
      pos.zeile -= n;

      pick_buffer.write(text, sl, el, mpos1.spalte, mpos2.spalte,
         mark_mode == MARK_COLUMNS);

      f->undo_spos(&pos);           // Startposition f. undo setzen
      if (mark_mode == MARK_COLUMNS) {
            f->undo_type(UNDO_CHANGE_LINE);
            f->delete_columns(sl, el, mpos1.spalte, mpos2.spalte);
            pos.spalte -= (mpos2.spalte-mpos1.spalte);
            if (pos.spalte < 0)
            	pos.spalte *= -1;
            }
      else {
            f->undo_type(UNDO_INSERT_LINE);
            f->delete_lines(sl, el);
            }
      adjust_cursor();
      register_update(UPDATE_ALL);
      }

//---------------------------------------------------------
//   updateDoc
//---------------------------------------------------------

void Kontext::updateDoc(void* p)
      {
      // nur aktuellen (d.h. sichtbaren) Kontext updaten
      if ((this != p)  && ((*(editor->kll)) == this)) {
            setOpenflag(false);
            open();
            register_update(UPDATE_ALL);
            update1();
            }
      }
