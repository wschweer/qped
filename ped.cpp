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

#include <cmath>
#include <unistd.h>

#include <QApplication>
#include <QDir>
#include <QMenu>
#include <QFileSystemModel>
#include <QFileDialog>
#include <QStatusBar>
#include <QToolBar>
#include <QToolButton>
#include <QShortcut>
#include <QCompleter>
#include <QActionGroup>
#include <QMenuBar>

#include "ped.h"
#include "utils.h"
#include "text.h"
#include "enter.h"
#include "cmd.h"
#include "xml.h"
#include "editwin.h"

#include "filesave.xpm"
#include "fileopen.xpm"

extern void cmd_table_init();
QDir* start_dir; // Verzeichnis, in dem der Editor gestartet wurde

bool isoLatin      = false;
bool utf8          = true;
bool debug_flag    = false;
bool changeInPlace = false;

static char* pedName;

//---------------------------------------------------------
//   printVersion
//---------------------------------------------------------

static void printVersion(const char* name)
      {
      printf("%s: Version 1.0\n", name);
      }

//---------------------------------------------------------
//   usage
//---------------------------------------------------------

static void usage(const char* reason)
      {
      if (reason)
            printf("%s: %s\n", pedName, reason);
      printf("usage: %s [options] file[s]\n", pedName);
      printf("options: -v   print version\n"
             "         -l   use iso latin1 codec\n"
             "         -u   use utf8 codec (default)\n"
             "         -i   change file 'in place'\n"
            );
      }

//---------------------------------------------------------
//   genFileToolbar
//---------------------------------------------------------

void Ped::genFileToolbar()
      {
      QToolBar* tools = new QToolBar("File Buttons");

      QAction* a = tools->addAction(openIcon, "Open File");
      connect(a, SIGNAL(triggered()), SLOT(load()));

      a = tools->addAction(saveIcon, "Save all Files");
      connect(a, SIGNAL(triggered()), SLOT(saveAll()));
      tools->addSeparator();

      undoAction = tools->addAction(undoIcon, "Undo");
      undoAction->setEnabled(false);
      connect(undoAction, SIGNAL(triggered()), SLOT(undo()));

      redoAction = tools->addAction(redoIcon, "Redo");
      redoAction->setEnabled(false);
      connect(redoAction, SIGNAL(triggered()), SLOT(redo()));

      histBAction = tools->addAction(histBIcon,"Back");
      histBAction->setEnabled(false);
      connect(histBAction, SIGNAL(triggered()), SLOT(histb()));

      histVAction = tools->addAction(histVIcon,"Forward");
      histVAction->setEnabled(false);
      connect(histVAction, SIGNAL(triggered()), SLOT(histv()));

      a = tools->addAction(searchVIcon, "Search forwards");
      connect(a, SIGNAL(triggered()), SLOT(searchForward()));

      a = tools->addAction(searchBIcon, "Search backwards");
      connect(a, SIGNAL(triggered()), SLOT(searchBack()));

      a = tools->addAction(markzIcon, "mark lines");
      connect(a, SIGNAL(triggered()), SLOT(markLines()));

      a = tools->addAction(marksIcon, "mark columns");
      connect(a, SIGNAL(triggered()), SLOT(markColumns()));

      a = tools->addAction(splithIcon, "split window horizontal");
      connect(a, SIGNAL(triggered()), SLOT(splitHorizontal()));

      a = tools->addAction(splitvIcon, "split window vertical");
      connect(a, SIGNAL(triggered()), SLOT(splitVertical()));

      tools->addSeparator();

      a = tools->addAction(macroPIcon, "play recorded macro");
      connect(a, SIGNAL(triggered()), SLOT(playMacro()));

      a = tools->addAction(macroRIcon, "record macro");
      connect(a, SIGNAL(triggered()), SLOT(recordMacro()));

      tools->addSeparator();
      }

//---------------------------------------------------------
//   genPopupMenu
//---------------------------------------------------------

