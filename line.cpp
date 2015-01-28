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
#include "utils.h"
#include "ped.h"

//---------------------------------------------------------
//    Line::Line
//---------------------------------------------------------

Line::Line(const QString& t, int l)
      {
      txt = t.left(l);
      }

Line::Line(const QString& s)
   : txt(s)
      {
      }

//---------------------------------------------------------
//   columns
//---------------------------------------------------------

int Line::columns() const
      {
      int col = 0;
      iLine  i(const_cast<Line*>(this), 0);
      while (!i.eol()) {
            ++col;
            ++i;
            }
      return col;
      }

//---------------------------------------------------------
//   idx2column
//---------------------------------------------------------

int Line::idx2column(int idx) const
      {
      if (idx >= txt.size())
            idx = txt.size();
      if (idx < 0)
            return 0;
      int col = 0;
      for (int i = 0; i < idx; ++i) {
            ++col;
            if (txt[i] == QChar(QLatin1Char('\t')))
                  col = (col + TABL - 1) / TABL * TABL;
            }
      return col;
      }

//---------------------------------------------------------
//   column2idx
//---------------------------------------------------------

int Line::column2idx(int col) const
      {
      int idx = 0;
      for (int i = 0; (i < col) && (idx < txt.size()); ++i) {
            if (!(txt[idx] == QChar(QLatin1Char('\t')) && (i % TABL)))
                  ++idx;
            }
      return idx;
      }

//---------------------------------------------------------
//    append
//---------------------------------------------------------

void Line::append(Line& l)
      {
      txt += l.txt;
      }

//---------------------------------------------------------
//   indent
//---------------------------------------------------------


void Line::indent(int columns)
      {
      txt = QString(columns, QChar(' ')) + txt;
      }

//---------------------------------------------------------
//   swap
//---------------------------------------------------------

void Line::swap(Line& l)
      {
      QString t = l.txt;
      l.txt = txt;
      txt = t;
      }

//---------------------------------------------------------
//    Line::delete_word
//---------------------------------------------------------

void Line::delete_word(int col)
      {
      iLine s(this, col);

      if (*s == ' ') {
            while (*s == ' ')
                  s.delete_char();
            }
      else {
            while (!(*s).isNull() && *s != ' ')
                  s.delete_char();
            while (*s == ' ')
                  s.delete_char();
            }
      }

//---------------------------------------------------------
//   cstring
//    gibt mit 0 terminierten String zurück
//---------------------------------------------------------

char* Line::cstring() const
      {
      char* p = new char[size()+1];
      memcpy(p, txt.toLatin1().data(), size());
      p[size()] = 0;
      return p;
      }

//---------------------------------------------------------
//    Line::del
//---------------------------------------------------------

void Line::del(int scol, int ecol)
      {
      iLine s(this, scol);
      int n = ecol - scol;
      while (n-- >= 0)
            s.delete_char();
      }

//---------------------------------------------------------
//    Line::getindent
//---------------------------------------------------------

int Line::getindent(int)
      {
      iLine p(this, 0);
      while (*p == ' ')
            ++p;
      int col = p.col;
#if 0
      if (type == INDENT_C) {
            if (l > 2 && strncmp(p, "if", 2)==0)
                  col += TABL;
            else if (l >= 3 && strncmp(p, "for", 3) == 0)
                  col += TABL;
            else if (l >= 4 && strncmp(p, "else", 4) == 0)
                  col += TABL;
            else if (l >= 5 && strncmp(p, "while", 5) == 0)
                  col += TABL;
            else if (l >= 2 && strncmp(p, "do", 2) == 0)
                  col += TABL;
            else if (l && *p == '}')
                  col -= TABL;
            if (col < 0)
                  col = 0;
            }
#endif
      return col;
      }

//---------------------------------------------------------
//   splitLine
//---------------------------------------------------------

Line* Line::splitLine(const Line* l, int col)
      {
      iLine i(const_cast<Line*>(l), col);
      QString dst;
      while (!i.eol())
            dst.append(*i++);
      Line* line = new Line(dst);
      return line;
      }

