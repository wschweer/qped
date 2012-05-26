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

#ifndef __line_h__
#define __line_h__

#define INDENT_NORMAL 0
#define INDENT_C 1

//---------------------------------------------------------
//    class Line
//---------------------------------------------------------

class Line {
   protected:
      QString txt;
      int _tag;

   public:
      Line(const QString& t, int l);
      Line(const Line&);
      Line(const QString& t);

      int size() const   { return txt.size(); }
      int tag() const    { return _tag; }
      void setTag(int t) { _tag = t; }

      void indent(int columns);
      void swap(Line& l);

      char* cstring() const;
      const QString& text() const { return txt; }

      static Line* splitLine(const Line*, int col);
      int columns() const;
      int idx2column(int) const;
      int column2idx(int) const;
      Line* columns(int, int) const;
      void delete_char(int col);
      void change(const QString& p);
      void insert_char(int col, QChar c);
      void append(Line& s);
      void delete_rest(int column);
      void delete_word(int col);
      int getindent(int);
      void del(int scol, int ecol);
      void replace(int i, int n, const QString s) { txt.replace(i, n, s); }
      friend class iLine;
      };

//---------------------------------------------------------
//    class LineList
//---------------------------------------------------------

class LineList: public std::list<Line> {
      int lines;
      int w;            // breite der bounding box
   public:
      LineList()    { lines = 0; w = 0;}
      ~LineList();
      void clear();
      void updateWidth(int n) { w = n > w ? n : w; }
      LineList::iterator line2iLine(int line);
      int size() const    {  return lines;  }
      int columns() const { return w; }
      void push_back(Line* l);
      void insert(LineList::iterator i, Line* l);
      const LineList::const_iterator begin() const { return std::list<Line>::begin(); }
      const LineList::const_iterator end() const  { return std::list<Line>::end(); }
      LineList::iterator begin() { return std::list<Line>::begin(); }
      LineList::iterator end()   { return std::list<Line>::end(); }
      void splice(LineList::iterator dst, LineList& ll, LineList::iterator sl, LineList::iterator el);
      void erase(LineList::iterator sl, LineList::iterator el) {
            std::list<Line>::erase(sl, el);
            lines -= distance(sl, el);
            }
      bool empty() const { return std::list<Line>::empty(); }
      void write(QTextStream& f);
      };

typedef LineList::iterator iLineList;
typedef LineList::const_iterator ciLineList;
class LinePos;

//---------------------------------------------------------
//    iLine
//    ein iLine Objekt realisiert das Pointer Paradigma
//    für eine Line
//---------------------------------------------------------

class iLine {
      Line* l;
      int p;            // index auf aktuelles Zeichen

   public:
      int col;          // aktuelle logische Spalte

      iLine() { p = 0;}
      iLine(Line*);
      iLine(Line*, int);

      int size() const { return p; }
      QChar operator*();
      iLine operator++(int);
      iLine& operator++();
      iLine operator--(int);
      iLine& operator--();
      iLine& operator-(int i);
      void operator-=(int i);
      void operator+=(int i);
      void incr();
      void decr();
      void insert_char(QChar c);
      void delete_char();
      void setcol(int);
      void restline(Line*);
      friend bool operator==(iLine& ln, const char* txt);
      bool eol();
      const char* txt() const {
            return l->txt.toAscii().data() + p;
            }
      bool match(const char*) const;
      };

//---------------------------------------------------------
//    LinePos
//    implementiert das Pointer Paradigma auf eine
//    LineList, die aus Zeilen besteht
//---------------------------------------------------------

class LinePos {
      LineList* list;
      iLineList ilist;
      iLine il;
      int ln;

   public:
      LinePos() {}
      LinePos(LineList*lst, iLineList il, int line, int col);
      Line* cur_line() const  { return &(*ilist); }
      iLine cur_iline() const { return il; }
      void setIline(const iLine& l) { il = l; }
      void decr();
      void incr();
      QChar operator*();
      LinePos operator++(int);
      LinePos& operator--();
      LinePos& operator++();
      LinePos& operator-(int i);
      void operator-=(int i);
      void operator+=(int i);
      void skipline();
      int operator-(LinePos& pos);
      bool operator<(LinePos& pos);
      bool operator>(LinePos& pos);
      bool operator>=(LinePos& pos);
      bool operator==(const LinePos& pos) const;
      friend int strncmp(const LinePos& a, const LinePos& b, int n);
      friend int strncmp(const LinePos& a, const char* b, int n);
      void to_lastchar();
      bool eol();
      bool sol();
      bool bot();
      void copy_first_part(char*& dst);
      int col();
      int line() { return ln; }
      bool match(const char*) const;
      };

extern bool read_from_script(LineList&, const char*);

#endif
