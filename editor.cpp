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

#include "file.h"
#include "editor.h"
#include "ped.h"
#include "cmd.h"
#include "utils.h"
#include "text.h"
#include "xml.h"
#include "line.h"
#include "config.h"
#include "editwin.h"

//---------------------------------------------------------
//   removeKontext
//---------------------------------------------------------

void Editor::removeKontext(int idx)
      {
      fb->blockSignals(true);
      fb->removeTab(idx);
      fb->blockSignals(false);
      edit_cmd(CMD_REM_ALTFIL);
      }

//---------------------------------------------------------
//   Editor
//---------------------------------------------------------

Editor::Editor(QWidget* parent, Ped* p, const QFont& efont)
   : QWidget(parent)
      {
      o_xvisible = o_xtotal = o_xpos = -1;
      o_yvisible = o_ytotal = o_ypos = -1;
      editStackId = 0;
      ped = p;

      QVBoxLayout* tgrid = new QVBoxLayout(this);
      tgrid->setSpacing(0);
      tgrid->setMargin(0);

      fb      = new TabBar(this);
      stack   = new QStackedWidget(this);
      tgrid->addWidget(fb, 0, Qt::AlignLeft);
      tgrid->addWidget(stack, 5);

      eframe  = new QWidget(stack);
      grid    = new QGridLayout(eframe);
      grid->setSpacing(0);
      grid->setMargin(0);

      hScroll = new QScrollBar(Qt::Horizontal, eframe);
      vScroll = new QScrollBar(Qt::Vertical, eframe);
      win     = new EditWin(eframe, ped, this);
      win->setFont(efont);

      grid->addWidget(win, 0, 0);
      grid->addWidget(vScroll, 0, 1);
      grid->addWidget(hScroll, 1, 0);

      grid->setRowStretch(0, 5);
      grid->setColumnStretch(0, 5);

      connect(fb,      SIGNAL(currentChanged(int)), this, SLOT(gotoKontext(int)));
      connect(fb,      SIGNAL(doubleClick(int)),   this, SLOT(removeKontext(int)));
      connect(hScroll, SIGNAL(valueChanged(int)),  this, SLOT(hScrollTo(int)));
      connect(vScroll, SIGNAL(valueChanged(int)),  this, SLOT(vScrollTo(int)));
      addStack(eframe);
      }

//---------------------------------------------------------
//   is_delim
//---------------------------------------------------------

#if 0
static bool is_delim(QChar c)
      {
      switch (c.toAscii()) {
            case ' ':
            case ';':
            case ':':
            case '*':
            case '(':
            case ')':
            case ',':
                  return true;
            default:
                  return false;
            }
      }
#endif

//---------------------------------------------------------
//   is_keyword
//---------------------------------------------------------

#if 0
static int is_keyword(const char* s)
      {
      char* keywords[] = {
            "for", "while", "do", "if", "else", "switch",
            "const", "int", "long", "short", "char", "bool",
            "true", "false", "class", "struct", "case", "default", "void",
            "double", "float", "new", "delete", "return",
            "typedef", "static", "enum", "break", "extern", "continue",
            "throw", "catch", "try", "unsigned", "signed", "inline",
            "mutable",
            0};
      for (char** p = keywords; *p; ++p) {
            int n = strlen(*p);
            if (::strncmp(s, *p, n) == 0 && is_delim(*(s+n)))
                  return n;
            }
      return 0;
      }
#endif

//---------------------------------------------------------
//   leaveEnterInput
//---------------------------------------------------------

void Editor::leaveEnterInput(int code, const QString& s)
      {
      if (code == CMD_FUNCTION) {
            ((Kontext*)(*kll))->xposition(0);
            switch((*kll)->getFile()->getFileType()) {
                  case FILE_C:
                        ped->expand_c_proc(s);
                        break;
                  case FILE_H:
                        ped->expand_h_proc(s);
                        break;
                  default:
                        break;
                  }
            }
      else if (code == CMD_SHELL) {
            FILE* f = popen(s.toLatin1().data(), "r");
            int c;
            while ((c = fgetc(f)) != EOF) {
                  edit(QChar(c));
                  }
            update();
            }
      else if (code != ESCAPE)
            edit_cmd(code, s);
      win->setFocus();
      }

//---------------------------------------------------------
//   saveAs
//---------------------------------------------------------

void Editor::saveAs(const char* name)
      {
      (*kll)->saveAs(name);
      }