void Ped::genPopupMenu()
      {
      QMenuBar* mb = new QMenuBar(this);
      setMenuBar(mb);

      //---------------------------------------------------
      //    generate File Pulldown
      //---------------------------------------------------

      QMenu* file = new QMenu("File");
      mb->addMenu(file);
      file->addAction(openIcon, "Open", this, SLOT(load()));
      file->addAction(saveIcon, "Save", this, SLOT(save()));
      file->addAction(saveIcon, "Save All", this, SLOT(saveAll()));
      file->addAction(saveIcon, "Save As", this, SLOT(saveAs()));
      file->addSeparator();
      file->addAction("Save Exit",    this, SLOT(close()));
      file->addAction("Exit no save", this, SLOT(quitDoc()));

      //---------------------------------------------------
      //    generate Config Pulldown
      //---------------------------------------------------

      controls = new QMenu("Config");
      mb->addMenu(controls);
      cc = controls->addAction("Colorify", this, SLOT(toggleColorify()));
      cc->setCheckable(true);
      pa = controls->addAction("Paren", this, SLOT(toggleParen()));
      pa->setCheckable(true);

      controls->addSeparator();
      controls->addAction("Foreground Color", this, SLOT(configFgColor()));
      controls->addAction("Background Color", this, SLOT(configBgColor()));
      controls->addAction("Font", this, SLOT(configFont()));
      controls->addSeparator();
      controls->addAction("Save Configuration", this, SLOT(saveConfig()));
      }

//---------------------------------------------------------
//   ActionList
//---------------------------------------------------------

struct ActionList {
      int cmd;
      const QKeySequence shortcut;
      };

#ifdef Q_WS_MAC
#define CONTROL   Qt::MetaModifier
#else
#define CONTROL   Qt::ControlModifier
#endif

static const ActionList editActions[] = {
      { CMD_SAVE_EXIT,    Qt::Key_F1 },
      { CMD_QUIT,         Qt::Key_F1 | Qt::SHIFT },
      { CMD_BACK_HIST,    Qt::Key_F2 },
      { CMD_VOR_HIST,     Qt::Key_F2 | Qt::SHIFT },
      { CMD_VIEW_TOGGLE,  Qt::Key_V | CONTROL },
      { CMD_NEXT_KONTEXT, Qt::Key_F3 },
      { CMD_TAB,          Qt::Key_Backtab },
      { CMD_END_WINDOW,   Qt::Key_End | Qt::SHIFT },
      { CMD_END_FILE,     Qt::Key_End | CONTROL },
      { CMD_END_LINE,     Qt::Key_End },
      { CMD_NEXT,         Qt::Key_Right },
      { CMD_BACK,         Qt::Key_Left  },
      { CMD_PAGE_UP,      Qt::Key_PageUp },
      { CMD_PAGE_DOWN,    Qt::Key_PageDown },
      { CMD_VIEW_TOGGLE,  Qt::Key_Up | CONTROL },
      { CMD_VIEW_TOGGLE,  Qt::Key_Down | CONTROL },
      { CMD_START_LINE,   Qt::Key_Home },
      { CMD_START_FILE,   Qt::Key_Home | CONTROL },
      { CMD_START_WINDOW, Qt::Key_Home | Qt::SHIFT},
      { CMD_UNDO,         Qt::Key_F4 },
      { CMD_REDO,         Qt::Key_F4 | Qt::SHIFT},
      { CMD_LINE_MARK,    Qt::Key_F5 },
      { CMD_COLUMN_MARK,  Qt::Key_F6 },
      { CMD_SEARCH_F,     Qt::Key_F7 },
      { CMD_SEARCH_R,     Qt::Key_F7 | Qt::SHIFT},
      { CMD_PICK,         Qt::Key_F8 },
      { CMD_PUT,          Qt::Key_F9 },
      { CMD_UNDELETE,     Qt::Key_F9 | CONTROL },
      { CMD_UNDELETE,     Qt::Key_F9 | Qt::SHIFT},
      { CMD_PLAY,         Qt::Key_F10 },
      { CMD_RECORD,       Qt::Key_F10 | Qt::SHIFT},
      { CMD_RUBOUT,       Qt::Key_Delete },

      { CMD_START_LINE,   QKeySequence(Qt::Key_Q | CONTROL, Qt::Key_S | CONTROL) },
      { CMD_END_LINE,     QKeySequence(Qt::Key_Q | CONTROL, Qt::Key_D | CONTROL) },
      { CMD_START_FILE,   QKeySequence(Qt::Key_Q | CONTROL, Qt::Key_R | CONTROL) },
      { CMD_END_FILE,     QKeySequence(Qt::Key_Q | CONTROL, Qt::Key_C | CONTROL) },
      { CMD_START_WINDOW, QKeySequence(Qt::Key_Q | CONTROL, Qt::Key_E | CONTROL) },
      { CMD_END_WINDOW,   QKeySequence(Qt::Key_Q | CONTROL, Qt::Key_X | CONTROL) },
      { CMD_DEL_RESTLINE, QKeySequence(Qt::Key_Q | CONTROL, Qt::Key_Y | CONTROL) },

      { CMD_CKLAMMER,     QKeySequence(Qt::Key_K | CONTROL, Qt::Key_K | CONTROL) },
      { CMD_SAVE_ALL,     QKeySequence(Qt::Key_K | CONTROL, Qt::Key_A | CONTROL) },
      { CMD_CKOMPOUND,    QKeySequence(Qt::Key_K | CONTROL, Qt::Key_L | CONTROL) },
      { CMD_SAVE_EXIT,    QKeySequence(Qt::Key_K | CONTROL, Qt::Key_X | CONTROL) },
      { CMD_QUIT,         QKeySequence(Qt::Key_K | CONTROL, Qt::Key_Q | CONTROL) },
      { CMD_REDO,         QKeySequence(Qt::Key_K | CONTROL, Qt::Key_U | CONTROL) },

      { CMD_PAGE_UP,      Qt::Key_R | CONTROL },
      { CMD_BACK,         Qt::Key_S | CONTROL },
      { CMD_DEL_WORD,     Qt::Key_T | CONTROL },
      { CMD_UNDO,         Qt::Key_U | CONTROL },
      { CMD_SCROLL_UP,    Qt::Key_W | CONTROL },
      { CMD_DOWN,         Qt::Key_X | CONTROL },
      { CMD_DEL_LINE,     Qt::Key_Y | CONTROL },
      { CMD_SCROLL_DOWN,  Qt::Key_Z | CONTROL },
      { CMD_BACK_WORD,    Qt::Key_A | CONTROL },
      { CMD_PAGE_DOWN,    Qt::Key_C | CONTROL },
      { CMD_NEXT,         Qt::Key_D | CONTROL },
      { CMD_UP,           Qt::Key_E | CONTROL },
      { CMD_NEXT_WORD,    Qt::Key_F | CONTROL },
      { CMD_DEL_CHAR,     Qt::Key_G | CONTROL },
      { CMD_INSERT_LINE,  Qt::Key_N | CONTROL },
      { CMD_GET_WORD,     QKeySequence(Qt::Key_O | CONTROL, Qt::Key_W | CONTROL) },
      { CMD_LINK,         QKeySequence(Qt::Key_O | CONTROL, Qt::Key_L | CONTROL) },
      { CMD_RUBOUT,       Qt::Key_Backspace },
      { CMD_NEWLINE,      Qt::Key_Return },
      { CMD_ENTER_INPUT,  Qt::Key_Escape },

      { CMD_TOUPPER,       QKeySequence(Qt::Key_P | CONTROL, Qt::Key_U | CONTROL) },
      };

