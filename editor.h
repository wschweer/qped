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

#ifndef __EDITOR_H__
#define __EDITOR_H__

#include "kontext.h"

class Ped;
class Xml;
class EditWin;

//---------------------------------------------------------
//   TabBar
//---------------------------------------------------------

class TabBar : public QTabBar {
      Q_OBJECT

      virtual void mouseDoubleClickEvent(QMouseEvent* ev) {
            emit doubleClick(currentIndex());
            QTabBar::mouseDoubleClickEvent(ev);
            }

   signals:
      void doubleClick(int);

   public:
      TabBar(QWidget* parent) : QTabBar(parent) {}
      };

//---------------------------------------------------------
//   Editor
//---------------------------------------------------------

class Editor : public QWidget {
      Q_OBJECT

      TabBar* fb;
      QWidget* eframe;
      QStackedWidget* stack;
      QGridLayout* grid;
      QScrollBar* hScroll;
      QScrollBar* vScroll;
      QSize size;
      Ped* ped;
      int editStackId;
      int o_xvisible, o_xtotal, o_xpos;
      int o_yvisible, o_ytotal, o_ypos;
      bool _startupMode { false };

   public slots:
      void edit_cmd(int, QString s = QString());
      void edit(const QChar&);
      void gotoKontext(int k);
      void hScrollTo(int val);
      void vScrollTo(int val);
      void removeKontext(int id);

   public:
      EditWin* win;
      KontextListList kll;

      Editor(QWidget* parent, Ped*);

      QSize getSize() { return QWidget::size(); };
      void repaint(int x, int y, int w, int h) {
            QWidget::repaint(x, y, w, h);
            }
      void leaveEnterInput(int, const QString&);
      void saveAs(const char* name);
      void saveStatus(Xml&, const char* name);
      bool loadStatus(QDomNode, bool);
      void update();
      void updateHScrollbar();
      void updateVScrollbar();
      void goto_kontext(int k, int l);
      void open_kontext();
      void getposX(int& col, int& line, int& xoff, int& yoff);
      void updateCurDir();
      int man(const QFile&, bool flag);
      void loadManPage(const char* url, bool flag);
      int file(const QString&, bool flag, Position*);
      int addStack(QWidget* w);
      void removeStack(QWidget* w);
      void raiseStack(int id);
      void setStartupMode(bool val) { _startupMode = val; }
      };

#endif

