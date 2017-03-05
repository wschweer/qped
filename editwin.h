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

#ifndef __editwin_h__
#define __editwin_h__

class Ped;
class Xml;
class Editor;

//----------------------------------------------------------
//   EditWin
//----------------------------------------------------------

class EditWin : public QFrame
      {
      Q_OBJECT
      int mouseButton;
      Ped* ped;
      Editor* editor;


      virtual void paintEvent(QPaintEvent* e);
      virtual bool event(QEvent* event);
      virtual void focusInEvent(QFocusEvent*e);
      virtual void wheelEvent(QWheelEvent*);

   protected:
      void mousePressEvent(QMouseEvent*);
      void mouseReleaseEvent(QMouseEvent*);
      void mouseMoveEvent(QMouseEvent*);
      void mouseDoubleClickEvent(QMouseEvent* e);

   public:
      qreal fw, fh, fa, fd;   // font metrics
      int cy;
      EditWin(QWidget*, Ped*, Editor*);
      int zeilen() const;
      int spalten() const;
      int yoffset() const;
      int xoffset() const;
      void pos2xy(int x, int y, int* z, int* s) const;
      int baseline(int) const;
      void fontChanged();
      };

#endif

