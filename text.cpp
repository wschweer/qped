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

#include "line.h"
#include "text.h"
#include "ped.h"
#include "cmd.h"
#include "utils.h"
#include "editwin.h"
#include "gettag.h"

#define FONT_TEXT 0

Text pick_buffer;
Text delete_buffer;

typedef void (Kontext::*Fkt)(const QString& s);
#define F(x)  (Fkt)(&Kontext::x)

Kontext::TextEditFunction Kontext::fkt[] = {
      { CMD_SCROLL_UP,    F(cmd_scroll_up),   },
      { CMD_SCROLL_DOWN,  F(cmd_scroll_down), },
      { CMD_NEXT,         F(cmd_next),        },
      { CMD_BACK,         F(cmd_back),        },
      { CMD_UP,           F(cmd_up),          },
      { CMD_DOWN,         F(cmd_down),        },
      { CMD_START_FILE,   F(cmd_start_file),  },
      { CMD_END_FILE,     F(cmd_end_file),    },
      { CMD_PAGE_UP,      F(cmd_page_up),     },
      { CMD_PAGE_DOWN,    F(cmd_page_down),   },
      { CMD_START_LINE,   F(cmd_start_line),  },
      { CMD_END_LINE,     F(cmd_end_line),    },
      { CMD_START_WINDOW, F(start_window),    },
      { CMD_END_WINDOW,   F(end_window),      },
      { CMD_BACK_WORD,    F(back_word),       },
      { CMD_NEXT_WORD,    F(next_word),       },
      { CMD_PICK,         F(pick),            },
      { CMD_GOTO,         F(cmd_goto),        },
      { CMD_SEARCH_F,     F(searchF),         },
      { CMD_SEARCH_R,     F(searchR),         },
      { CMD_VIEW_TOGGLE,  F(view_toggle),     },
      { CMD_ENTER_KOLLAPS,  F(search_kollaps),     },
      { CMD_LINE_MARK,    F(line_mark),       },
      { CMD_COLUMN_MARK,  F(column_mark),     },
      { CMD_CKLAMMER,     F(c_klammer),       },
      { CMD_CKOMPOUND,    F(cmd_compound),    },
      { CMD_LINK,         F(cmd_link),        },
      { 0,    0,   },
      };

Kontext::MtextEditFunction Kontext::mfkt[] = {
      { CMD_DEL_RESTLINE, F(del_restline),   true},
      { CMD_DEL_WORD,     F(del_word),       true},
      { CMD_TAB,          F(insert_tab),     true},
      { CMD_DEL_CHAR,     F(delete_char),    true},
      { CMD_RUBOUT,       F(rubout),         true},
      { CMD_DEL_LINE,     F(delete_line),    true},
      { CMD_NEWLINE,      F(newline),        true},
      { CMD_PUT,          F(cmd_put),        true},
      { CMD_UNDO,         F(undo),           false},
      { CMD_REDO,         F(redo),           false},
      { CMD_SAVE,         F(save),           false},
      { CMD_INS_SPECIAL,  F(insert_special), false},
      { CMD_SEARCH_F,     F(searchF),        false},
      { CMD_SEARCH_R,     F(searchR),        false},
      { CMD_INSERTMODE,   F(insertmode),     false},
      };
#undef F

//---------------------------------------------------------
//   editChar
//---------------------------------------------------------

void Kontext::editChar(const QChar& c)
      {
      if (view_mode || f->readonly()) {
            return;
            }
      if (mark_mode != MARK_NONE) {        // Abort Mark-Mode
            pos = mpos1;
            adjust_cursor();
            register_update(UPDATE_ALL);
            mark_mode = MARK_NONE;
            return;
            }
      f->undo_start(&pos);
      if (insert_flag)
            insert_char(c);
      else
            put_char(c);
      f->undo_end(&pos);
      mark_mode = MARK_NONE;
      }

//---------------------------------------------------------
//	Kontext::edit
//---------------------------------------------------------

bool Kontext::edit(int cmd, const QString& param)
      {
      bool auto_indent_save = auto_indent;
//      auto_indent = true;         // why?

      for (const TextEditFunction* fk = fkt; fk->cmd; ++fk) {
            if (fk->cmd == cmd) {
                  (this->*fk->f)(param);
                  if (mark_mode) {
                        mpos2 = pos;
                        update_flags |= UPDATE_MARK;
                        }
                  auto_indent = auto_indent_save;
                  return true;
                  }
            }
      if (view_mode || f->readonly()) {
            auto_indent = auto_indent_save;
            return false;
            }
      const MtextEditFunction* mfptr = mfkt;
      while (mfptr < mfkt + (sizeof(mfkt)/sizeof(*mfkt))) {
            if (mfptr->cmd == cmd) {
                  //
                  // im berschreibmode verndert CMD_NEWLINE
                  // das File nicht
                  //
                  bool do_undo = mfptr->undo && !(cmd == CMD_NEWLINE && !insert_flag);
                  if (do_undo)
                        f->undo_start(&pos);
                  (this->*mfptr->f)(param);
                  if (do_undo)
                        f->undo_end(&pos);
                  mark_mode = MARK_NONE;
                  auto_indent = auto_indent_save;
                  return false;
                  }
            ++mfptr;
            }
#if 0
      if (cmd < 0x100) {
            if (mark_mode != MARK_NONE) {        // Abort Mark-Mode
                  pos = mpos1;
                  adjust_cursor();
                  register_update(UPDATE_ALL);
                  mark_mode = MARK_NONE;
                  auto_indent = auto_indent_save;
                  return true;
                  }
            f->undo_start(&pos);
            if (insert_flag)
                  insert_char(cmd);
            else
                  put_char(cmd);
            f->undo_end(&pos);
            }
#endif
      mark_mode = MARK_NONE;
      auto_indent = auto_indent_save;
      return false;
      }