//---------------------------------------------------------
//   saveStatus
//---------------------------------------------------------

void Editor::saveStatus(Xml& xml, const char* name)
      {
      xml.tag(name);
      int nn = 0;
      for (iKontextListList k = kll.begin(); k != kll.end(); ++k, ++nn) {
            xml.tag("KontextList");
            KontextList* kl = *k;
            int n = 0;
            for (iKontextList i = kl->begin(); i != kl->end(); ++i, ++n) {
                  xml.tag("Kontext");
                  xml.strTag("path", (*i)->path());
            	int column, line, xoff, yoff;
            	(*i)->getpos(column, line, xoff, yoff);
                  xml.intTag("column", column);
                  xml.intTag("line", line);
                  xml.intTag("xoff", xoff);
                  xml.intTag("yoff", yoff);
                  xml.etag("Kontext");
            	}
            if (nn == kll.cur())
                  xml.intTag("cur", 1);
            xml.etag("KontextList");
            }
      xml.etag(name);
      }

//---------------------------------------------------------
//   loadStatus
//---------------------------------------------------------

bool Editor::loadStatus(QDomNode node, bool load_files)
      {
      int i  = 0;
      int cur = 0;
      int nn = 0;
      for (node = node.firstChild(); !node.isNull(); node = node.nextSibling()) {
            QDomElement e = node.toElement();
            if (e.isNull())
                  continue;
            QString tag(e.tagName());
            if (tag != "KontextList") {
                  domError(node);
                  continue;
                  }
            QString val(e.text());
            int ii = 0;
            for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
                  QDomElement e = n.toElement();
                  QString tag(e.tagName());
                  if (tag == "Kontext") {
                        if (load_files) {
                              QString path;
                              Position pos;
                              for (QDomNode nn = n.firstChild(); !nn.isNull(); nn = nn.nextSibling()) {
                                    QDomElement e = nn.toElement();
                                    QString tag(e.tagName());
                                    QString val(e.text());
                                    int i = val.toInt();
                                    if (tag == "path")
                                          path = val;
                                    else if (tag == "column")
                                          pos.spalte = i;
                                    else if (tag == "line")
                                          pos.zeile = i;
                                    else if (tag == "xoff")
                                          pos.xoffset = i;
                                    else if (tag == "yoff")
                                          pos.yoffset = i;
                                    else if (tag == "cur")
                                          cur = i;
                                    else
                                          domError(nn);
                                    }
                              QString fn(urlGen(ped->getCurDir(), path));
                              int rv = file(fn, ii != 0, &pos);
                              if (rv == -1) {
                                    printf("Error loading file\n");
                                    return true;
                                    }
                              ++ii;
                              }
                        }
                  else if (tag == "cur") {
                        nn = i;
                        if (load_files)
                              kll.setcur_hist(i, cur);
                        }
                  else
                        domError(n);
                  }
            ++i;
            }
      if (load_files)
            kll.setcur(nn);
      return false;
      }

//---------------------------------------------------------
//   edit_cmd
//---------------------------------------------------------

void  Editor::edit_cmd(int cmd, QString s)
      {
      int rv = 0;
      if (cmd == CMD_NEW_ALTFIL) {
            QString url(urlGen(ped->getCurDir(), s));
            rv = file(url, false, 0);
            if (rv == 0)
                  open_kontext();
            return;
            }
	if (!(*kll)->edit(cmd, s)) {
            switch(cmd) {
               case CMD_NEXT_KONTEXT:
                  {
                  unsigned int i = kll.cur();
                  if (kll.size() > 1) {
                        i = (i == kll.size()-1) ? 0 : i+1;
                        gotoKontext(i);
                        }
                  }
                  break;
               case CMD_PREV_KONTEXT:
                  {
                  int i = kll.cur();
                  if (i == 0)
                        i = kll.size()-1;
                  else
                        i--;
                  if (i != kll.cur()) {
                        gotoKontext(i);
                        }
                  }
                  break;
               case CMD_REM_ALTFIL:
                  kll.delete_cur_context();
                  open_kontext();
                  break;
               case CMD_MAN:
                  {
                  char buffer[512];
                  const char* p = s.toLatin1().data();
                  snprintf(buffer, 512, "man:%s#%s", p, p);
                  file(buffer, true, 0);
                  }
                  open_kontext();
                  break;
               case CMD_STREAM:
                  {
                  char buffer[512];
                  snprintf(buffer, 512, "cmd:%s", s.toLatin1().data());
                  file(buffer, false, 0);
                  }
                  open_kontext();
                  break;
               case CMD_NEW_HIST:
                  {
                  QString url(urlGen(ped->getCurDir(), s));
                  file(url, true, 0);
                  (*kll)->open();
                  }
                  break;
               case CMD_VOR_HIST:
                  kll.vorhist();
                  open_kontext();
//                  (*kll)->open();
//                  (*kll)->expose(QRect(0, 0, width(), height()));
//                  fileBar->setText((*kll)->title());
//                  urlLabel->setText(QString((*kll)->url()));
//                  updateCurDir();
                  break;
               case CMD_BACK_HIST:
                  kll.backhist();
                  open_kontext();
//                  (*kll)->open();
//                  (*kll)->expose(QRect(0, 0, width(), height()));
//                  fileBar->setText((*kll)->title());
//                  urlLabel->setText(QString((*kll)->url()));
//                  updateCurDir();
                  break;
               default:
                  break;
               }
            }
      update();
      }