//---------------------------------------------------------
//   Ped
//---------------------------------------------------------

Ped::Ped(int argc, char** argv)
   : QMainWindow()
      {
      QActionGroup* ag = new QActionGroup(this);
      for (const ActionList& al : editActions) {
            QAction* a = new QAction(ag);
            a->setData(al.cmd);
            a->setShortcut(al.shortcut);
            a->setShortcutContext(Qt::ApplicationShortcut);
            }
      addActions(ag->actions());
      upAction = new QAction(ag);
      upAction->setData(CMD_UP);
      upAction->setShortcut(Qt::Key_Up);
      upAction->setShortcutContext(Qt::ApplicationShortcut);
      addAction(upAction);

      downAction = new QAction(ag);
      downAction->setData(CMD_DOWN);
      downAction->setShortcut(Qt::Key_Down);
      downAction->setShortcutContext(Qt::ApplicationShortcut);
      addAction(downAction);

      connect(ag, SIGNAL(triggered(QAction*)), SLOT(editCmd(QAction*)));

      new QShortcut(Qt::Key_F3 | Qt::SHIFT, this, SLOT(cmdShiftF3()));

      colorify       = false;
      paren          = true;
      aboutDialog    = 0;
      commandsDialog = 0;
      recmode        = false;
      openIcon       = QPixmap(fileopen);
      saveIcon       = QPixmap(filesave);

      readConfig();
      setFont();

      genFileToolbar();
      genPopupMenu();

      //
      //    Central Widget:
      //
      //  +-box-------------------------+
      //  | +-delim1------------------+ |
      //  | +-----+ +-editor-+-editor-+ |
      //  | |     | |        |        | |
      //  | |Tree | |        |        | |
      //  | |     | |        |        | |
      //  | +-----+ +--------|--------+ |
      //  | +-------------------------+ |
      //  | |    enter                | |
      //  | +-----------------+--+--+-+ |
      //  | |status           |  |  | | |
      //  | +-----------------+--+--+-+ |
      //  +-----------------------------+

      QWidget* box      = new QWidget;
      QVBoxLayout* grid = new QVBoxLayout;
      box->setLayout(grid);
      grid->setContentsMargins(0, 0, 0, 0);
      grid->setSpacing(0);

      //-------------------------------
      tsplitter = new QSplitter(box);
      splitter  = new QSplitter(tsplitter);
      splitter->setOpaqueResize(true);
      editor1   = new Editor(splitter, this);
      editor2   = 0;
      cur_editor = editor1;
      grid->addWidget(tsplitter, 200);
      //---------------------------------

      enter = new QWidget;
      grid->addWidget(enter, 0);

      QBoxLayout* layout1 = new QBoxLayout(QBoxLayout::LeftToRight, enter);
      layout1->setContentsMargins(0, 0, 0, 0);
      layout1->setSpacing(0);

      QLabel* enterLabel = new QLabel("Enter:", enter);
      layout1->addWidget(enterLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);

      enterLine = new HistoryLineEdit(enter);
      QStringList sl;
      sl << "mops";
      sl << "mopschen";
      sl << "klops";
      QCompleter* completer = new QCompleter(sl, this);

      QStringList nameFilters;
      nameFilters << "*";
      QDir::Filters filters = QDir::Files;
      QFileSystemModel* fm = new QFileSystemModel(completer);
      fm->setFilter(filters);
//      fm->setRootPath("/home2/ws/qped");
//      completer->setModel(fm);
//      completer->setCompletionMode(QCompleter::InlineCompletion);
      enterLine->setWordCompleter(completer);

      layout1->addWidget(enterLine, 50, Qt::AlignVCenter);

      QToolButton* enterCmdButton = new QToolButton(enter);
      enterCmdButton->setText("Cmd");
      QMenu* enterPopup = new QMenu(enter);
      enterPopup->addAction("Search F", this, SLOT(cmdEnterSearchF()));
      enterPopup->addAction("Search R", this, SLOT(cmdEnterSearchR()));
      enterPopup->addAction("Search K", this, SLOT(cmdEnterSearchK()));
      enterPopup->addSeparator();
      enterPopup->addAction("new File", this, SLOT(cmdEnterNewFile()));
      enterPopup->addAction("Manual", this, SLOT(cmdEnterManual()));
      enterPopup->addAction("StreamIn", this, SLOT(cmdEnterStream()));
      enterPopup->addSeparator();
      enterPopup->addAction("gen Func", this, SLOT(cmdEnterFunc()));
      enterPopup->addAction("Goto", this, SLOT(cmdEnterGoto()));
      enterPopup->addAction("Shell", this, SLOT(cmdEnterShell()));

      enterCmdButton->setMenu(enterPopup);
      enterCmdButton->setPopupMode(QToolButton::InstantPopup);

      layout1->addWidget(enterCmdButton, 1, Qt::AlignTop | Qt::AlignRight);
      enter->hide();

      QStatusBar* sb = statusBar();

      urlLabel = new QLabel(sb);
      urlLabel->setText("url");
      urlLabel->setToolTip("url current doc");
      sb->addWidget(urlLabel, 100);

      insertLabel = new QLabel;
      insertLabel->setText(" I ");
      insertLabel->setToolTip("Insert/Overwrite Flag");
      sb->addPermanentWidget(insertLabel, 0);

      lineLabel = new QLabel;
      lineLabel->setToolTip("current line");
      sb->addPermanentWidget(lineLabel, 0);

      colLabel  = new QLabel;
      colLabel->setToolTip("current column");
      sb->addPermanentWidget(colLabel, 0);
      //---------------------------------

      setCentralWidget(box);

      cur_dir = *start_dir;
      bool load_last_files = argc == 0;

      cur_editor->setStartupMode(true);
      for (int i = 0; i < argc; i++)
            edit_cmd(CMD_NEW_ALTFIL, QString(argv[i]));
      cur_editor->setStartupMode(false);

      geometry_x = 0;
      geometry_y = 0;
      geometry_w = 1200;
      geometry_h = 1410;

      if (!loadStatus(load_last_files)) {
            if (load_last_files) {
                  usage(0);
                  exit(1);
                  }
            setGeometry(geometry_x, geometry_y, geometry_w, geometry_h);
            }
      //
      //  endgültige Konfiguration einstellen
      //
      cc->setChecked(colorify);
      pa->setChecked(paren);

      editor1->open_kontext();
      if (editor2)
            editor2->open_kontext();
      cur_editor->win->setFocus();
      }

