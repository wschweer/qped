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
#include "text.h"

//---------------------------------------------------------
//	Kontext::initSearch
//---------------------------------------------------------

bool Kontext::initSearch(const QString& param)
      {
#if 0  // TODO
      QString pattern, replace;

      int idx = param.lastIndexOf(QChar('/', 0));
      if (idx > 0 && param[idx-1] != QChar('\\', 0) && idx < param.size()-1) {
            pattern = param.left(idx);
            replace = param.mid(idx+1);
            }
      else {
            pattern = param;
            replace = "";
            }
      if (idx > 0 && pattern[idx-1] == QChar('\\', 0))
            pattern.remove(idx-1);
      if (!pattern.isEmpty())
            ped->searchPattern.setPattern(pattern);
      if (ped->searchPattern.pattern().isEmpty())
            return false;

      if (!ped->searchPattern.isValid()) {
            ped->msg(2000, "bad regular expression");
            ped->searchPattern.setPattern("");
            return false;
            }
      ped->replace = replace;
#endif
      return true;
      }

//---------------------------------------------------------
//    Kontext::searchR
//    search reverse
//---------------------------------------------------------

void Kontext::searchR(const QString& param)
      {
      if (!initSearch(param))
            return;
#if 0
      while (!p->bot()) {
            p->decr();
            if (stepr(*p, search_s)) {
                  adjust_pos(p->line(), p->col());
                  if (!f->readonly() && replace_op)
                        do_replace(p->cur_line(), loc1, loc2, replace);
                  register_update(UPDATE_ALL);
                  break;
                  }
            else {
                  if (p->bot()) {
                        ped->msg(2000, "\"%s\" not found", ped->searchPattern.pattern().toLatin1().data());
                        break;
                        }
                  }
            }
      delete p;
#endif
      }

//---------------------------------------------------------
//    Kontext::searchF
//    search forward
//---------------------------------------------------------

void Kontext::searchF(const QString& param)
      {
      if (!param.isEmpty() && !initSearch(param))
            return;
      int col  = pos.spalte;
      int line = 0;
      for (iLineList i = cursor; i != text->end(); ++i) {
            int idx = i->column2idx(col);
            int n   = ped->searchPattern.indexIn(i->text(), idx);
            if (n != -1) {
                  int len = ped->searchPattern.matchedLength();
                  if (ped->replace.isEmpty()) {
                        pos.spalte = i->idx2column(n + len);
                        }
                  else {
                        QStringList cl = ped->searchPattern.capturedTexts();
                        QString r = ped->replace;
                        for (int ii = 1; ii < cl.size(); ++ii) {
                              int idx = r.indexOf(QString("\\%1").arg(ii));
                              if (idx == -1)
                                    break;
                              r.replace(idx, 2, ped->searchPattern.cap(ii));
                              }
                        i->replace(n, len, r);
                        f->setModified();
                        pos.spalte = i->idx2column(n + r.size());
                        }
                  if ((pos.yoffset + line) < (zeilen() - KONTEXT_LINES))
                        pos.yoffset += line;
                  pos.zeile += line;
                  cursor = i;

                  // horizontal scrollen ?
                  int xmax = spalten() - 1 - KONTEXT_COLUMNS;
                  if (pos.spalte > xmax)
                        pos.xoffset = pos.spalte - xmax;
                  else if (pos.xoffset && (pos.spalte - pos.xoffset <= 1))
                        pos.xoffset = pos.spalte - 1;

                  register_update(UPDATE_ALL);
                  return;
                  }
            col = 0;
            ++line;
            }
      ped->msg(2000, QString("\"%1\" not found").arg(ped->searchPattern.pattern()));
      }

//---------------------------------------------------------
//   search_kollaps
//---------------------------------------------------------

void Kontext::search_kollaps(const QString& param)
      {
      if (view_mode == 1)     // kein search_kollaps im search_kollaps
            return;
      if (!initSearch(param))
            return;
      kollaps_list.clear();
      view_mode      = 1;
      int view_zl    = 0;
      int n          = 0;
      int matchLines = 0;

      for (iLineList i = text->begin(); i != text->end(); ++i, ++n) {
            int rv = ped->searchPattern.indexIn(i->text(), 0);
            if (rv != -1) {
                  Line* nl = new Line(*i);
                  nl->setTag(n);
                  if (pos.zeile >= n)
                        view_zl = matchLines;
                  kollaps_list.push_back(nl);
                  ++matchLines;
                  }
            }
      if (matchLines == 0)
            return;
      pos.zeile = view_zl;
      text = &kollaps_list;

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