//---------------------------------------------------------
//   getpos
//---------------------------------------------------------

void Editor::getposX(int& col, int& line, int& xoff, int& yoff)
      {
      (*kll)->getpos(col, line, xoff, yoff);
      }

int Editor::addStack(QWidget* w)
      {
      stack->addWidget(w);
      return 0;
      }
void Editor::removeStack(QWidget* w)
      {
      stack->removeWidget(w);
      }
void Editor::raiseStack(int id)
      {
      stack->setCurrentIndex(id);
      }

//---------------------------------------------------------
//   setFont
//---------------------------------------------------------

void Editor::setEditorFont(const QFont& font)
      {
      win->setFont(font);
      }

/*---------------------------------------------------------
 *    file
 *    erzeugt Kontext fr URL
 *	flag = TRUE	 an Historie anhngen, ansonst neuer
 *                 Kontext
 *---------------------------------------------------------*/

int Editor::file(const QString& u, bool flag, Position* pos)
      {
      QFileInfo fi(u);

      File* file       = 0;

      struct Ftype {
            const char* const extension;
            FileType  type;
            Ftype(const char* const e, FileType t) : extension(e), type(t) {}
            };

      static struct Ftype ftypes[] = {
            Ftype("cc",  FILE_C),
            Ftype("cpp", FILE_C),
            Ftype("m",   FILE_C),
            Ftype("mm",  FILE_C),
            Ftype("c",   FILE_C),
            Ftype("C",   FILE_C),
            Ftype("h",   FILE_H),
            };

      QString suffix = fi.suffix();

      FileType ftype = FILE_UNKNOWN;
      for (unsigned i = 0; i < sizeof(ftypes)/sizeof(*ftypes); ++i) {
         	if (suffix == ftypes[i].extension) {
                  ftype = ftypes[i].type;
                  break;
                  }
            }

      //---------------------------------------------------
      // Checken, ob das File schon geladen ist:
      //    inode und device vergleichen
      //---------------------------------------------------

      if (fi.exists() && fi.isFile())
            file = files.find(fi);

      bool writable = true;
      if (file == 0) {
#if 0
            if (access(path, R_OK)) {
                  if (access(path.toLatin1(), W_OK)) {
                        ped->msg(2000, "cannot create <%s>: %s",
                           pathname, strerror(errno));
                        printf("create <%s> failed: %s\n",
                           pathname, strerror(errno));
                        return -1;
                        }
                  }
#endif
            file = new File(fi, ftype);
            files.push_back(file);
            }
      else
            writable = fi.isWritable();

      Kontext* kontext = new Kontext(file, this, ped, pos);

      if (flag) {
            kll.append_kontext(kontext);
            }
      else {
            KontextList* cl = new KontextList();
            cl->append(kontext);
            kll.append(cl);
            fb->addTab((*kll)->name());
            fb->setCurrentIndex(kll.cur());
            }
      file->setReadonly(!writable);
      return 0;
      }

//---------------------------------------------------------
//    goto_kontext
//---------------------------------------------------------

void Editor::gotoKontext(int k)
      {
      if (kll.cur() == k)
            return;
      kll.setcur(k);
      open_kontext();
      }

//---------------------------------------------------------
//    Win::goto_kontext
//---------------------------------------------------------

void Editor::goto_kontext(int k, int l)
      {
      kll.setcur(k, l);
      open_kontext();
      }

//---------------------------------------------------------
//   open_kontext
//---------------------------------------------------------

