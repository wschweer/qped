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
#include "file.h"
#include "cmd.h"
#include "text.h"
#include "utils.h"

extern bool changeInPlace;

FileList files;

//---------------------------------------------------------
//    File::File
//---------------------------------------------------------

File::File(const QFileInfo& info, FileType t)
      {
      state      = F_VIRGIN;
      _type      = FILE_UNKNOWN;
      mode       = QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::ReadOther;
      dirty      = false;
      modified   = false;
      _readonly  = true;
      _type      = t;
      _fi        = info;
      subscriber = new SubscriberList;
      }

//---------------------------------------------------------
//   tabl
//---------------------------------------------------------

int File::tabl() const
      {
      switch(_type) {
            case FILE_QML:    return 4;
            default:          return 6;
            }
      }

//---------------------------------------------------------
//    File::~File
//---------------------------------------------------------

File::~File()
      {
      delete subscriber;
      }

//---------------------------------------------------------
//    FileList::~FileList
//---------------------------------------------------------

FileList::~FileList()
      {
      for (auto i = begin(); i != end(); ++i)
            delete *i;
      clear();
      }

//---------------------------------------------------------
//    FileList::saveall
//---------------------------------------------------------

void FileList::saveall()
      {
      for (auto i = begin(); i != end(); ++i)
            (*i)->save(0);
      }

//---------------------------------------------------------
//    File::find
//---------------------------------------------------------

File* FileList::find(QFileInfo fi) const
      {
      for (auto i = begin(); i != end(); ++i) {
            if ((*i)->fi() == fi)
                  return *i;
            }
      return 0;
      }

//---------------------------------------------------------
//    update
//---------------------------------------------------------

void File::updateFile(void* p)
      {
      if (dirty) {
            for (iKontextList k = subscriber->begin(); k != subscriber->end(); ++k)
                  (*k)->updateDoc(p);
            }
      dirty = false;
      }

//---------------------------------------------------------
//    File::subscribe
//---------------------------------------------------------

void File::subscribe(Kontext*k)
 	{
      subscriber->push_back(k);
	}

//---------------------------------------------------------
//    File::unsubscribe
//---------------------------------------------------------

int File::unsubscribe(Kontext*k)
	{
      for (iKontextList i = subscriber->begin(); i != subscriber->end(); ++i) {
            if (*i == k) {
                  subscriber->erase(i);
                  break;
                  }
            }
	return subscriber->size();
	}

//---------------------------------------------------------
//    File::save
//    1. write temp file
//    2. mv orig-file backup-file
//    3. mv temp-file orig-file
//    4. (append diff)
//---------------------------------------------------------

void File::save(const char* name)
      {
      if (name) {
            saveAs(name);
            return;
            }
      if (!modified)
            return;
      if (changeInPlace) {
            QFile f(_fi.filePath());
            if (!f.open(QIODevice::WriteOnly)) {
                  QString s = QString("Open File\n") + f.fileName() + QString("\nfailed: ")
                     + QString(strerror(errno));
                  QMessageBox::critical(0, "cannot open file", s);
                  return;
                  }
            QTextStream os(&f);
            if (utf8)
                  os.setCodec(QTextCodec::codecForName("utf8"));
            else if (isoLatin)
                  os.setCodec(QTextCodec::codecForName("latin1"));
            top.write(os);
            if (f.error()) {
                  QString s = QString("Write Temp File\n") + f.fileName() + QString("\nfailed: ")
                     + f.errorString();
                  QMessageBox::critical(0, QString("Ped: Write Temp"), s);
                  return;
                  }
            f.close();
            return;
            }
      QTemporaryFile temp(_fi.path() + "/PEDXXXXXX");
      temp.setAutoRemove(false);
      if (!temp.open()) {
             QString s = QString("Open Temp File\n") + temp.fileName() + QString("\nfailed: ")
               + QString(strerror(errno));
            QMessageBox::critical(0, QString("Ped: Open Temp"), s);
            return;
            }
      QTextStream os(&temp);
      if (utf8)
            os.setCodec(QTextCodec::codecForName("utf8"));
      else if (isoLatin)
            os.setCodec(QTextCodec::codecForName("latin1"));

      top.write(os);
      if (temp.error()) {
             QString s = QString("Write Temp File\n") + temp.fileName() + QString("\nfailed: ")
               + temp.errorString();
            QMessageBox::critical(0, QString("Ped: Write Temp"), s);
            return;
            }

      //
      // remove old backup file if exists
      //
      QDir dir(_fi.path());
      QString backupName = QString(".") + _fi.fileName() + QString(",");
      dir.remove(backupName);

      //
      // rename old file into backup
      //
      QString n(_fi.filePath());
      dir.rename(n, backupName);

      //
      // rename temp name into file name
      //
      temp.rename(n);

      modified = false;
      if (!QFile::setPermissions(n, mode))
            printf("set permissions 0x%x on <%s> failed\n", int(mode), n.toLatin1().data());
      }

