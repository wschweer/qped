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
#include "cmd.h"

//---------------------------------------------------------
//   helpAbout
//---------------------------------------------------------

void Ped::helpAbout()
      {
      QMessageBox::about(this, "qped Editor",
      "This is qped.\n"
      "Copyright Werner Schweer.\n");
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