void Editor::open_kontext()
      {
      (*kll)->open();
      (*kll)->showKontext(true);
      fb->setCurrentIndex(kll.cur());
      fb->setTabText(kll.cur(), (*kll)->name());
      ped->urlLabel->setText((*kll)->path());

      (*kll)->register_update(UPDATE_ALL);
      (*kll)->update();
      update();
      updateCurDir();
      }

//---------------------------------------------------------
//   updateCurDir
//---------------------------------------------------------

void Editor::updateCurDir()
      {
      QFileInfo fi((*kll)->path());

      ped->cur_dir = fi.absolutePath();
      }

//---------------------------------------------------------
//   edit_char
//    subset der Editor-Schnittstelle; kein refresh
//---------------------------------------------------------

void Editor::edit(const QChar& c)
      {
   	if (kll.empty())
      	return;
      else
      	(*kll)->editChar(c);
      }

//---------------------------------------------------------
//   update
//---------------------------------------------------------

void Editor::update()
      {
   	if (!kll.empty()) {
            static bool redo = false;
            static bool undo = false;
            static bool vhist = false;
            static bool bhist = false;
            (*kll)->update();
            bool i = (*kll)->undo_ok();
            if (i != undo) {
                  ped->undoAction->setEnabled(i);
                  undo = i;
                  }
            i = (*kll)->redo_ok();
            if (i != undo) {
                  ped->redoAction->setEnabled(i);
                  redo = i;
                  }
            i = kll.backhist_ok();
            if (i != bhist) {
                  ped->histBAction->setEnabled(i);
                  bhist = i;
                  }
            i = kll.vorhist_ok();
            if (i != vhist) {
                  ped->histVAction->setEnabled(i);
                  vhist = i;
                  }
            }
      ped->set_line_column_var();
      }

//---------------------------------------------------------
//   updateHScrollbar
//---------------------------------------------------------

void Editor::updateHScrollbar()
      {
      int visible, total, pos;
      (*kll)->xscrollinfo(visible, total, pos);
      total--;
      if (total < 0)
            total = 0;
      if (total < visible)
            visible = total;
      if (o_xpos != pos) {
            if (pos > o_xtotal) {    // callbacks vermeiden
                  if (pos > total)
                        printf("HSCROLL unnormal\n");
                  hScroll->setRange(0, total);
                  o_xtotal = total;
                  }
            hScroll->setValue(pos);
            o_xpos = pos;
            }
      if (o_xtotal != total) {
            hScroll->setRange(0, total);
            o_xtotal = total;
            }
      if (o_xvisible != visible) {
            hScroll->setSingleStep(1);
            hScroll->setPageStep(visible);
            o_xvisible = visible;
            }
      }

//---------------------------------------------------------
//   updateVScrollbar
//    das Teil lt sich nicht atomar updaten sondern
//    erzeugt evtl. Callbacks zwischen den drei Aufrufen
//    wenn 'value' temporr auerhalb von 'range' liegt;
//    update() wird dadurch rekursiv aufgerufen, es
//    flickert und gibt andere interessante Nebeneffekte
//---------------------------------------------------------

static int prev_total;
void Editor::updateVScrollbar()
      {
      int visible, total, pos;
      (*kll)->yscrollinfo(visible, total, pos);
      --total;
      if (total < 0)
            total = 0;
      if (total < visible)
            visible = total;
      if (o_ypos != pos) {
            if (pos > o_ytotal) {    // callbacks vermeiden
                  vScroll->setRange(0, total);
                  prev_total = o_ytotal;
                  o_ytotal = total;
                  }
            vScroll->setValue(pos);
            o_ypos = pos;
            }
      if (o_ytotal != total) {
            vScroll->setRange(0, total);
            o_ytotal = total;
            }
      if (o_yvisible != visible) {
            vScroll->setSingleStep(1);
            vScroll->setPageStep(visible);
            o_yvisible = visible;
            }
      }

//---------------------------------------------------------
//   hScrollto
//---------------------------------------------------------

void Editor::hScrollTo(int val)
      {
      if ((*kll)->xscrollto(val)) {
            (*kll)->update();
            }
      }

//---------------------------------------------------------
//   vScrollTo
//---------------------------------------------------------

void Editor::vScrollTo(int val)
      {
      int visible, total, pos;
      (*kll)->yscrollinfo(visible, total, pos);
      if (val == pos)
            return;
      /* int n =*/ (*kll)->yscrollto(val);
      win->update();
      }


