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

#ifndef KONTEXT_H
#define KONTEXT_H

class File;
class Link;
class Ped;
class Editor;
class Text;

#include "line.h"

enum MarkMode { MARK_NONE=0, MARK_LINES, MARK_COLUMNS };

//---------------------------------------------------------
//    Position
//    verknpft Cursorposition mit Fileposition
//---------------------------------------------------------

struct Position {
      int zeile;        // vertical offset of cursor in file
      int spalte;       // horizontal offset of cursor in line
      int yoffset;      // cursor screen line
      int xoffset;      // position of screen column 0
      Position() {
            zeile   = 0;
            spalte  = 0;
            yoffset = 0;
            xoffset = 0;
            }
      friend bool operator==(const Position&, const Position&);
      friend bool operator!=(const Position& p1, const Position& p2) {
            return !(p1 == p2);
            }
      };

//---------------------------------------------------------
//    class Kontext
//---------------------------------------------------------

class Kontext {
      File* f;
      Editor* editor;
      Ped* ped;
      bool openflag;
      int update_flags;
      bool insert_flag;
      iLineList cursor;
      Position pos;                 // Current Position in File
      bool cursor_visible;
      LineList kollaps_list;
      int view_mode;                // Darstellungsart
      MarkMode mark_mode;
      Position ompos1, ompos2;      // mark Position
      Position mpos1, mpos2;	      // mark Position
      int paren_line;

      struct TextEditFunction {
            int cmd;
            void  (Kontext::*f)(const QString& s);
            };
      struct MtextEditFunction {
            int cmd;
            void  (Kontext::*f)(const QString& s);
            bool undo;
            };

      static TextEditFunction fkt[];
      static MtextEditFunction mfkt[];

      void cmd_scroll_up(const QString&)   { yscroll(1); }
      void cmd_scroll_down(const QString&) { yscroll(-1); }
      void cmd_next(const QString&)        { rxposition(1); }
      void cmd_back(const QString&)        { rxposition(-1); }
      void cmd_up(const QString&)          { ryposition(-1); }
      void cmd_down(const QString&)        { ryposition(1); }
      void cmd_start_file(const QString&)  { yposition(0); }
      void cmd_end_file(const QString&)    { yposition(-1); }
      void cmd_page_up(const QString&)     { page(1); }
      void cmd_page_down(const QString&)   { page(-1); }
      void cmd_start_line(const QString&)  { xposition(0); }
      void cmd_end_line(const QString&)    { xposition(-1); }

      void cmd_link();
      void about();
      char* findProc(LinePos&, int);
      void save(const char* p);
      void del_restline();
      void del_word();
      void cmd_put();
      void cmd_undelete();
      void rubout();
      void insert_char(const QChar&);
      void put_char(const QChar&);
      void delete_char();
      void insert_tab();
      void undo();
      void redo();
      void delete_line();
      void put(Text&);
      void insert_special(int code);
      void searchF(const QString& param);
      void searchR(const QString& param);
      void do_replace(Line*, const LinePos&, const LinePos&, const char*);
      void insertmode();

      void adjust_pos(int, int);
      void adjust_cursor();
      int move_line(iLineList&, int);
      bool initSearch(const QString& param);

   public:
      bool edit(int, const QString&);
      void editChar(const QChar&);
      bool auto_indent;
      LineList* text;               // pointer to List of Lines

      Kontext(File*, Editor*, Ped*, const Position*);
      virtual ~Kontext();

      bool undo_ok() const;
      bool redo_ok() const;
      void open();
      void showKontext(bool);
      virtual void getpos(int& x, int& y, int& xoff, int& yoff) const;
      Line* operator*() { return &*cursor; }
      int yoffset() const;
      int xoffset() const;
      int zeilen() const;
      int spalten() const;
      void yscrollinfo(int&, int&, int&);
      void xscrollinfo(int&, int&, int&);
      bool xscrollto(int);
      int yscrollto(int);
      int yscroll(int);
      QChar getCursorInfo(int& x, int& y);
      int get_ocursor_info(int& x, int& y);
      void yposition(int);    // move cursor to absolute vertical position
      void ryposition(int);   // move cursor to relative vertical position
      virtual bool rxposition(int);
      void start_window();
      void end_window();
      void page(int);         // move cursor one page up or down
      void back_word();
      void next_word();
      void xposition(int x);
      QChar curChar();
      QChar curChar(int, int);
      QChar leftChar();
      void update();
      void update1();
      void register_update(int);
      void pick();
      void cmd_goto(const QString&);
      void search_kollaps(const QString&);
      virtual void view_toggle();
      int generate_cview(LineList*, LineList*, int zl);
      void line_mark();
      void column_mark();
      void buttonpress(int, int, bool, int);
      void buttonmotion(int, int);
      bool isMarked(int y, int& x1, int& x2);
      void split_marked(int x, int len, int& l1, int& l2, int& l3);
      QString get_word(int) const;
      int mark_interval(iLineList& sl, iLineList& el);
      void c_klammer() const;
      void cmd_compound();
      int c_compound();
      Link* getLink() const;
      LineList* body();
      MarkMode getMarkMode() const { return mark_mode; }
      iLineList topLine(int line);
      iLine sl(Line* l) const { return iLine(l, pos.xoffset); }
      QChar colorBraceV(QChar brace, int& x, int& y);
      QChar colorBraceR(QChar brace, int& x, int& y);

      void setOpenflag(bool f) {  openflag = f; }
      QString name() const;
      QString path() const;

      File* getFile() const  { return f; }
      void saveAs(const char* name);
      void updateDoc(void*);
      void newline();
      };

//---------------------------------------------------------
//    class KontextList
//---------------------------------------------------------

typedef std::list<Kontext*>::iterator iKontextList;

class KontextList:public std::list<Kontext*>  {
	iKontextList _cont;

   public:
      KontextList()        { _cont = end(); }
      ~KontextList();
      Kontext* operator*() { return *_cont; }
      iKontextList at(int i);
      void append(Kontext* k);
      bool first_kontext();
      bool last_kontext();
      void back();
      void vor();
      int cur();
      void setcur(int n);
      };

//---------------------------------------------------------
//    KontextListList
//---------------------------------------------------------

typedef std::list<KontextList*>::iterator iKontextListList;

class KontextListList : public std::list<KontextList*> {
      iKontextListList cc;          // current context list

   public:
      KontextListList()    { cc = end(); }
      Kontext* operator*() {
            // *c       KontextList*
            // **c      KontextList
            // ***c     *KontextList ->  Kontext*
            return ***cc;
            }
      ~KontextListList();
      void append(KontextList* cl);
      void append_kontext(Kontext* c);
      iKontextListList at(int i);
      int cur();
      void delete_cur_context();
      bool backhist_ok() const;
      bool vorhist_ok() const;
      void backhist();
      void vorhist();
      void setcur(int n);
      void setcur(int n, int k);
	void setcur_hist(int i, int n);
      void clear_all();
      };

class SubscriberList : public std::list<Kontext*> {
      };

typedef SubscriberList::iterator iSubscriberList;

#endif