//---------------------------------------------------------
//   setFont
//---------------------------------------------------------

void Ped::setFont()
      {
      eefont = QFont(fontFamily);
      eefont.setPointSizeF(fontSize);
      eefont.setWeight(QFont::Weight(fontWeight));
      eefont.setFixedPitch(true);
      QFontMetricsF fm(eefont);
      _fw = fm.averageCharWidth();
      fh = lrint(fm.height());
      fa = lrint(fm.ascent());
      fd = lrint(fm.descent());
      }

//---------------------------------------------------------
//   enterInput
//---------------------------------------------------------

void Ped::enterInput(QString s)
      {
      enterActive = true;
      enterLine->setText(s);
      enter->show();
      enterLine->setFocus();
      removeAction(upAction);
      removeAction(downAction);
      }

//---------------------------------------------------------
//   removeMsg
//---------------------------------------------------------

void Ped::removeMsg()
      {
      if (recmode)
            statusBar()->showMessage("Recording...");
      else
            statusBar()->clearMessage();
      }

/*---------------------------------------------------------
 *    msg
 *---------------------------------------------------------*/

void Ped::msg(int timeout, const QString& s)
      {
      statusBar()->showMessage(s, timeout);
      }

static const QString ct("//---------------------------------------------------------");

//---------------------------------------------------------
//   expand_c_proc
//---------------------------------------------------------