//---------------------------------------------------------
//   body
//---------------------------------------------------------

LineList* Kontext::body()
      {
      return f->Text();
      }

/*---------------------------------------------------------
 *    xscrollinfo
 *---------------------------------------------------------*/

void Kontext::xscrollinfo(int& visible, int& max, int& p)
      {
      visible = spalten();
      max     = text->columns();
      p       = pos.xoffset;
      }

/*---------------------------------------------------------
 *    yscrollinfo
 *---------------------------------------------------------*/

void Kontext::yscrollinfo(int& visible, int& max, int& p)
      {
      visible = zeilen();                 // screen height in textlines
      max = text ? text->size() : 0;      // textlines in file
      p = pos.zeile - pos.yoffset;        // window position
      }

//---------------------------------------------------------
//    xscrollto
//    gibt true, wenn gescrolled wurde
//---------------------------------------------------------

bool Kontext::xscrollto(int new_pos)
      {
      if (new_pos != pos.xoffset) {
            pos.xoffset = new_pos;
            if (pos.spalte < pos.xoffset)
                  pos.spalte = pos.xoffset;
            if (pos.spalte >= pos.xoffset+spalten())
                  pos.spalte = spalten()-1+pos.xoffset;
            register_update(UPDATE_ALL);
            return true;
            }
      return false;
      }

//---------------------------------------------------------
//    is_marked
//    checks whether screenline y containes marked text
//    return screen start/end column of marked text
//---------------------------------------------------------

bool Kontext::isMarked(int y, int& x1, int& x2)
      {
      if (mark_mode == MARK_NONE)
            return false;
      int line = pos.zeile - pos.yoffset + y;
      int y1 = mpos1.zeile;
      int y2 = mpos2.zeile;
      if (y1 > y2) {
            int tmp = y2;
            y2 = y1;
            y1 = tmp;
            }
      x1 = mpos1.spalte;
      x2 = mpos2.spalte;
      if (x1 > x2) {
            int tmp = x1;
            x1 = x2;
            x2 = tmp;
            }
      bool rv = line >= y1 && line <= y2;
      if (mark_mode == MARK_LINES) {
            x1 = 0;
            x2 = spalten();
            }
      return rv;
      }

/*---------------------------------------------------------
 *    curChar
 *    returns current char, cursor points to
 *    return space if cursor points behind text
 *---------------------------------------------------------*/

QChar Kontext::curChar()
      {
      if (cursor == text->end())
            return QChar(' ');
      iLine p(&*cursor, pos.spalte);
      QChar c = *p;
      if (c.isNull())
            c = QChar(' ');
      return c;
      }

/*---------------------------------------------------------
 *    curChar
 *    returns current char at pos x/x
 *---------------------------------------------------------*/

QChar Kontext::curChar(int x, int y)
      {
      LineList::iterator i = text->line2iLine(y);
      if (i == text->end())
            return QChar(' ');
      iLine p(&*i, x);
      QChar c = *p;
      if (c.isNull())
            c = QChar(' ');
      return c;
      }

/*---------------------------------------------------------
 *    leftChar
 *    returns char left to the cursor
 *---------------------------------------------------------*/

QChar Kontext::leftChar()
      {
      if (cursor == text->end())
            return QChar(' ');
      iLine p(&*cursor, pos.spalte);
      --p;
      QChar c = *p;
      if (c.isNull())
            c = QChar(' ');
      return c;
      }

/*---------------------------------------------------------
 *    getCursorInfo
 *---------------------------------------------------------*/

QChar Kontext::getCursorInfo(int& x, int& y)
      {
      x = pos.spalte - pos.xoffset;
      y = pos.yoffset;
      return curChar();
      }

/*---------------------------------------------------------
 *    ryposition
 *    position cursor vertical
 *    TODO:  while Schleife durch Berechnung ersetzen
 *---------------------------------------------------------*/

void Kontext::ryposition(int n)
      {
      if (n < 0) {
            while (pos.zeile && n < 0) {
                  if ((pos.yoffset > KONTEXT_LINES)
                     || (pos.yoffset-pos.zeile == 0))
                        pos.yoffset--;
                  else {
                        register_update(UPDATE_ALL);
                        }
                  pos.zeile--;
                  move_line(cursor, -1);
                  ++n;
                  }
            }
      else {
            int ts = (signed) text->size() -1;
            while ((pos.zeile < ts) && n > 0) {
                  ++pos.yoffset;
                  if (pos.yoffset >= (zeilen() - KONTEXT_LINES)) {
                        pos.yoffset--;
                        register_update(UPDATE_ALL);
                        }
                  ++pos.zeile;
                  move_line(cursor, 1);
                  --n;
                  }
            }
      }

