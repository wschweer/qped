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

#ifndef __file_h__
#define __file_h__

#include "line.h"

class Editor;
class SubscriberList;

enum FileType {FILE_UNKNOWN, FILE_TEXT, FILE_GRAFIK, FILE_C,
   FILE_MAN, FILE_H, FILE_HTML, FILE_QML};

// KONTEXT_LINES must be less than (window_height / 2) - 1

#define KONTEXT_LINES   1
#define KONTEXT_COLUMNS 1
#define PAGE_OVERLAP    3

class Ped;
class File;
class Link;
class Kontext;
class Position;

//-------------------------------------------------------
//	struct Undo
//-------------------------------------------------------

enum UndoType { UNDO_NOTHING, UNDO_CHANGE_LINE, UNDO_INSERT_LINE,
      UNDO_DELETE_LINE, UNDO_LINEBREAK, UNDO_LINEUNBREAK };

struct Undo {
      UndoType type;
      Position* start_pos;
      Position* end_pos;
      LineList l;
      int n;

      Undo(const Position* p);
      ~Undo();
      void sPos(Position* p);
      };

typedef std::list<Undo*>::iterator iUndoList;
typedef std::list<Undo*>::const_iterator ciUndoList;

//-------------------------------------------------------
//	class UndoList
//-------------------------------------------------------

class UndoList:public std::list<Undo*> {

   protected:
      iUndoList cur_undo;

   public:
      friend class File;
      UndoList() { cur_undo = end(); }
      ~UndoList() {}    // TODO: lschen der Liste fehlt!
      void start_new(const Position* p);
      void sPos(Position* p) { (*cur_undo)->sPos(p); }
      void end_new(const Position* p);
      void append_line(const Line* l);
      void splice(LineList* ll, iLineList sl, iLineList el) {
            LineList* lst = &((*cur_undo)->l);
            lst->splice(lst->end(), *ll, sl, el);
            }
	void clear_lines() {(*cur_undo)->l.clear(); }
      };

//---------------------------------------------------------
//    class File
//---------------------------------------------------------

class File {
      UndoList undo_list;

   protected:
      QFileInfo _fi;
      QFile::Permissions mode;
      LineList top;

      enum FileState {F_VIRGIN = 0, F_CREATED, F_NORMAL};
      enum FileState state;
      bool modified;
      SubscriberList* subscriber;
      FileType _type;
      char* construct_name(const char*);
      bool dirty;
      bool _readonly;

   public:
      File(const QFileInfo&, FileType);
      ~File();

      bool saveAs(const char* name);

      QFileInfo fi() const         { return _fi;                    }
      QString name() const         { return _fi.fileName();         }
      QString path() const         { return _fi.absoluteFilePath(); }

      bool readonly() const        { return _readonly; }
      void setReadonly(bool flag)  { _readonly = flag; }

      FileType type() const        { return _type; }

      void subscribe(Kontext*);
	int unsubscribe(Kontext*);
      void updateFile(void*);

      void open();
      void close()                 { save(0); }
      void save(const char* name);

      LineList* Text()             { return &top; }

      bool read_from_script(const char* script);

      void undo_start(Position* p)       { undo_list.start_new(p); }
      void undo_spos(Position* p)        { undo_list.sPos(p); }
      void undo_type(UndoType t)         {
		(*(undo_list.cur_undo))->type = t;
		}
      void undo_type(UndoType t, int p)        {
		(*(undo_list.cur_undo))->type = t;
		(*(undo_list.cur_undo))->n    = p;
		}
      void undo_end(Position* pos) { undo_list.end_new(pos); }
      bool undo_ok() const;
      bool redo_ok() const;
      Position* undo(Kontext*);
      Position* redo(Kontext*);
      void undo_redo(Kontext*, Undo*);
      void undo_line(Line*l) { undo_list.append_line(l); }
      void insert_char(Line* l, int xpos, const QChar& c);
      void delete_char(Line* l, int xpos);
      void delete_lines(iLineList l, int n);
      void delete_lines(iLineList sl, iLineList el);
      void append_line(iLineList d, iLineList s);
      void insert_line(iLineList l, const char*, int);
      void insert_line(iLineList d, ciLineList s);
      void insert_line(iLineList d, const Line* l);
      void insert_line(iLineList d, const LineList* s);
      void delete_restline(iLineList, int);
      void del_word(Line*, int);
	void insert_column(Line* d, Line*s, int col);
	void delete_columns(iLineList, iLineList, int, int);
      void replace(Line*, const iLine, const iLine,
         const char*);
      void setDirty() { dirty = true; }
      void setModified() { modified = true; }
      int tabl() const;
      };

//---------------------------------------------------------
//    class FileList
//---------------------------------------------------------

class FileList:public std::list<File*> {
   public:
      FileList() {}
      ~FileList();
      void saveall();
      File* find(QFileInfo) const;
      };

typedef FileList::iterator iFileList;
typedef FileList::const_iterator ciFileList;

extern FileList files;
extern enum FileType file_type(const char*, const char**a=0, const char**b=0);
extern const char* gen_path(const char*);
extern const char* gen_name(const char*);
extern void ref2pos(QFile* k, Position& pos);
#endif

