/***-------------------------------------------------------
 ***  PEd Editor
 ***        KDE - Version
 ***  $Id: help.c 1.1 Fri, 24 Nov 2000 15:23:56 +0100 ws $
 ***
 ***  (C) - Copyright Werner Schweer 1999; ws@seh.de
 ***-------------------------------------------------------*/

#include "ped.h"
#include "cmd.h"

//---------------------------------------------------------
//   helpAbout
//---------------------------------------------------------

void Ped::helpAbout()
      {
      QMessageBox::about(this, "qped Editor",
      "this is qped\n"
      "Copyright Werner Schweer 2000-2007\nws@seh.de");
      }
class CommandsDialog : public QDialog {
//      QMLBrowser* l;
   public:
      CommandsDialog(QWidget*);
      void closeEvent(QCloseEvent*e);
      };

CommandsDialog::CommandsDialog(QWidget* parent) :
   QDialog(parent)
      {
      setWindowTitle("qped Command Reference");
      setMinimumHeight(200);
      setMinimumWidth(300);

//      QVBoxLayout* vbox = new QVBoxLayout(this);
//      qtProvider* provider = new qtProvider(this);
//      l = new QMLBrowser(this);
//      vbox->addWidget(l, 2);
//      l->setProvider(provider);
//      l->setDocument("index.doc");
//      l->show();
      show();
      }

//---------------------------------------------------------
//   closeEvent
//---------------------------------------------------------

void CommandsDialog::closeEvent(QCloseEvent* e)
      {
      e->accept();
      }

//---------------------------------------------------------
//   helpCommands
//---------------------------------------------------------

void Ped::helpCommands()
      {
      if (commandsDialog) {
            commandsDialog->show();
            return;
            }
      commandsDialog = new CommandsDialog(this);
      }

//---------------------------------------------------------
//   helpCommands
//---------------------------------------------------------

void Ped::helpQt()
      {
      edit_cmd(CMD_NEW_ALTFIL, "/usr/qt2/doc/html/index.html");
      }

