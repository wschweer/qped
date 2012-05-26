/***-------------------------------------------------------
 ***  PEd Editor
 ***        KDE - Version
 ***  $Id: tree.h 1.1 Fri, 24 Nov 2000 15:23:56 +0100 ws $
 ***
 ***  (C) - Copyright Werner Schweer 1999; ws@seh.de
 ***-------------------------------------------------------*/

#ifndef __TREE_H__
#define __TREE_H__

#if 0
extern QPixmap* folderIcon;
extern QPixmap* openFoldIcon;
extern QPixmap* fileIcon;
extern QPixmap* plusIcon;
extern QPixmap* minusIcon;

class Ped;

//---------------------------------------------------------
//   TreeTab
//---------------------------------------------------------

class TreeTab : public QTabWidget {
      Ped* ped;
      QLabel* label4;
      QWidget* w4;
      QListView* l1;
      QListView* l2;
      QListView* l3;
      QListView* l4;
      bool l1_init;
      bool l2_init;
      bool l3_init;
      bool l4_init;
      QPopupMenu* filePopup;
      int delete_id;
      int edit_id;

      void initTree3();
      void initTree4();

      Q_OBJECT
   private slots:
      void doubleClickFile(QListViewItem*);
      void doubleClickBm(QListViewItem*);
      void treeSelect(const QString&);
      void rightButtonPressed(QListViewItem*, const QPoint&, int);
      void rightButtonClicked(QListViewItem*, const QPoint&, int);
   public:
      TreeTab(QWidget*, Ped*);
      };

//---------------------------------------------------------
//   FItem
//---------------------------------------------------------

class FItem : public QListViewItem {
      FItem* p;
   protected:
      QFileInfo info;
   public:
      FItem(QListView*, const QFileInfo*);
      FItem(FItem*, const QFileInfo*);
      void setOpen(bool);
      QString fullName();
      bool isDir() const { return info.isDir(); }
      virtual void setup();
      virtual QString text(int col) const;
      virtual const QPixmap* pixmap(int col) const;
      virtual QString key(int, bool) const;
      };

class F2Item : public FItem {
   public:
      F2Item(QListView* v, const QFileInfo* i)
         : FItem(v, i) {}
      F2Item(FItem* v, const QFileInfo* i)
         : FItem(v, i) {}
      void setOpen(bool);
      };
#endif

#endif