/*---------------------------------------------------------
 *    Kontext::rxposition
 *    position cursor horizontal
 *---------------------------------------------------------*/

bool Kontext::rxposition(int n)
      {
//      if (*cursor == 0)
//            return true;

      pos.spalte += n;
      if (pos.spalte < pos.xoffset)
            pos.xoffset = pos.spalte;
      if (pos.xoffset < 0)
            pos.xoffset = 0;
      if (n > 0) {
            if (pos.spalte-pos.xoffset >= (spalten() - 2)) {
                  pos.xoffset = pos.spalte - spalten() + 2;
                  register_update(UPDATE_ALL);
                  }
            return pos.spalte > cursor->columns();
            }
      else {
            if (pos.spalte < 0)
                  pos.spalte = 0;
            if (pos.xoffset && (pos.spalte - pos.xoffset <= 1)) {
                  pos.xoffset = pos.spalte - 1;
                  register_update(UPDATE_ALL);
                  }
            return pos.spalte == 0;
            }
      }

/*---------------------------------------------------------
 *    start_window
 *---------------------------------------------------------*/

void Kontext::start_window()
      {
      int n = KONTEXT_LINES - pos.yoffset;
      if (n < 0)
            ryposition(n);
      }

/*---------------------------------------------------------
 *    end_window
 *---------------------------------------------------------*/

void Kontext::end_window()
      {
      int n = zeilen() - pos.yoffset - KONTEXT_LINES - 1;
      if (n > 0)
            ryposition(n);
      }

/*---------------------------------------------------------
 *    yposition
 *    move cursor to absolute vertical position
 *    0  - top of file
 *    -1 - end of file
 *---------------------------------------------------------*/

void Kontext::yposition(int n)
      {
      int oldpos  = pos.zeile - pos.yoffset;
      if (n == 0) {
            cursor      = text->begin();
            pos.zeile   = 0;
            pos.yoffset = 0;
            if (pos.zeile - pos.yoffset != oldpos) {
                  register_update(UPDATE_ALL);
                  }
            }
      else if (n == -1) {
            pos.zeile  = text->size() - 1;
            cursor     = text->begin();
            move_line(cursor, pos.zeile);

            if (pos.zeile >= zeilen() - 1)
                  pos.yoffset = zeilen() - 1;
            else
                  pos.yoffset = pos.zeile;
            }
      if (pos.zeile - pos.yoffset != oldpos) {
            register_update(UPDATE_ALL);
            }
      }

/*---------------------------------------------------------
 *    xposition
 *---------------------------------------------------------*/

void Kontext::xposition(int x)
      {
      int oldpos = pos.xoffset;

      pos.xoffset = 0;
      if (x == 0)
            pos.spalte = 0;
      else if (x == -1) {
            pos.spalte = cursor->columns();
            int xmax = spalten() - 1 - KONTEXT_COLUMNS;
            if (pos.spalte > xmax) {
                  pos.xoffset = pos.spalte - xmax;
                  }
            }
      if (pos.xoffset != oldpos) {
            register_update(UPDATE_ALL);
            }
      }

/*---------------------------------------------------------
 *    back_word
 *---------------------------------------------------------*/

void Kontext::back_word()
      {
      iLine l(&*cursor, pos.spalte);
      --l;
      if (*l == ' ')
            while (l.col && *l == ' ')
                  --l;
      while (l.col && *l != ' ')
            --l;
      if (*l == ' ')
            ++l;
      pos.spalte = l.col;
      if (pos.xoffset && (pos.spalte - pos.xoffset <= 1)) {
            pos.xoffset = pos.spalte - 1;
            register_update(UPDATE_ALL);
            }
      }

/*---------------------------------------------------------
 *    next_word
 *---------------------------------------------------------*/

void Kontext::next_word()
      {
      iLine l(&*cursor, pos.spalte);
      if (*l != ' ') {
            while (!(*l).isNull() && *l != ' ')
                  ++l;
            }
      while (*l == ' ')
            ++l;
      if (*l== ' ')
            --l;
      pos.spalte = l.col;
      int xmax = spalten() - 1 - KONTEXT_COLUMNS;
      if (pos.spalte > xmax) {
            pos.xoffset = pos.spalte - xmax;
            register_update(UPDATE_ALL);
            }
      }

//---------------------------------------------------------
//    register_update
//---------------------------------------------------------

void Kontext::register_update(int flags)
      {
      update_flags |= flags;
      }

//---------------------------------------------------------
//    Text::read
//---------------------------------------------------------

LineList* Text::read(bool& coltype)
      {
      coltype = column_type;
      return &l;
      }

//---------------------------------------------------------
//    Text::write
//---------------------------------------------------------

