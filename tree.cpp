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
#include "tree.h"
#include "cmd.h"
#include "utils.h"

// #include "xpm/file.xpm"

// extern char* start_dir;

QPixmap* folderIcon;
QPixmap* openFoldIcon;
QPixmap* fileIcon;

//---------------------------------------------------------
//   toggleTree
//---------------------------------------------------------

void Ped::toggleTree()
      {
#if 0
      if (!tree) {
            tree = new TreeTab(tsplitter, this);
            int w = tsplitter->width();
            QValueList<int> vl;
            vl.append(tree_width);
            vl.append(w - tree_width);
            tsplitter->moveToFirst(tree);
            tsplitter->setSizes(vl);
            tree->show();
            return;
            }
      if (tree->isVisible()) {
            tree->hide();
            }
      else {
            tree->show();
            }
#endif
      }

#if 0
//---------------------------------------------------------
//   TreeTab
//---------------------------------------------------------

TreeTab::TreeTab(QWidget* parent, Ped* p)
   : QTabWidget(parent)
      {
      ped = p;
      l1 = new QListView(this);
      l2 = new QListView(this);
      l3 = new QListView(this);
      w4 = new QWidget(this);
      l1_init = false;
      l2_init = false;
      l3_init = false;
      l4_init = false;
      addTab(l1, "1");
      addTab(l2, "2");
      addTab(l3, "3");
      addTab(w4, "4");
      connect(this, SIGNAL(selected(const QString&)), SLOT(treeSelect(const QString&)));
      folderIcon   = new QPixmap(folder_xpm);
      openFoldIcon = new QPixmap(open_fold_xpm);
      fileIcon     = new QPixmap(file_xpm);
      filePopup = 0;
      }

//---------------------------------------------------------
//   doubleClickFile
//---------------------------------------------------------

void TreeTab::doubleClickFile(QListViewItem* vitem)
      {
      FItem* item = (FItem*)vitem;
      if (item->isDir())
            return;
      QString qs = item->fullName();
      ped->edit_cmd(CMD_NEW_ALTFIL, qs.ascii());
      }

//---------------------------------------------------------
//   rightButtonClicked
//---------------------------------------------------------

void TreeTab::rightButtonClicked(QListViewItem*, const QPoint&, int)
      {
      if (filePopup)
             filePopup->hide();
      }

//---------------------------------------------------------
//   rightButtonPressed
//---------------------------------------------------------

void TreeTab::rightButtonPressed(QListViewItem *vitem, const QPoint&p, int col)
      {
      if (vitem == 0 || col != 0)
            return;
      FItem* item = (FItem*)vitem;
      QString qs = item->fullName();
      if (filePopup == 0) {
            filePopup = new QPopupMenu(this);
            edit_id = filePopup->insertItem("edit");
            filePopup->insertSeparator();
            delete_id = filePopup->insertItem("delete");
            }
      int n = filePopup->exec(p);
      if (n == edit_id)
            ped->edit_cmd(CMD_NEW_ALTFIL, qs.ascii());
      else if (n == delete_id) {
            if (!item->isDir()) {
                  int rv = unlink(qs.ascii());
                  if (rv == -1) {
                        ped->msg(2000, "unlink <%s> failed: %s\n",
                           qs.ascii(), strerror(errno));
                        }
                  else
                        delete vitem;
                  }
            }
      }

//---------------------------------------------------------
//   treeSelect
//---------------------------------------------------------

