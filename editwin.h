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

      virtual void paintEvent(QPaintEvent* e) override;
      virtual void keyPressEvent(QKeyEvent* event) override;
      virtual void focusInEvent(QFocusEvent*e) override;
      virtual void wheelEvent(QWheelEvent*) override;

   protected:
      void mousePressEvent(QMouseEvent*) override;
      void mouseReleaseEvent(QMouseEvent*) override;
      void mouseMoveEvent(QMouseEvent*) override;
      void mouseDoubleClickEvent(QMouseEvent* e) override;

   public:
      int cy;
      EditWin(QWidget*, Ped*, Editor*);
      int zeilen() const;
      int spalten() const;
      int yoffset() const;
      int xoffset() const;
      void pos2xy(int x, int y, int* z, int* s) const;
      int baseline(int) const;
      };

#endif