void Text::write(LineList*, iLineList begin, iLineList end,
   int col1, int col2, bool column_mark)
      {
      // delete old contents
      l.clear();
      // copy
      if (col2 < col1) {
            int tmp = col1;
            col1 = col2;
            col2 = tmp;
            }
      for (iLineList i = begin; i != end; ++i) {
            if (column_mark)
                  l.push_back(i->columns(col1, col2));
            else
                  l.push_back(new Line(*i));
            }
      column_type = column_mark;
      }

//---------------------------------------------------------
//    Kontext::mark_interval
//---------------------------------------------------------

int Kontext::mark_interval(iLineList& sl, iLineList& el)
	{
      sl = cursor;
      el = cursor;
      int n = 0;
      if (mark_mode) {
            if (mpos1.zeile > mpos2.zeile) {
                  Position tmp = mpos1;
                  mpos1 = mpos2;
                  //mpos2 = mpos1;
                  mpos2 = tmp;
                  }
		el = text->line2iLine(mpos2.zeile);
            sl = text->line2iLine(mpos1.zeile);
            n = mpos2.zeile - mpos1.zeile;
            }
      if (!move_line(el, 1))
            return -1;
      return n;
      }

//---------------------------------------------------------
//    Kontext::pick
//    picks the current line(s) into the pick_buffer
//---------------------------------------------------------

void Kontext::pick()
      {
      iLineList sl;
      iLineList el;
      if (mark_interval(sl, el) == -1)
            return;
      pick_buffer.write(text, sl, el, mpos1.spalte, mpos2.spalte,
         mark_mode==MARK_COLUMNS);
      if (mark_mode) {
            mark_mode = MARK_NONE;
            pos = mpos1;
            adjust_cursor();
            register_update(UPDATE_ALL);
            }

      bool coltype;
      LineList* pl = pick_buffer.read(coltype);
      LinePos s(pl, pl->begin(), 0, 0);
      QString dst;

      for (;;) {
            QChar c = *s++;
            if (c.isNull())
                  break;
            dst.append(c);
            }
      if (!dst.isEmpty()) {
            QClipboard* cb = QApplication::clipboard();
            cb->setText(dst, QClipboard::Clipboard);
            }
      }

//---------------------------------------------------------
//    Kontext::adjust_cursor
//---------------------------------------------------------

void Kontext::adjust_cursor()
      {
      cursor = text->line2iLine(pos.zeile);
      }

//---------------------------------------------------------
//    Kontext::adjust_pos
//---------------------------------------------------------

void Kontext::adjust_pos(int zeile, int col)
      {
      ryposition(zeile - pos.zeile);
      pos.spalte = col;
      if (col > spalten()) {
            pos.xoffset = col - (spalten() - 2);
            register_update(UPDATE_ALL);
            }
      else
            pos.xoffset = 0;
      }

//---------------------------------------------------------
//    Kontext::cmd_goto
//---------------------------------------------------------

void Kontext::cmd_goto(const QString& param)
      {
      int n = param.toInt();
      if (param[0] != '+' && param[0] != '-')
            n -= (pos.zeile + 1);
      ryposition(n);
      }

//---------------------------------------------------------
//    Kontext::move_line
//---------------------------------------------------------

int Kontext::move_line(iLineList& l, int distance)
      {
      int n = 0;
      if (distance > 0) {
            while (distance--) {
                  if (l != text->end()) {
                        ++l;
                        ++n;
                        }
                  }
            }
      else  {
            n = 0;
            while (distance++ < 0) {
                  if (l != text->begin()) {
                        --l;
                        --n;
                        }
                  }
            }
      return n;
      }

//---------------------------------------------------------
//    Kontext::view_toggle
//---------------------------------------------------------

void Kontext::view_toggle()
      {
      if (view_mode == 0) {
            kollaps_list.clear();
            switch (f->type()) {
                  case FILE_C:
                  case FILE_H:
                  case FILE_QML:
                        pos.zeile = generate_cview(&kollaps_list, text,
                           pos.zeile);
                        break;
                  case FILE_TEXT:
                  case FILE_UNKNOWN:
                  default:
                        return;
                  }
            if (kollaps_list.size()) {
                  text = &kollaps_list;
                  view_mode = 1;
                  }
            }
      else {
            view_mode = 0;
            pos.zeile = cursor->tag();
            text = f->Text();
            }
      //
      //    pos.yoffset korrigieren
      //
      if (pos.yoffset >= zeilen())
            pos.yoffset = zeilen()-2;
      adjust_cursor();
      if (pos.yoffset < KONTEXT_LINES)
            pos.yoffset = KONTEXT_LINES;
      if (pos.zeile < pos.yoffset)
            pos.yoffset = pos.zeile;
      pos.spalte = 0;
      pos.xoffset = 0;
      register_update(UPDATE_ALL);
      }

//---------------------------------------------------------
//    Kontext::line_mark
//---------------------------------------------------------

void Kontext::line_mark()
      {
      if (mark_mode == MARK_LINES) {
            mark_mode = MARK_NONE;
            }
      else {
            ompos1 = mpos1 = pos;
            ompos2 = mpos2 = pos;
            mark_mode = MARK_LINES;
            }
      register_update(UPDATE_ALL);
      }