void Ped::expand_c_proc(const QString& txt)
      {
      edit_print(ct);
      edit_print("\n//   ");
      edit_print(txt);
      edit_cmd(CMD_NEWLINE);
      edit_print(ct);
      if (txt == "main") {
            QString s("\n\nint %1(int argc, char* argv[])\n");
            edit_print(s.arg(txt));
            edit_print("      {\n\n      return 0;\n      }\n");
            edit_cmd(CMD_UP);
            }
      else {
            edit_cmd(CMD_NEWLINE);
            edit_cmd(CMD_NEWLINE);
            QString s;
            if (txt.contains(' '))
                  s = QString("%1()");
            else
                  s = QString("void %1()");
            edit_print(s.arg(txt));
            edit_cmd(CMD_NEWLINE);
            edit_print("      {");
            edit_cmd(CMD_NEWLINE);
            edit_cmd(CMD_NEWLINE);
            edit_print("}");
            edit_cmd(CMD_NEWLINE);
            }
      edit_cmd(CMD_UP);
      edit_cmd(CMD_UP);
      edit_print("      ");
      update();
      }

//---------------------------------------------------------
//   expand_h_proc
//---------------------------------------------------------

void Ped::expand_h_proc(const QString& txt)
      {
      edit_cmd(CMD_START_LINE);
      edit_cmd(CMD_NEWLINE);
      QString s("%1\n//   %2\n%3");
      edit_print(s.arg(ct).arg(txt).arg(ct));
      edit_cmd(CMD_NEWLINE);
      edit_cmd(CMD_NEWLINE);
      update();
      }

//---------------------------------------------------------
//   leaveEnterInput
//---------------------------------------------------------

void Ped::leaveEnterInput(int code)
      {
      if (enterActive) {
            enterActive = false;
            cur_editor->leaveEnterInput(code, enterLine->text());
            enterLine->execute();
            enter->hide();
            addAction(upAction);
            addAction(downAction);
            }
      }

//---------------------------------------------------------
//   load
//---------------------------------------------------------

void Ped::load()
      {
      QString fn = QFileDialog::getOpenFileName(this, "Ped");
      if ( !fn.isEmpty() )
            load( fn );
      }

//---------------------------------------------------------
//   save
//---------------------------------------------------------

void Ped::save()
      {
      edit_cmd(CMD_SAVE);
      msg(5000, QString("saving %1").arg((*(cur_editor->kll))->name()));
      }

//---------------------------------------------------------
//   saveAs
//---------------------------------------------------------

void Ped::saveAs()
      {
      QString fn = QFileDialog::getSaveFileName(this, "Ped", cur_dir.path());
      if (!fn.isEmpty()) {
            cur_editor->saveAs(fn.toLatin1().data());
            }
      }

//---------------------------------------------------------
//   toggleColorify
//---------------------------------------------------------

void Ped::toggleColorify()
      {
      colorify = !colorify;
      cc->setChecked(colorify);
      (*(cur_editor->kll))->register_update(UPDATE_ALL);
      cur_editor->update();
      }

void Ped::toggleParen()
      {
      paren = !paren;
      pa->setChecked(paren);
      (*(cur_editor->kll))->register_update(UPDATE_ALL);
      cur_editor->update();
      }

//---------------------------------------------------------
//   goMenue
//---------------------------------------------------------

void Ped::goMenue()
      {
      goPd->clear();
      }

//---------------------------------------------------------
//   goSelect
//    wird beim Anclicken eines Select-Pulldown-Items
//    aufgerufen
//---------------------------------------------------------

void Ped::goSelect(QAction*)
      {
// TODO      cur_editor->goto_kontext(cur_editor->kll.cur(), idx);
      }