//---------------------------------------------------------
//    Line::columns
//    kopiert eine Spalte der Zeile
//---------------------------------------------------------

Line* Line::columns(int col1, int col2) const
	{
      int n = col2 - col1 + 1;
      iLine src(const_cast<Line*>(this), col1);
      QString dst;
      for (int i = 0; i < n; i++)
            dst.append(*src++);
      Line* line = new Line(dst);
    	return line;
      }

//---------------------------------------------------------
//    Line::change
//---------------------------------------------------------

void Line::change(const QString& p)
      {
      txt = p;
      }

//---------------------------------------------------------
//    Line::delete_char
//---------------------------------------------------------

void Line::delete_char(int xpos)
      {
      int n   = size();
      int src = 0;
      QString dst;

      for (int col = 0; n; n--) {
            if (txt[src] == '\t') {
                  int ncol = ((col + TABL) / TABL) * TABL;
                  if (ncol > xpos) {
                        ncol--;
                        while (col < ncol) {
                              dst.append(' ');
                              ++col;
                              }
                        src++;
                        dst += txt.mid(src);
                        break;
                        }
                  else {
                        col = ncol;
                        dst.append(txt[src++]);
                        }
                  }
            else {
                  if (col == xpos) {
                        ++src;
                        dst += txt.mid(src);
                        break;
                        }
                  else
                        dst.append(txt[src++]);
                  ++col;
                  }
            }
      change(dst);
      }

//---------------------------------------------------------
//   insert_char
//---------------------------------------------------------

void Line::insert_char(int col, QChar c)
      {
      QString dst;
      int src = 0;
      int i;

      int n = size();
      for (i = 0; n && i < col; n--) {
            if (txt[src] == '\t') {
                  int ncol = ((i + TABL) / TABL) * TABL;
                  if (ncol > col) {
                        if (c == '\t') { // ignore tab in tab
                              return;
                              }
                        //
                        //  destroy tab
                        //
                        // tab vorlauf
                        while (i < col) {
                              dst.append(' ');
                              ++i;
                              }
                        dst.append(c);
                        ++i;
                        //
                        // tab nachlauf
                        //
                        ++src;      // skip tab
                        if (size() <= 0)
                              break;
                        while (i < (ncol+1)) {
                              dst.append(' ');
                              ++i;
                              }
                        dst += txt.mid(src);
                        break;
                        }
                  else {
                        i = ncol;
                        dst.append(txt[src++]);
                        }
                  }
            else {
                  dst.append(txt[src++]);
                  ++i;
                  }
            }
      if (i <= col) {
            while (i < col) {       // evtl. mit Leerzeichen füllen
                  dst.append(' ');
                  ++i;
                  }
            dst.append(c);
            dst += txt.mid(src);   // evtl rest kopieren
            }
      change(dst);
      }

//---------------------------------------------------------
//    delete_rest
//---------------------------------------------------------

void Line::delete_rest(int col)
      {
      int idx = 0;
      for (int i = 0; i < col; i++) {
            if (!(idx >= size()) && !(txt[idx] == '\t' && ((i+1) % TABL)))
                  ++idx;
            }
      txt = txt.left(idx);
      }

//---------------------------------------------------------
//	LineList::line2iLine
//---------------------------------------------------------

iLineList LineList::line2iLine(int line)
      {
      int zl = 0;
      iLineList i;
      for (i = begin(); i != end(); ++i, ++zl)
            if (zl == line)
                  break;
      return i;
      }

void LineList::clear()
      {
      std::list<Line>::clear();
      lines = 0;
      }

LineList::~LineList()
      {
      }

void LineList::insert(LineList::iterator i, Line* l)
      {
      std::list<Line>::insert(i, *l);
      ++lines;
      int cols = l->columns();
      w = cols > w ? cols : w;
      }
void LineList::push_back(Line* l)
      {
      std::list<Line>::push_back(*l);
      ++lines;
      int cols = l->columns();
      w = cols > w ? cols : w;
      }