//---------------------------------------------------------
//    Kontext::column_mark
//---------------------------------------------------------

void Kontext::column_mark()
      {
      if (mark_mode == MARK_COLUMNS)
            mark_mode = MARK_NONE;
      else {
            ompos1 = mpos1 = pos;
            ompos2 = mpos2 = pos;
            mark_mode  = MARK_COLUMNS;
            }
      register_update(UPDATE_ALL);
      }

//---------------------------------------------------------
//    Kontext::buttonpress
//---------------------------------------------------------

void Kontext::buttonpress(int x, int y, bool doubleclick, int button)
      {
      if (mark_mode) {
            mark_mode = MARK_NONE;
            mpos2 = pos;
            register_update(UPDATE_ALL);
            }
      int zl, sp;
      editor->win->pos2xy(x, y, &zl, &sp);
      if (doubleclick) {
            if (view_mode == 1) {
      	      view_toggle();
                  update();
                  return;
                  }
            cmd_link();       // cursor warp
            (*(editor->kll))->buttonpress(x, y, false, button);
            return;
            }
      ryposition(zl - pos.yoffset);
      rxposition(sp + pos.xoffset - pos.spalte);
      ped->set_line_column_var();
      update();
      ompos1 = mpos1 = pos;
      ompos2 = mpos2 = pos;
      if (button == 2)
            about();
      }

//---------------------------------------------------------
//    Kontext::buttonmotion
//---------------------------------------------------------

void Kontext::buttonmotion(int x, int y)
      {
      int zl, sp;
      editor->win->pos2xy(x, y, &zl, &sp);

      ryposition(zl - pos.yoffset);
      rxposition(sp + pos.xoffset - pos.spalte);
      if (mpos2 != pos) {
            if (mark_mode == MARK_NONE) {
                  if (mpos1.zeile != pos.zeile)
                        mark_mode = MARK_LINES;
                  if (mpos1.spalte != pos.spalte)
                        mark_mode = MARK_COLUMNS;
                  register_update(UPDATE_MARK);
                  update();
                  }
            ped->set_line_column_var();
            mpos2 = pos;
            register_update(UPDATE_MARK);
            update();
            }
      }

/*---------------------------------------------------------
 *    Kontext::open
 *---------------------------------------------------------*/

void Kontext::open()
      {
      if (openflag)
            return;
      f->open();
      text = f->Text();
      if (pos.zeile > text->size())       // falls der Text krzer geworden ist
            pos.zeile = text->size()-1;
      adjust_cursor();
      openflag = true;
      }

//---------------------------------------------------------
//    Kontext::c_klammer
//---------------------------------------------------------

void Kontext::c_klammer() const
      {
      int level = 0;
      iLine p(&*cursor, 0);

      while (!(*p).isNull()) {
            if (*p == '(')
                  ++level;
            else if (*p == ')')
                  --level;
            ++p;
            }
      ped->msg(2000, QString("() Balance %1").arg(level));
      }

//---------------------------------------------------------
//    operator==
//---------------------------------------------------------

bool operator==(const iLine& ln, const char* txt)
      {
      iLine p = ln;
      for (;;) {
            QChar c1 = *p++;
            int c2 = *txt++;
            if (c2 == 0)
                  return true;
            if (c1.toLatin1() != c2)
                  return false;
            }
      }

//---------------------------------------------------------
//	is_wordchar
//---------------------------------------------------------

static bool is_wordchar(QChar c, int type)
      {
      if (type == 1)
            return c.isLetterOrNumber() || c == '_' || c == '/' || c == '.';
      return c.isLetterOrNumber() || c == '_';
      }

//---------------------------------------------------------
//	Kontext::get_word
//---------------------------------------------------------

QString Kontext::get_word(int type) const
      {
      iLine p(&*cursor, pos.spalte);
      if ((*p).isNull())
            return strdup("");
      while (p.col && is_wordchar(*p, type))
            --p;
      if (!is_wordchar(*p, type))
            ++p;

      QString dst;

      for (int i = 0; i < 511; i++) {
            if (!is_wordchar(*p, type))
                  break;
            dst.append(*p++);
            }
      return dst;
      }

//---------------------------------------------------------
//   searchInclude
//    sucht includefile "name" anhand einer
//    (z.Z.) festverdrahteten Pfadliste
//---------------------------------------------------------

static Link* searchInclude(const QString& name)
      {
      static const char* pathlist[] = {
            "/usr/include",
            "/usr/X11R6/include",
            "/usr/qt2/include",
            "/usr/qt/include",
            0
            };
      for (const char** p = pathlist; *p; ++p) {
            QString s = *p;
            s += "/" + name;
            QFileInfo fi(s);
            if (fi.exists())
                  return new Link(s, 0);
            }
      return 0;
      }

//---------------------------------------------------------
//   is_include
//---------------------------------------------------------

#undef strncmp

static bool is_include(iLine& p)
      {
      if (*p != '#')
            return false;
      ++p;
      while (*p == ' ' || *p == '\t')
            ++p;
      const char* src = "include";
      while (*src) {
            if (*src++ != *p)
                  return false;
            ++p;
            }
      return true;
      }