void Ped::quitDoc()
      {
      last_cmd = CMD_QUIT;
      close();
      }

//---------------------------------------------------------
//   closeEvent
//---------------------------------------------------------

void Ped::closeEvent(QCloseEvent*)
      {
      saveStatus();
      if (last_cmd != CMD_QUIT) {
            files.saveall();
            }
      exit(0);
      }

//---------------------------------------------------------
//   editCmd
//---------------------------------------------------------

void Ped::editCmd(QAction* a)
      {
      edit_cmd(a->data().toInt());
      }

// bindings for enter popup:

void Ped::cmdEnterSearchF() { leaveEnterInput(CMD_SEARCH_F); }
void Ped::cmdEnterSearchR() { leaveEnterInput(CMD_SEARCH_R); }
void Ped::cmdEnterSearchK() { leaveEnterInput(CMD_ENTER_KOLLAPS); }
void Ped::cmdEnterNewFile() { leaveEnterInput(CMD_NEW_ALTFIL); }
void Ped::cmdEnterManual()  { leaveEnterInput(CMD_MAN); }
void Ped::cmdEnterStream()  { leaveEnterInput(CMD_STREAM); }
void Ped::cmdEnterFunc()    { leaveEnterInput(CMD_FUNCTION); }
void Ped::cmdEnterGoto()    { leaveEnterInput(CMD_GOTO); }
void Ped::cmdEnterShell()   { leaveEnterInput(CMD_SHELL); }

void Ped::undo()          { edit_cmd(CMD_UNDO); }
void Ped::redo()          { edit_cmd(CMD_REDO); }
void Ped::histv()         { edit_cmd(CMD_VOR_HIST); }
void Ped::histb()         { edit_cmd(CMD_BACK_HIST); }
void Ped::searchForward() { edit_cmd(CMD_SEARCH_F); }
void Ped::searchBack()    { edit_cmd(CMD_SEARCH_R); }
void Ped::markColumns()   { edit_cmd(CMD_COLUMN_MARK); }
void Ped::markLines()     { edit_cmd(CMD_LINE_MARK); }
void Ped::playMacro()     { edit_cmd(CMD_PLAY); }
void Ped::recordMacro()   { edit_cmd(CMD_RECORD); }
void Ped::saveAll()       { edit_cmd(CMD_SAVE_ALL); }

//---------------------------------------------------------
//   load
//---------------------------------------------------------

void Ped::load(const QString& name)
      {
      edit_cmd(CMD_NEW_ALTFIL, name);
      }

//---------------------------------------------------------
//   cmdShiftF3
//---------------------------------------------------------

void Ped::cmdShiftF3()
      {
      if (enterActive)
            ;
      else
            edit_cmd(CMD_PREV_KONTEXT);
      }

//---------------------------------------------------------
//   splitVertical
//---------------------------------------------------------

void Ped::splitVertical()
      {
      int orientation = splitter->orientation();
      splitter->setOrientation(Qt::Horizontal);
      if (!editor2) {
            editor2  = new Editor(splitter, this);
            cur_editor = editor2;
            load((*(editor1->kll))->path());
            editor2->show();
            }
      else if (orientation == Qt::Horizontal) {
            files.saveall();
            delete editor2;
            editor2 = 0;
            cur_editor = editor1;
            cur_editor->open_kontext();
            }
      }

//---------------------------------------------------------
//   splitHorizontal
//---------------------------------------------------------

void Ped::splitHorizontal()
      {
      int orientation = splitter->orientation();
      splitter->setOrientation(Qt::Vertical);
      if (editor2 == 0) {
            editor2  = new Editor(splitter, this);
            cur_editor = editor2;
            load((*(editor1->kll))->path());
            editor2->show();
            }
      else if (orientation == Qt::Vertical) {
            files.saveall();
            delete editor2;
            editor2 = 0;
            cur_editor = editor1;
            cur_editor->open_kontext();
            }
      }

//---------------------------------------------------------
//   saveStatus
//---------------------------------------------------------

