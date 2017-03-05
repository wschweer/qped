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

#ifndef __PED_H__
#define __PED_H__

#include "line.h"
#include "file.h"
#include "editor.h"

const static int TABL = 6;

enum GraphicContext {
      GC_NORMAL,
      GC_CURSOR,
      GC_MARK,
      GC_KEYWORD,
      GC_KLAMMER,
      };

extern QDir* start_dir;       // current working directory on editor start
extern char* cur_dir;         // current working directory

extern bool isoLatin;
extern bool utf8;
extern QColor bgColor;
extern QColor fgColor;

#define UPDATE_ALL      0x01
#define UPDATE_LINE     0x02
#define UPDATE_MARK     0x04

//---------------------------------------------------------
//   Cmd
//---------------------------------------------------------

struct Cmd {
      int cmd;
      const char* param;

      Cmd(int c, const char* p) {
            cmd = c;
            param = p ? strdup(p) : 0;
            }
      ~Cmd() {
            if (param)
                  delete param;
            }
      };

typedef std::list<Cmd> CmdList;
typedef std::list<Cmd>::iterator iCmdList;

class EnterEdit;
class TreeTab;

//----------------------------------------------------------
//   Ped
//----------------------------------------------------------

class Ped : public QMainWindow
      {
      Q_OBJECT

      QString fontFamily  { "Bitstream Vera Sans Mono" };
      int fontWeight      { 50 };
      qreal fontSize      { 14 };

      int tree_width      { 150 };
      int last_cmd;
      Editor* editor1     { 0 };          // split1
      Editor* editor2     { 0 };          // split2
      QSplitter* splitter;
      QSplitter* tsplitter;
      TreeTab* tree       { 0 };

      QLabel* lineLabel;
      QLabel* colLabel;

      QDialog* aboutDialog;
      QDialog* commandsDialog;

      QToolBar *tools;

      QMenu* controls;
      QMenu* goPd;
      QMenu* helpPd;

      QAction* cc;
      QAction* pa;

      bool colorify, paren;

      bool enterActive   { false };

      QPixmap openIcon, saveIcon;
      QPixmap undoIcon, redoIcon;
      QPixmap histVIcon, histBIcon, searchVIcon, searchBIcon;
      QPixmap macroPIcon, macroRIcon, markzIcon, marksIcon;
      QPixmap makeIcon, runIcon, rebuildIcon, debugIcon;
      QPixmap splithIcon, splitvIcon, toggleTreeIcon;

      int update_flags;
      int recmode;
      CmdList cmd_list;

      int geometry_x;
      int geometry_y;
      int geometry_w;
      int geometry_h;

      QWidget* enter;
      EnterEdit* enterLine;
      double _fw;

      void genFileToolbar();
      void genPopupMenu();
      void readConfig();
      void updateCurDir();

   private slots:
      void load();
      void save();
      void saveAs();
      void saveAll();
      void quitDoc();

      void goMenue();
      void goSelect(QAction*);
      void helpCommands();
      void helpQt();
      void helpAbout();

      void toggleColorify();
      void toggleParen();

      void cmdEnterSearchF();
      void cmdEnterSearchR();
      void cmdEnterSearchK();
      void cmdEnterNewFile();
      void cmdEnterManual();
      void cmdEnterStream();
      void cmdEnterFunc();
      void cmdEnterGoto();
      void cmdEnterShell();

      void cmdShiftF3();

      void splitHorizontal();
      void splitVertical();
      void toggleTree();
      void editCmd(QAction*);

   public slots:
      void undo();
      void redo();
      void goto_kontext(int k);
      void leaveEnterInput(int code);
      void histv();
      void histb();
      void searchForward();
      void searchBack();
      void markColumns();
      void markLines();
      void playMacro();
      void recordMacro();
      void configFgColor();
      void configBgColor();
      void configFont();
      void saveConfig();
      void removeMsg();
      void edit_cmd(int cmd);
      void edit_cmd(int cmd, const char*);

   public:
      double fw() const { return _fw; }
      QFont eefont;
      int fh, fa, fd;   // font metrics
      QFrame* aboutItem     { 0 };
      QAction* undoAction;
      QAction* redoAction;
      QAction* histVAction;
      QAction* histBAction;
      QLabel* urlLabel;
      QDir cur_dir;
      Editor* cur_editor;           // current edit split

      Ped(int argc, char** argv);
      void open_kontext();
      QString getCurDir() const { return cur_dir.path(); }

      QLabel* insertLabel;
      QRegExp searchPattern;
      QString replace;

      void load(const QString&);
      bool loadStatus(bool);
      void saveStatus();
      int file(const QString&, bool flag);

      void set_line_column_var();
      void goto_kontext(int k, int l);
      void enterInput(QString s);
      void play_cmd();
      void rec_cmd(int cmd, const char* param);
      void edit_print(const QString&);
      void msg(int timeout, const QString&);
      void edit(const QChar&);
      void update();
      void closeEvent(QCloseEvent*e);
      bool getColorify() const { return colorify; }
      bool getParen() const { return paren; }
      void loadManPage(const char* url, bool flag);
      void expand_c_proc(const QString&);
      void expand_h_proc(const QString&);
      void setFont();
      };

#endif