//---------------------------------------------------------
//    File::save
//---------------------------------------------------------

bool File::saveAs(const char* name)
      {
      QFile f(name);
      if (!f.open(QIODevice::WriteOnly))
            return true;
      QTextStream os(&f);
      top.write(os);
      bool error = f.error() != QFile::NoError;
      f.close();
      modified = false;
      return error;
      }

//---------------------------------------------------------
//    File::open()
//---------------------------------------------------------

void File::open()
      {
      if (state != F_VIRGIN)
            return;
      QFile qf(path());
      if (qf.exists())
            mode = qf.permissions();
      if (!qf.open(QIODevice::ReadOnly)) {
            // sollte nicht passieren wg. access() - test
            top.push_back(new Line("", 0));
            state = F_CREATED;
            return;
            }
      int n;
      for (n = 0;; ++n) {
            QByteArray ba = qf.readLine(1024 * 32);
            int size = ba.size();
            if (size == 0)
                  break;
            if (size && ba.at(size-1) == '\n') {
                  --size;
                  ba[size] = 0;
                  }
            QString s;
            if (utf8)
                  s = QString::fromUtf8(ba.constData());
            else if (isoLatin)
                  s = QString::fromLatin1(ba.constData());

            top.push_back(new Line(s));
            }
      if (n == 0)
            top.push_back(new Line(QString("")));
      qf.close();
      state = F_NORMAL;
      }

//---------------------------------------------------------
//    read_from_script
//	return true if error
//---------------------------------------------------------

bool File::read_from_script(const char* script)
      {
      return ::read_from_script(top, script);
      }

bool File::undo_ok()  const
      {
      bool val = !undo_list.empty() && static_cast<ciUndoList> (undo_list.cur_undo) != static_cast<ciUndoList> (undo_list.begin());
      return val;
      }
bool File::redo_ok() const
      {
      return static_cast<ciUndoList> (undo_list.cur_undo) != static_cast<ciUndoList> (undo_list.end());
      }

//---------------------------------------------------------
//    File::insert_char
//---------------------------------------------------------

void File::insert_char(Line* l, int xpos, const QChar& c)
      {
      l->insert_char(xpos, c);
      top.updateWidth(l->columns());
      modified = true;
      dirty = true;
      }

//---------------------------------------------------------
//    File::delete_columns
//---------------------------------------------------------

void File::delete_columns(iLineList sl, iLineList el, int col1,
   int col2)
      {
      if (col2 < col1) {
            int tmp = col1;
            col1 = col2;
            col2 = tmp;
//            swap(col1, col2);
            }
      iLineList l = sl;
      do {
            undo_list.append_line(&*l);
            l->del(col1, col2);
            ++l;
            } while (l != el);
      modified = true;
      dirty = true;
      }

//---------------------------------------------------------
//    MtextFile::redo
//---------------------------------------------------------

Position* File::redo(Kontext* kontext)
      {
      Undo* undo = undo_list.cur_undo == undo_list.end()
         ? 0 : *(undo_list.cur_undo);
      if (undo == 0 || undo->type == UNDO_NOTHING)
            return 0;
      undo_redo(kontext, undo);
      ++(undo_list.cur_undo);
      Position tmp(*undo->start_pos);
      *(undo->end_pos) = *(undo->start_pos);
      *(undo->start_pos) = tmp;
      return undo->end_pos;
      }

//---------------------------------------------------------
//    File::undo
//---------------------------------------------------------

Position* File::undo(Kontext* kontext)
      {
      Undo* undo = (undo_list.cur_undo == undo_list.begin())
        ? 0 : *--(undo_list.cur_undo);
      if (undo == 0 || undo->type == UNDO_NOTHING)
            return 0;
      undo_redo(kontext, undo);
      Position tmp(*(undo->start_pos));
      *(undo->end_pos) = *(undo->start_pos);
      *(undo->start_pos) = tmp;
      return undo->end_pos;
      }

//---------------------------------------------------------
//    File::undo_redo
//---------------------------------------------------------