void LineList::splice(iLineList dst, LineList& ll,
   iLineList sl, iLineList el)
      {
      int n = 0;
      for (iLineList i = sl; i != el; i++, n++) {
            int cols = i->columns();
            w = cols > w ? cols : w;
            }
      if (n != distance(sl, el)) {
            fprintf(stderr, "interner Fehler: %d != %td\n", n, distance(sl, el));
            exit(1);
            }
      std::list<Line>::splice(dst, ll, sl, el);
      ll.lines -= n;
      lines += n;
      }

//---------------------------------------------------------
//    LinePos
//---------------------------------------------------------

LinePos::LinePos(LineList*lst, iLineList il, int line, int col)
 : il(&*il, col)
      {
      list  = lst;
      ilist = il;
      ln    = line;
      }

QChar LinePos::operator*()
      {
      if (ilist == list->end())
            return QChar(0);
      QChar c = *il;
      if (c == 0)
            return QLatin1Char('\n');
      return c;
      }

//---------------------------------------------------------
//   match
//---------------------------------------------------------

bool LinePos::match(const char* s) const
      {
      return il.match(s);
      }

//---------------------------------------------------------
//    LinePos::bot
//    return true if at begin of text
//---------------------------------------------------------

bool LinePos::bot()
      {
      return (ilist == list->begin() && il.col == 0);
      }

//---------------------------------------------------------
//    LinePos::decr
//---------------------------------------------------------

void LinePos::decr()
      {
      if (bot())
            return;
      if (il.col == 0) {
            --ilist;
            --ln;
            il = iLine(&*ilist, 0);
            while (!(*il).isNull())
                  ++il;
            }
      else
            --il;
      }

void LinePos::incr()
      {
      if (ilist == list->end())
            return;
      if (*il == 0) {
            ++ilist;
            ++ln;
            if (ilist != list->end())
                  il = iLine(&*ilist, 0);
            }
      else
            ++il;
      }

void LinePos::skipline()
      {
      if (ilist == list->end())
            return;
      ++ilist;
      ++ln;
      if (ilist != list->end())
            il = iLine(&*ilist, 0);
      }

//---------------------------------------------------------
//	strncmp
//---------------------------------------------------------

int strncmp(const LinePos& a, const LinePos& b, int n)
      {
      LinePos la = a;
      LinePos lb = b;
      while (n--) {
            QChar ca = *la;
            QChar cb = *lb;
            if (ca.isNull() || ca != cb)
                  return ca > cb ? 1 : -1;
            ++la;
            ++lb;
            }
      return 0;
      }

//---------------------------------------------------------
//	strncmp
//---------------------------------------------------------

int strncmp(const LinePos& a, const char* b, int n)
      {
      LinePos la = a;
      while (n--) {
            QChar ca = *la;
            QChar cb = *b;
            if (ca.isNull() || ca != cb)
                  return ca > cb ? 1 : -1;
		++la;
            ++b;
		}
      return 0;
	}

void LinePos::operator+=(int i)
	{
      while(i--)
            incr();
	}
void LinePos::operator-=(int i)
      {
      while (i--)
            decr();
      }
LinePos& LinePos::operator-(int i)
      {
      while (i--)
            decr();
      return *this;
      }

bool LinePos::operator==(const LinePos& pos) const
      {
      return (ln == pos.ln) && (il.col == pos.il.col);
      }

LinePos LinePos::operator++(int) {
	LinePos p = *this;
	incr();
	return p;
	}
LinePos& LinePos::operator--() {
	decr();
	return *this;
	}
LinePos& LinePos::operator++() {
	incr();
	return *this;
	}
int LinePos::operator-(LinePos&) {
      printf("LinePos: - LinePos\n");
      return 0;
      }
bool LinePos::operator<(LinePos& pos) {
      return (ln < pos.ln) || (ln == pos.ln && il.col < pos.il.col);
      }
bool LinePos::operator>(LinePos& pos) {
      return (ln > pos.ln) || (ln == pos.ln && il.col > pos.il.col);
      }
bool LinePos::operator>=(LinePos& pos) {
      return (ln >= pos.ln) || (ln == pos.ln && il.col >= pos.il.col);
	}