void TreeTab::treeSelect(const QString& s)
      {
      if (s == "1") {
            if (l1_init == false) {
                  l1->addColumn("File");
                  l1->addColumn("Size");
                  l1->addColumn("Owner");
                  l1->addColumn("Group");
                  l1->setColumnAlignment(1, Qt::AlignRight);
                  l1->setColumnAlignment(2, Qt::AlignRight);
                  l1->setColumnAlignment(3, Qt::AlignRight);
                  l1->setTreeStepSize(15);
                  QFileInfo fi(start_dir);
                  F2Item* root = new F2Item(l1, &fi);
                  root->setOpen(true);
                  connect(l1, SIGNAL(doubleClicked(QListViewItem*)), this,
                     SLOT(doubleClickFile(QListViewItem*)));
                  connect(l1, SIGNAL(rightButtonPressed(QListViewItem*, const QPoint&, int)), this,
                     SLOT(rightButtonPressed(QListViewItem*, const QPoint&, int)));
                  l1_init = true;
                  }
            }
      else if (s == "2") {
            if (l2_init == false) {
                  l2->addColumn("File");
                  l2->addColumn("Size");
                  l2->addColumn("Owner");
                  l2->addColumn("Group");
                  l2->setColumnAlignment(1, Qt::AlignRight);
                  l2->setColumnAlignment(2, Qt::AlignRight);
                  l2->setColumnAlignment(3, Qt::AlignRight);
                  l2->setTreeStepSize(15);
                  QFileInfo fi(start_dir);
                  FItem* root = new FItem(l2, &fi);
                  root->setOpen(true);
                  connect(l2, SIGNAL(doubleClicked(QListViewItem*)), this,
                     SLOT(doubleClickFile(QListViewItem*)));
                  connect(l2, SIGNAL(rightButtonPressed(QListViewItem*, const QPoint&, int)), this,
                     SLOT(rightButtonPressed(QListViewItem*, const QPoint&, int)));
                  connect(l2, SIGNAL(rightButtonClicked(QListViewItem*, const QPoint&, int)), this,
                     SLOT(rightButtonPressed(QListViewItem*, const QPoint&, int)));
                  l2_init = true;
                  }
            }
      else if (s == "4") {
            if (l4_init == false) {
                  initTree4();
                  l4_init = true;
                  }
            }
      }

//---------------------------------------------------------
//   FItex
//---------------------------------------------------------

FItem::FItem(QListView* parent, const QFileInfo* i)
  : QListViewItem(parent), info(*i)
      {
      p = 0;
      }

FItem::FItem(FItem* parent, const QFileInfo* i)
  : QListViewItem(parent), info(*i)
      {
      p = parent;
      }

//---------------------------------------------------------
//   setOpen
//---------------------------------------------------------

void FItem::setOpen(bool o)
      {
      if (o && !childCount()) {
            QString s(fullName());
            QDir thisDir(s);
            if (!thisDir.isReadable()) {
                  return;
                  }
            thisDir.setFilter(QDir::All | QDir::Hidden);
            thisDir.setSorting(QDir::Name | QDir::DirsFirst);
            QFileInfoList files = thisDir.entryInfoList();
            for (int i = 0; i <files.size(); ++i) {
                  QFileInfo f = files.at(i);
                  if (f.fileName() != "." && f.fileName() != "..")
                        new FItem(this, &f);
                  }
            }
      QListViewItem::setOpen(o);
      }

//---------------------------------------------------------
//   setOpen
//---------------------------------------------------------

void F2Item::setOpen(bool o)
      {
      if (o && !childCount()) {
            QString s(fullName());
            QDir thisDir(s);
            if (!thisDir.isReadable()) {
                  return;
                  }
            thisDir.setFilter(QDir::All|QDir::Hidden);
            thisDir.setSorting(QDir::Name|QDir::DirsFirst);
            QFileInfoList files = thisDir.entryInfoList();
            for (int i = 0; i < files.size(); ++i) {
                  QFileInfo f = files.at(i);
                  //
                  //  filter:   ,*   .*   *.o
                  //
                  QString fn = f.fileName();
                  if (fn == "core")
                        continue;
                  const char* fname = fn.ascii();
                  if (*fname == '.')
                        continue;
                  if (*fname == ',')
                        continue;
                  const char* p = strrchr(fname, '.');
                  if (p) {
                        ++p;
                        if (strcmp(p, "o")==0)
                              continue;
                        }
                  new F2Item(this, &f);
                  }
            }
      QListViewItem::setOpen(o);
      }

void FItem::setup()
      {
      setExpandable(info.isDir());
      QListViewItem::setup();
      }

QString FItem::fullName()
      {
      QString s;
      if (p) {
            s = p->fullName();
            s.append("/");
            s.append(info.fileName());
            }
      else {
            s = start_dir;    // info.fileName();
            }
      return s;
      }