void File::undo_redo(Kontext* kontext, Undo* undo)
      {
      Position* spos = undo->start_pos;
      iLineList l = top.line2iLine(spos->zeile);

      switch(undo->type) {
            case UNDO_CHANGE_LINE:
            	for (iLineList ll = undo->l.begin(); ll != undo->l.end(); ++ll) {
                       	l->swap(*ll);
                  	++l;
                  	}
                  break;
            case UNDO_INSERT_LINE:
                  undo->type = UNDO_DELETE_LINE;
                  undo->n = undo->l.size();
			insert_line(l, &undo->l);
                  undo->l.clear();
                  break;
            case UNDO_DELETE_LINE:
                  delete_lines(l, undo->n);
                  undo->type = UNDO_INSERT_LINE;
                  break;
            case UNDO_LINEBREAK:
                  undo->type = UNDO_LINEUNBREAK;
                  {
                  iLineList pl = l;
                  ++l;
                  append_line(pl, undo->l.begin());
                  undo_list.clear_lines();
                  delete_lines(l, 1);  // verschiebt nach undo_list
                  }
                  break;
            case UNDO_LINEUNBREAK:
                  undo->l.clear();
            	kontext->newline();
                  break;
            default:
                  fprintf(stderr, "internal Error: bad undo type\n");
                  break;
            }
      modified = true;
      dirty = true;
      }

//---------------------------------------------------------
//    File::delete_char
//---------------------------------------------------------

void File::delete_char(Line* l, int xpos)
      {
      undo_list.append_line(l);
      l->delete_char(xpos);
      modified = true;
      dirty = true;
      }

//---------------------------------------------------------
//   replace
//---------------------------------------------------------

void File::replace(Line* l, const iLine ib, const iLine ie,
   const char* txt)
      {
      undo_list.append_line(l);

      int scol = ib.col;
      int ecol = ie.col;
      int n = ecol - scol;

      while (n--)
            l->delete_char(scol);
      while (*txt)
            l->insert_char(scol++, *txt++);
      modified = true;
      dirty = true;
      }

//---------------------------------------------------------
//    MtextFile::delete_lines
//---------------------------------------------------------

void File::delete_lines(iLineList sl, int n)
      {
      iLineList el = sl;
      while (n--) {
            ++el;
            if (el == top.end())
                  break;
            }
      delete_lines(sl, el);
      modified = true;
      dirty = true;
      }

//---------------------------------------------------------
//    MtextFile::delete_lines
//---------------------------------------------------------

void File::delete_lines(iLineList sl, iLineList el)
      {
      undo_list.splice(&top, sl, el);
      modified = true;
      dirty = true;
      }

//---------------------------------------------------------
//    MtextFile::append_line
//---------------------------------------------------------

void File::append_line(iLineList d, iLineList s)
      {
      d->append(*s);
      modified = true;
      dirty = true;
      }

//---------------------------------------------------------
//    MtextFile::insert_column
//---------------------------------------------------------

void File::insert_column(Line* d, Line*s, int col)
      {
      iLine dst(d, col);
      iLine src(s, 0);

	undo_list.append_line(d);
	for (;;) {
      	QChar c = *src++;
            if (c == QChar::Null)
            	break;
        	dst.insert_char(c);
            ++dst;
            }
      modified = true;
      dirty = true;
      }

//---------------------------------------------------------
//    MtextFile::insert_line
//---------------------------------------------------------

void File::insert_line(iLineList l, const char* txt, int len)
      {
      insert_line(l, new Line(txt, len));
      }

void File::insert_line(iLineList l, ciLineList s)
      {
      insert_line(l, &*s);
      }

void File::insert_line(iLineList l, const Line* line)
      {
      top.insert(l, new Line(*line));
      modified = true;
      dirty = true;
      }

void File::insert_line(iLineList dst, const LineList* src)
	{
  	for (ciLineList i = src->begin(); i != src->end(); ++i)
            insert_line(dst, i);
    	}

//---------------------------------------------------------
//    File::delete_restline
//---------------------------------------------------------

void File::delete_restline(iLineList l, int col)
      {
      l->delete_rest(col);
      modified = true;
      dirty = true;
      }

//---------------------------------------------------------
//    File::del_word
//---------------------------------------------------------

void File::del_word(Line* l, int col)
      {
      l->delete_word(col);
      modified = true;
      dirty = true;
      }

Undo::Undo(const Position* p)
      {
      start_pos = new Position;
      end_pos   = new Position;
      *start_pos = *p;
      type = UNDO_NOTHING;
      }

Undo::~Undo()
      {
      delete start_pos;
      delete end_pos;
      }

void Undo::sPos(Position* p)
      {
      *start_pos = *p;
      }