void LinePos::to_lastchar()
      {
      while (!(*il).isNull())
            ++il;
      --il;
      }
bool LinePos::eol()
      {
      return *il == 0;
      }
bool LinePos::sol()
      {
      return il.col == 0;
      }
int LinePos::col() {
      return il.col;
      }

//---------------------------------------------------------
//   class iLine
//      Line* l;
//      char* p;
//   public:
//      int col;
//---------------------------------------------------------

iLine::iLine(Line* line, int spalte)
      {
      l = line;
      setcol(spalte);
      }

iLine::iLine(Line* line)
      {
      l = line;
      p = 0;
      }

bool iLine::eol()
      {
      return (p >= l->size());
      }

QChar iLine::operator*()
      {
      if (eol())
            return QChar::Null;
      QChar c = l->txt[p];
      return (c == '\t')  ? ' ' : c;
      }

//---------------------------------------------------------
//   match
//---------------------------------------------------------

bool iLine::match(const char* s) const
      {
      return l->txt.mid(p, strlen(s)) == s;
      }

void iLine::incr()
      {
      ++col;
      if (!eol()) {
            QChar c = l->txt[p];
            if (c.isHighSurrogate())
                  ++p;
            if (!(l->txt[p] == '\t' && (col % TABL)))
                  ++p;
            }
      }

void iLine::decr()
      {
      if (col == 0)
            return;
      --col;
      setcol(col);
      }

iLine iLine::operator++(int)        // postfix
      {
      iLine np = *this;
      incr();
      return np;
      }
iLine& iLine::operator++()
      {
      incr();
      return *this;
      }
iLine iLine::operator--(int)
      {
      iLine p = *this;
      decr();
      return p;
      }
iLine& iLine::operator--()
      {
      decr();
      return *this;
      }
iLine& iLine::operator-(int i)
      {
      while (i--)
            decr();
      return *this;
      }
void iLine::operator-=(int i)
      {
      while(i--)
            decr();
      }
void iLine::operator+=(int i)
      {
      while(i--)
            incr();
      }

//---------------------------------------------------------
//    iLine::insert_char
//---------------------------------------------------------

void iLine::insert_char(QChar c)
      {
      l->insert_char(col, c);
      setcol(col);
      }

//---------------------------------------------------------
//    iLine::delete_char
//---------------------------------------------------------

void iLine::delete_char()
      {
      if (eol())
            return;
      l->delete_char(col);
      setcol(col);
      }

//---------------------------------------------------------
//   setcol
//    updatet p
//---------------------------------------------------------

void iLine::setcol(int c)
      {
      p = 0;
      for (col = 0; col < c; col++) {
            if (!eol() && !(l->txt[p] == '\t' && ((col+1) % TABL)))
                  ++p;
            }
      }

//---------------------------------------------------------
//    read_from_script
//	return true if error
//---------------------------------------------------------

#define LINMAX (1024*32)
bool read_from_script(LineList& ll, const char* script)
      {
      FILE* f = popen(script, "r");
      if (!f)
            return true;
      char buffer[LINMAX];
      int n = 0;
      int len;
      for (;;++n) {
            *buffer = 0;
            if ((len = xgets(buffer, LINMAX, f)) < 0)
                  break;
            //
            //    Backstep filtern
            //
            buffer[len] = 0;
            char* s = buffer;
            char* d = buffer;
            while (*s) {
                  if (*s == 0x8) {
                        if (d != buffer) {
                              --d;
                              len-=2;
                              }
                        }
                  else {
                        *d++ = *s;
                        }
                  ++s;
                  }
            ll.push_back(new Line(buffer, len));
            }
      pclose(f);
      return (len == -2);
      }

//---------------------------------------------------------
//   write
//---------------------------------------------------------

void LineList::write(QTextStream& f)
      {
      for (iLineList l = begin();l != end(); ++l) {
            //
            // remove trailing space
            //
            int n = l->text().size();
            while (n && l->text()[n - 1].isSpace())
                  n--;
            if (n)
                  f << l->text().left(n) << '\n';
            else
                  f << '\n';
            }
      }