QString FItem::text(int col) const
      {
      QString s;
      switch (col) {
            case 0:
                  s = info.fileName();
                  break;
            case 1:
                  if (!info.isDir())
                        s.sprintf("%6llu", info.size());
                  break;
            case 2:
                  s = info.owner();
                  break;
            case 3:
                  s = info.group();
                  break;
            }
      return s;
      }
const QPixmap* FItem::pixmap(int col) const
      {
      if (col == 0)
            return info.isDir() ?
               (isOpen() ? openFoldIcon : folderIcon)
               : fileIcon;
      return 0;
      }
QString FItem::key(int col, bool) const
      {
      QString s;
      switch(col) {
            case 0:
                  s.sprintf("%d", !info.isDir());
                  s.append(info.fileName());
                  break;
            case 1:
                  s.sprintf("%08llu",
                     info.isDir() ? 0 : info.size());
                  break;
            case 2:
                  s = info.owner();
                  break;
            case 3:
                  s = info.group();
                  break;
            break;
            }
      return s;
      }

//---------------------------------------------------------
//   doubleClickBm
//---------------------------------------------------------

void TreeTab::doubleClickBm(QListViewItem* item)
      {
      QString qs = item->text(1);
      if (qs.isEmpty())
            return;
      ped->edit_cmd(CMD_NEW_ALTFIL, qs.ascii());
      }

//---------------------------------------------------------
//   initTree4
//    Bookmark List
//---------------------------------------------------------

void TreeTab::initTree4()
      {
      QGridLayout* g4 = new QGridLayout(w4, 1, 2);
      label4 = new QLabel("Bookmarks", w4);
      l4 = new QListView(w4);
      connect(l4, SIGNAL(doubleClicked(QListViewItem*)), this,
            SLOT(doubleClickBm(QListViewItem*)));
//      connect(l4, SIGNAL(rightButtonPressed(QListViewItem*, const QPoint&, int)), this,
//            SLOT(rightButtonPressed(QListViewItem*, const QPoint&, int)));

      l4->setRootIsDecorated(true);
      l4->addColumn("Document");
      l4->addColumn("Link");
      l4->setAllColumnsShowFocus(true);
      l4->setSorting(-1);
      QString qs(getenv("HOME"));
      qs.append('/');
      qs.append(".kped-bm");
      FILE* f = fopen(qs.ascii(), "r");
      if (f == 0) {
            ped->msg(2000, "open bookmarks <%s> failed: %s\n",
               qs.ascii(), strerror(errno));
            return;
            }
      std::stack<QListViewItem*> istack;
      int level = 0;
      while (!feof(f)) {
            char buffer[1024];
            int n = xgets(buffer, 1024, f);
            if (n < 0)
                  break;
            if (n == 0)
                  continue;
            char* p1 = buffer;
            while (isspace(*p1))
                  ++p1;
            int nlevel = p1 - buffer;
            if (nlevel > level) {
                  printf("What?\n");
                  nlevel = level;
                  }
            if (nlevel < level) {
                  while (!istack.empty() && nlevel < level) {
                        istack.pop();
                        --level;
                        }
                  level = nlevel;
                  }
            char* p2 = p1;
            ++p2;
            while(*p2 && !isspace(*p2))
                  ++p2;
            *p2 = 0;
            ++p2;
            while(isspace(*p2))
                  ++p2;
            QListViewItem* item;
            if (*p2) {
                  //
                  //  Link-Definition
                  //
                  if (level) {
                        item = new QListViewItem(istack.top(), p1, p2);
                        }
                  else {
                        item = new QListViewItem(l4, p1, p2);
                        }
                  item->setPixmap(0, file_xpm);
                  }
            else {
                  //
                  //  Folder-Definition
                  //
                  if (level) {
                        item = new QListViewItem(istack.top(), p1);
                        }
                  else {
                        item = new QListViewItem(l4, p1);
                        }
                  item->setPixmap(0, folder_xpm);
                  item->setExpandable(true);
                  istack.push(item);
                  level = nlevel+1;
                  }
            }
      fclose(f);
      g4->addWidget(label4, 0, 0);
      g4->addWidget(l4, 1, 0);

      g4->setRowStretch(0, 0);
      g4->setRowStretch(1, 500);
      g4->setColStretch(0, 500);

      label4->show();
      l4->show();
      g4->activate();
      }
#endif