void Ped::saveStatus()
      {
      QFile f(".qped");
      if (!f.open(QIODevice::WriteOnly))
            return;

      Xml xml(&f);
      xml.header();
      xml.tag("Ped version=\"1.0\"");
      xml.tag("Status");

      xml.geometryTag("geometry", this);

      editor1->saveStatus(xml, "Editor1");
      if (editor2) {
            editor2->saveStatus(xml, "Editor2");
            xml.intTag("CurEditor", cur_editor == editor2);
            }

      //----------------------------------------------
      //    save Filelist
      //----------------------------------------------

      if (editor2) {
            if (splitter->orientation() == Qt::Horizontal)
                  xml.tagE("hsplit");
            else
                  xml.tagE("vsplit");

            QList<int> tvl = splitter->sizes();
            int n = tvl.count();
            for (int i = 0; i < n; ++i) {
                  int val = tvl.at(i);
                  xml.intTag("split", val);
                  }
            }

      //----------------------------------------------
      //    save diverse Variablen
      //----------------------------------------------

      xml.strTag("search", searchPattern.pattern());
      xml.strTag("replace", replace);
      xml.intTag("colorify", colorify);
      xml.intTag("brackets", paren);

      xml.etag("Status");
      xml.etag("Ped");
      f.close();
      }

//---------------------------------------------------------
//   loadStatus
//    liefert true, wenn Statusfile gelesen wurde
//---------------------------------------------------------

bool Ped::loadStatus(bool load_files)
      {
      FILE* f = fopen(".qped", "r");
      if (f == 0)
            return false;
      QFile qf(".qped");
      qf.open(f, QIODevice::ReadOnly);

      QDomDocument doc;
      int line, column;
      QString err;

      if (!doc.setContent(&qf, false, &err, &line, &column)) {
            fprintf(stderr, "error reading file .qped at line %d column %d: %s\n",
               line, column, err.toLatin1().data());
            return false;
            }

      QList<int> tvl;
      bool hsplit = true;

      for (QDomElement e = doc.documentElement(); !e.isNull(); e = e.nextSiblingElement()) {
            if (e.tagName() != "Ped") {
                  domError(e);
                  continue;
                  }
            for (QDomElement ee = e.firstChildElement(); !ee.isNull(); ee = ee.nextSiblingElement()) {
                  if (ee.tagName() != "Status") {
                        domError(ee);
                        continue;
                        }
                  for (QDomElement eee = ee.firstChildElement(); !eee.isNull(); eee = eee.nextSiblingElement()) {
                        QString tag(eee.tagName());
                        QString val(eee.text());
                        int i = val.toInt();
                        if (tag == "search")
                              searchPattern.setPattern(val);
                        else if (tag == "replace")
                              replace = val;
                        else if (tag == "colorify")
                              colorify = i;
                        else if (tag == "brackets")
                              paren = i;
                        else if (tag == "geometry") {
                              QRect r = readGeometry(eee);
                              setGeometry(r.x(), r.y(), r.width(), r.height());
                              }
                        else if (tag == "treeVisible")
                              ;
                        else if (tag == "treeWidth")
                              ;
                        else if (tag == "Editor1")
                              editor1->loadStatus(eee, load_files);
                        else if (tag == "Editor2") {
                              editor2 = new Editor(splitter, this);
                              editor2->loadStatus(eee, load_files);
                              }
                        else if (tag == "CurEditor")
                              cur_editor = i ? editor2 : editor1;
                        else if (tag == "split")
                              tvl.append(i);
                        else if (tag == "hsplit")
                              hsplit = true;
                        else if (tag == "vsplit")
                              hsplit = false;
                        else
                              domError(eee);
                        }
                  }
            }
      int n = tvl.count();
      if (n > 1) {
            splitter->setOrientation(hsplit ? Qt::Horizontal : Qt::Vertical);
            splitter->setSizes(tvl);
            tsplitter->show();
            }
      return true;
      }

/*---------------------------------------------------------
 *    set_line_column_var
 *---------------------------------------------------------*/

void Ped::set_line_column_var()
      {
      static int old_column = -1;
      static int old_line  = -1;
      int dummy1, dummy2;

      int column, line;
      cur_editor->getposX(column, line, dummy1, dummy2);
      QString s;
      if (column != old_column) {
            colLabel->setText(QString(" %1 ").arg(column + 1));
            old_column = column;
            }
      if (line != old_line) {
            old_line = line;
            lineLabel->setText(QString(" %1 ").arg(line+1));
            }
      }

//---------------------------------------------------------
//   play_cmd
//---------------------------------------------------------

void Ped::play_cmd()
      {
      for (auto i = cmd_list.begin(); i != cmd_list.end(); ++i)
            edit_cmd(i->cmd, i->param);
      }

//---------------------------------------------------------
//   rec_cmd
//---------------------------------------------------------

void Ped::rec_cmd(int cmd, QString param)
      {
      if (cmd == CMD_RECORD || cmd == CMD_PLAY)
            return;
      cmd_list.push_back(Cmd(cmd, param));
      }

/*---------------------------------------------------------
 *    edit_print
 *---------------------------------------------------------*/