//---------------------------------------------------------
//    Kontext::getLink
//---------------------------------------------------------

Link* Kontext::getLink() const
      {
      int ft = f->type();
      if (ft != FILE_C && ft != FILE_H)
            return 0;

      QString word;
      iLine p(&*cursor, 0);
      if (is_include(p)) {
            word = get_word(1);
            while (!(*p).isNull()) {
                  if (*p == '<' || *p == '"')
                        break;
                  ++p;
                  }
            if (*p == '<')
                  return searchInclude(word);
            else
                  word = f->path() + "/" + word;
            return new Link(word, 0);
            }
      else {
#if 0
            QString word = get_word(0);
            Link* t = gettag(word);
            if (t)
                  return t;
            LineList* l = search_man(p);
            if (!l->empty()) {
                  for (iLineList i = l->begin(); i != l->end(); ++i) {
                        char buffer[1024];
	                  char* manpage = i->cstring();
                        snprintf(buffer, 1024, "man:%s", manpage);
                        t = new Link(QString(buffer), 0);
                        break;
                        }
                  delete l;
                  return t;
                  }
#endif
            }
      return 0;
      }

//---------------------------------------------------------
//   topLine
//    return iterator to screen line "line"
//---------------------------------------------------------

iLineList Kontext::topLine(int line)
      {
      iLineList l = cursor;
      move_line(l, line - pos.yoffset);
      return l;
      }

//---------------------------------------------------------
//    yscrollto
//---------------------------------------------------------

int Kontext::yscrollto(int npos)
      {
      return yscroll(npos - (pos.zeile - pos.yoffset));
      }

//---------------------------------------------------------
//    Kontext::scroll
//    scrollt Anzeige um 'n' Zeilen hoch bzw. runter
//    liefert die Zahl der tatschlich gescrollten Zeilen
//---------------------------------------------------------

#define screen_pos() (pos.zeile - pos.yoffset)

int Kontext::yscroll(int n)
      {
      int new_pos = screen_pos() + n;
      int topline = text->size();

      if (new_pos > (topline - 1))
            new_pos = topline - 1;
      if (new_pos < 0)
            new_pos = 0;

      n = new_pos - screen_pos();
      if (n == 0)
            return 0;

      pos.yoffset -= n;
      n = 0;
      if (pos.yoffset < 0) {
            n = -pos.yoffset;
            pos.yoffset = 0;
            }
      if (pos.yoffset >= zeilen()) {
            n = zeilen() - pos.yoffset - 1;
            pos.yoffset = zeilen() - 1;
            }
      pos.zeile += n;
      move_line(cursor, n);
      register_update(UPDATE_ALL);
      return n;
      }

//---------------------------------------------------------
//    page
//    move cursor one page up or down; try to preserve
//    cursor offset in window (pos.yoffset)
//---------------------------------------------------------

void Kontext::page(int direction)
      {
//      int n = (zeilen() - PAGE_OVERLAP + 1) * direction;
      int ozeile = pos.zeile;

      pos.zeile -= (zeilen() - PAGE_OVERLAP + 1) * direction;
      if (pos.zeile < 0)
            pos.zeile = 0;

      if (pos.zeile >= (text->size()-1))
            pos.zeile = text->size() - 1;
      advance(cursor, pos.zeile - ozeile);
      //
      //   adjust pos.yoffset
      //
      if (pos.yoffset < KONTEXT_LINES)
            pos.yoffset = KONTEXT_LINES;
      if (pos.zeile < pos.yoffset)
            pos.yoffset = pos.zeile;

      if (pos.zeile != ozeile) {
            register_update(UPDATE_ALL);
            }
      }

//---------------------------------------------------------
//   colorBrace
//    sucht nach einer korrespondierenden Klammer
//---------------------------------------------------------

QChar Kontext::colorBraceV(QChar sbrace, int& px, int& py)
      {
      int level = 0;
      QChar ebrace = sbrace == '(' ? ')' : '}';

      LinePos p(text, cursor, pos.zeile+pos.yoffset, pos.spalte);
      QChar c;
      int mz = pos.zeile + zeilen();

      for (;;) {
            QChar c = *p++;
            if (c.isNull())
                  break;
            if (c == '\'') {        // skip char constants
                  while (!(c = *p++).isNull()) {
                        if (c == '\\') {
                              ++p;
                              continue;
                              }
                        if (c == '\'')
                              break;
                        if (c == '\n') {
                              if (p.line() >= mz)      // sichtbar?
                                    return 0;
                              }
                        }
                  }
            else if (c == '"') {         // skip strings
                  while (!(c = *p++).isNull()) {
                        if (c == ')') {
                              ++p;
                              continue;
                              }
                        if (c == '"')
                              break;
                        if (c == '\n') {
                              if (p.line() >= mz)      // sichtbar?
                                    return 0;
                              }
                        }
                  }
            else if (c == '/') {         // skip comments
                  if (*p == '/') {
                        ++p;
                        while (!(c = *p++).isNull()) {
                              if (c == '\n') {
                                    if (p.line() >= mz)      // sichtbar?
                                          return 0;
                                    break;
                                    }
                              }
                        break;
                        }
                  else if (*p == '*') {
                        while (!(c = *p++).isNull()) {
                              if (c == '*' && *p == '/') {
                                    ++p;
                                    break;
                                    }
                              if (c == '\n') {
                                    if (p.line() >= mz)      // sichtbar?
                                          return 0;
                                    }
                              }
                        }
                  }
            else if (c == '{' || c == '(') {
                  if (c == sbrace)
                        ++level;
                  }
            else if (c == '}' || c == ')') {
                  if (c == ebrace)
                        --level;
                  if (level == 0) {
                        --p;
                        int y = p.line()-pos.zeile;
                        paren_line = y;
                        int x = p.col() - pos.xoffset;
                        if (x < 0 || x >= spalten()
                           || y < 0 || y >= zeilen()) {
                              return 0;
                              }
                        py = y;
                        px = x;
                        return c;
                        }
                  }
            else if (c == '\n') {
                  if (p.line() >= mz)      // sichtbar?
                        return 0;
                  }
            }
      return 0;
      }

