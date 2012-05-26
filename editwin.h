/***-------------------------------------------------------
 ***  PEd Editor
 ***        KDE - Version
 ***  $Id: editor.h 1.1 Fri, 24 Nov 2000 15:23:56 +0100 ws $
 ***
 ***  (C) - Copyright Werner Schweer 1999; ws@seh.de
 ***-------------------------------------------------------*/

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

      int fw, fh, fa;   // font metrics

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