void Ped::edit_print(const QString& s)
      {
      Kontext* k = *(cur_editor->kll);

      bool savedAI = k->auto_indent;
      k->auto_indent = false;
      for (int i = 0; i < s.size(); ++i) {
            QChar c = s[i];
            if (c == QLatin1Char('\n'))
                  edit_cmd(CMD_NEWLINE);
            else
                  edit(c);
            }
      k->auto_indent = savedAI;
      }

//---------------------------------------------------------
//   edit_cmd
//---------------------------------------------------------

void  Ped::edit_cmd(int cmd, const QString param)
      {
      if (recmode)
            rec_cmd(cmd, param);
      if (cmd >= CMD_MAXCODE) {        // ignore unknown commands
            qCritical("unknown cmd %x", cmd);
            return;
            }
      if (enterActive) {
            switch(cmd) {
                  case CMD_ENTER_INPUT:
                        leaveEnterInput(-1);
                        break;
                  case CMD_SEARCH_F:
                        leaveEnterInput(CMD_SEARCH_F);
                        break;
                  case CMD_NEXT_KONTEXT:
                        leaveEnterInput(CMD_NEW_ALTFIL);
                        break;
                  case CMD_DEL_CHAR:
                        leaveEnterInput(CMD_GOTO);
                        break;
                  case CMD_RUBOUT:
                        if (enterLine->cursorPosition()) {
                              enterLine->cursorBackward(false, 1);
                              enterLine->del();
                              }
                        break;
                  case CMD_BACK:
                        enterLine->cursorBackward(false, 1);
                        break;
                  case CMD_NEXT:
                        enterLine->cursorForward(false, 1);
                        break;
                  case CMD_BACK_WORD:
                        enterLine->cursorWordBackward(false);
                        break;
                  case CMD_NEXT_WORD:
                        // enterLine->cursorWordForward(false);
                        leaveEnterInput(CMD_FUNCTION);
                        break;
                  case CMD_UP: {
//                        QString s(enterLine->enter_up());
//                        if (!s.isEmpty())
//                              enterLine->setText(s);
                        }
                        break;
                  case CMD_DOWN: {
//                        QString s(enterLine->enter_down());
//                        if (!s.isEmpty())
//                              enterLine->setText(s);
                        }
                        break;
                  }
            return;
            }
      switch (cmd) {
            case CMD_ENTER_INPUT:
                  enterInput("");
                  return;
            case CMD_GET_WORD:
                  enterInput((*(cur_editor->kll))->get_word(0));
                  return;

            case CMD_INSERT_LINE:
                  cur_editor->edit_cmd(CMD_END_LINE, 0);
                  cur_editor->edit_cmd(CMD_NEWLINE, 0);
                  return;

            case CMD_SAVE_ALL:
                  files.saveall();
                  break;
            case CMD_SAVE_EXIT:
                  files.saveall();
                  quitDoc();
                  return;
            case CMD_QUIT:
                  quitDoc();
                  return;
            case CMD_RECORD:
                  if (recmode) {
                        recmode = false;
                        removeMsg();
                        }
                  else {
                        msg(0, "Recording...");
                        recmode = true;
                        cmd_list.clear();
                        }
                  break;
            case CMD_PLAY:
                  if (recmode) {
                        recmode = false;
                        removeMsg();
                        }
                  play_cmd();
                  break;
            default:
                  break;
            }
      cur_editor->edit_cmd(cmd, param);
      }

//---------------------------------------------------------
//   main
//---------------------------------------------------------

int main(int argc, char**argv)
      {
      int c;
      pedName = argv[0];

      while ((c = getopt(argc, argv, "vlui")) != EOF) {
            switch (c) {
                  case 'v':
                        printVersion(argv[0]);
                        return 0;
                  case 'l':
                        isoLatin = true;
                        utf8     = false;
                        break;
                  case 'u':
                        utf8 = true;
                        isoLatin = false;
                        break;
                  case 'i':
                        changeInPlace = true;
                        break;
                  default:
                        usage("bad argument");
                        return -1;
                  }
            }
      argc -= optind;
      argv += optind;
      start_dir = new QDir(".");

      new QApplication(argc, argv);
      Ped* mw = new Ped(argc, argv);
      mw->show();
      mw->activateWindow();
      return qApp->exec();
      }

//---------------------------------------------------------
//   file
//---------------------------------------------------------

int Ped::file(const QString& s, bool f)
      {
      return cur_editor->file(s, f, 0);
      }

void Ped::open_kontext()         { cur_editor->open_kontext(); }
void Ped::update()               { cur_editor->update();       }
void Ped::goto_kontext(int i)    { cur_editor->gotoKontext(i); }
void Ped::edit(const QChar& cmd) { cur_editor->edit(cmd);      }