//---------------------------------------------------------
//   colorBraceR
//---------------------------------------------------------

QChar Kontext::colorBraceR(QChar ebrace, int& px, int& py)
      {
return 0;

      QChar c;
      int level = 0;
      char sbrace = ebrace == ')' ? '(' : '{';

      LinePos p(text, cursor, pos.zeile+pos.yoffset, pos.spalte);
      --p;
      while (!p.bot()) {
            c = *p;
            --p;
            switch (c.toLatin1()) {
                  case '\'':        // skip char constants
                        while (!p.bot()) {
                              c = *p;
                              --p;
                              if (*p == '\\') {
                                    --p;
                                    continue;
                                    }
                              if (c == '\'')
                                    break;
                              if (c == '\n') {
                                    if (p.line() < pos.zeile)
                                          return 0;
                                    }
                              }
                        break;
                  case '"':         // skip strings
                        while (!p.bot()) {
                              c = *p;
                              --p;
                              if (*p == '\\') {
                                    --p;
                                    continue;
                                    }
                              if (c == '"')
                                    break;
                              if (c == '\n') {
                                    if (p.line() < pos.zeile)
                                          return 0;
                                    }
                              }
                        break;
                  case '/':         // skip comments
//  funktioniert rckwrts so nicht:
#if 0
                        if (*p == '/') {
                              ++p;
                              while (!p.bot()) {
                                    c = *p;
                                    --p;
                                    if (c == '\n') {
                                          if (p.line() < pos.zeile)
                                                return 0;
                                          break;
                                          }
                                    }
                              break;
                              }
#endif
                        if (*p != '*')
                              break;
                        while (!p.bot()) {
                              c = *p;
                              --p;
                              if (c == '*' && *p == '/') {
                                    ++p;
                                    break;
                                    }
                              if (c == '\n') {
                                    if (p.line() < pos.zeile)
                                          return 0;
                                    }
                              }
                        break;
                  case '{':
                  case '(':
                        if (c != sbrace)
                              break;
                        ++level;
                        if (level == 0) {
                              ++p;
                              int y = p.line()-pos.zeile;
                              paren_line = y;
                              int x = p.col() - pos.xoffset;
                              if (x < 0 || x >= spalten()
                                 || y < 0 || y >= zeilen())
                                    return 0;
                              px = x;
                              py = y;
                              return c;
                              }
                        break;
                  case '}':
                  case ')':
                        if (c == ebrace)
                              --level;
                        break;
                  case '\n':
                        if (p.line() < pos.zeile)
                              return 0;
                        {
                        //
                        // vor einen evtl vorhandenen
                        // '//'-Kommentar positionieren
                        //
                        iLine il(p.cur_line());
                        while (!il.eol()) {
                              QChar c = *il;
                              ++il;
                              if (c == '/' && *il == '/') {
                                    --il;
                                    --il;
                                    p.setIline(il);
                                    break;
                                    }
                              }
                        }
                        break;
                  default:
                        break;
                  }
            }
      return 0;
      }

//---------------------------------------------------------
//    Kontext::update
//    interne update-Routine, wird nicht vom Window-System
//    (via exposure) aufgerufen
//---------------------------------------------------------

void Kontext::update()
      {
      update1();
      f->updateFile(this);    // wenn sich file gendert hat, dann evtl.
                              // andere views updaten
      }

//---------------------------------------------------------
//   update1
//---------------------------------------------------------

void Kontext::update1()
      {
      update_flags |= UPDATE_LINE;

      if (update_flags & UPDATE_MARK) {
            // put_mwindow(p, update_flags & UPDATE_ALL);
            editor->win->update();
            paren_line = -1;
            }
      else if (update_flags & UPDATE_ALL) {
            editor->win->update();
            paren_line = -1;
            }
      else if (update_flags & UPDATE_LINE) {
            int zl = pos.yoffset;
            int fh = editor->win->fontMetrics().height();
            int cy = editor->win->cy;

            int y1 = fh * zl;
            int y2 = y1;
            if (cy != -1) {
                  if (cy < y1)
                        y1 = cy;
                  if (cy > y2)
                        y2 = cy;
                  }
            y2 += fh + editor->win->fontMetrics().descent();
            editor->win->update(QRect(0, y1, editor->win->width(), y2-y1));
            if ((paren_line >= 0) && (paren_line == pos.yoffset))
                  paren_line = -1;
            }
      if (paren_line >= 0) {
            int zl = paren_line;
            int fh = editor->win->fontMetrics().height();
            int y1 = fh * zl;
            int y2 = y1 + fh;
            editor->win->update(QRect(0, y1, editor->win->width(), y2-y1));
            }
      update_flags = 0;
      }

//---------------------------------------------------------
//   Kontext::cmd_link
//---------------------------------------------------------

void Kontext::cmd_link()
      {
      Link* link = getLink();
      if (link == 0) {
            ped->msg(1000, "no link found");
            return;
            }
      QString s(link->url());
      int zeile = link->line();
      int rv = ped->file(QString("%1#%2").arg(s).arg(zeile), true);
      if (rv == 0) {
            ped->open_kontext();
            ped->update();
            }
      delete link;
      }

//---------------------------------------------------------
//   show
//---------------------------------------------------------

void Kontext::showKontext(bool)
      {
      editor->raiseStack(0);      // editorwindow zeigen
      editor->win->setFocus();
      }

//---------------------------------------------------------
//   about
//---------------------------------------------------------

void Kontext::about()
      {
      if (ped->aboutItem) {
            printf("HALLO!\n");
            return;
            }
      iLine wp(&*cursor, pos.spalte);
      //
      //  Namen der aktuellen Prozedur suchen:
      //
      LinePos pp(text, text->begin(), 0, 0);
//      char* procName = findProc(pp, pos.zeile);
      //
      //  mit wp suchen wir den Anfang eines 'C'-tags, der nach
      //    word kopiert wird
      //  mit bp suchen wir ein vorangehendes '.' oder '->',
      //    um eine Baseklasse zu bestimmen, die nach base
      //    kopiert wird (wir ignorieren hier zunchst tiefer geschachtelte
      //    Konstrukte
      //
#if 0 //TODO
      if ((*wp).isNull())
            return;
      if (!is_wordchar(*wp, 0))
            return;
      while (wp.col && is_wordchar(*wp, 0))
            --wp;
      if (!is_wordchar(*wp, 0))  // zu weit zurckgelaufen?
            ++wp;
      iLine bp(wp);     // merken

      char word[512];
      char base[512];
      char* dst = word;
      for (int i = 0; i < 511; i++) {
            if (!is_wordchar(*wp, 0))
                  break;
            *dst++ = *wp++;
            }
      *dst = 0;
      while (isspace(*wp))
            --wp;
//      bool proc = *wp == '(';

      //---------------------------------------------------
      base[0] = 0;
      dst = base;
      --bp;
      char basec = *bp;
      --bp;
      if (basec == '.' || (basec == '>' && *bp == '-')) {
            if (*bp == '-')
                  --bp;
            while (isspace(*bp))
                  --bp;
            while (bp.col && is_wordchar(*bp, 0))
                  --bp;
            if (!is_wordchar(*bp, 0))  // zu weit zurckgelaufen?
                  ++bp;
            for (int i = 0; i < 511; i++) {
                  if (!is_wordchar(*bp, 0))
                        break;
                  *dst++ = *bp++;
                  }
            *dst = 0;
            }
      //-----------------------------------------------------------
      QString buffer;
      if (is_keyword(word)) {
            buffer = word + ": 'C'-Keyword";
            }
      else {
            if (procName) {
                  buffer = procName + "(): ";
                  int nn = strlen(buffer);
                  p += nn;
                  n -= nn;
                  }
            if (*base) {
                  snprintf(p, n, "%s%s%s%s",
                     base,
                     basec == '.' ? "." : "->",
                     word, proc ? "()" : "");
                  }
            else
                  snprintf(p, n, "%s%s", word, proc ? "()" : "");
            }
      int fw = editor->win->fontMetrics().averageCharWidth();

      int x = editor->win->xoffset() + fw * (pos.spalte-pos.xoffset);
      int y = editor->win->baseline(pos.yoffset);
      QPoint r = editor->win->mapToGlobal(QPoint(x, y));
      int w = editor->win->fontMetrics().averageCharWidth() * strlen(buffer) + 5;

      ped->aboutItem = new QFrame(editor->win);
      QLabel* l = new QLabel(buffer, ped->aboutItem);
      l->setFixedWidth(w);
      ped->aboutItem->setGeometry(r.x(), r.y(), w, l->height());
      ped->aboutItem->show();
#endif
      }

//---------------------------------------------------------
//   getpos
//---------------------------------------------------------

void Kontext::getpos(int& x, int& y, int& xoff, int& yoff) const
      {
      y    = pos.zeile;
      x    = pos.spalte;
      xoff = pos.xoffset;
      yoff = pos.yoffset;
      }

