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
#include "xml.h"
#include "editwin.h"

QColor fgColor(0, 0, 0);
QColor bgColor(240, 240, 240);
QString configFilename(".q4ped");

//---------------------------------------------------------
//   configFgColor
//---------------------------------------------------------

void Ped::configFgColor()
      {
      QColor color = QColorDialog::getColor(fgColor, this);
      if (color.isValid()) {
            fgColor = color;
            cur_editor->update();
            }
      }

//---------------------------------------------------------
//   configBgColor
//---------------------------------------------------------

void Ped::configBgColor()
      {
      QColor color = QColorDialog::getColor(bgColor, this);
      if (color.isValid()) {
            bgColor = color;
            cur_editor->update();
            }
      }

//---------------------------------------------------------
//   configFont
//---------------------------------------------------------

void Ped::configFont()
      {
      QFont old_font = cur_editor->win->font();
      bool flag;
      QFont font = QFontDialog::getFont(&flag, old_font, 0, "select font", QFontDialog::MonospacedFonts);
      if (flag) {
            fontFamily = font.family();
            fontWeight = font.weight();
            fontSize   = font.pointSizeF();
            setFont();
//            cur_editor->setEditorFont(font);
//            cur_editor->update();
            }
      }

//---------------------------------------------------------
//   saveConfig
//---------------------------------------------------------

void Ped::saveConfig()
      {
      QString fname(QDir::homePath() + "/" + configFilename);
      QFile f(fname);
      if (!f.open(QIODevice::WriteOnly)) {
            msg(5000, QString("save config failed: cannot open <%1>: <%2>")
               .arg(f.fileName()).arg(strerror(errno)));
            return;
            }
      Xml xml(&f);
      xml.header();
      xml.tag("PedConfig");

      xml.colorTag("foregroundColor", fgColor);
      xml.colorTag("backgroundColor", bgColor);
      xml.geometryTag("geometry", this);
      xml.intTag("colorify", colorify);
      xml.intTag("paren", paren);
      xml.strTag("fontFamily", fontFamily);
      xml.doubleTag("fontSize", fontSize);
      xml.intTag("fontWeight", fontWeight);

      xml.etag("PedConfig");
      msg(5000, QString("configuration saved in \"%1\"").arg(f.fileName()));
      }

//---------------------------------------------------------
//    readConfig
//---------------------------------------------------------

void Ped::readConfig()
      {
      QString fname(QDir::homePath() + "/" + configFilename);
      QFile f(fname);
      if (!f.open(QIODevice::ReadOnly)) {
	    printf("====readConfig failed\n");
            saveConfig();
//            msg(5000, QString("read config failed: cannot open <%1>: <%2>\n")
//               .arg(f.fileName()).arg(strerror(errno)));
            return;
            }
      QDomDocument doc;
      int line, column;
      QString err;

      fontWeight = 50;
      fontSize   = 18;

      if (!doc.setContent(&f, false, &err, &line, &column)) {
            printf("error reading file <%s> at line %d column %d: %s\n",
               fname.toLatin1().data(), line, column, err.toLatin1().data());
            return;
            }

      for (QDomNode node = doc.documentElement(); !node.isNull(); node = node.nextSibling()) {
            QDomElement e = node.toElement();
            if (e.isNull() || e.tagName() != "PedConfig") {
                  domError(node);
                  continue;
                  }
            for (QDomNode nnode = node.firstChild(); !nnode.isNull(); nnode = nnode.nextSibling()) {
                  QDomElement e = nnode.toElement();
                  if (e.isNull())
                        continue;
                  QString tag = e.tagName();
                  if (tag == "foregroundColor") {
                        int r = e.attribute("r").toInt();
                        int g = e.attribute("g").toInt();
                        int b = e.attribute("b").toInt();
                        fgColor.setRgb(r, g, b);
                        }
                  else if (tag == "backgroundColor") {
                        int r = e.attribute("r").toInt();
                        int g = e.attribute("g").toInt();
                        int b = e.attribute("b").toInt();
                        bgColor.setRgb(r, g, b);
                        }
                  else if (tag == "geometry") {
                        QRect r = readGeometry(nnode);
                        setGeometry(r);
                        }
                  else if (tag == "colorify")
                        ;
                  else if (tag == "paren")
                        ;
                  else if (tag == "fontFamily")
                        fontFamily = e.text();
                  else if (tag == "fontSize")
                        fontSize = e.text().toDouble();
                  else if (tag == "fontWeight")
                        fontWeight = e.text().toInt();
                  else
                        domError(nnode);
                  }
            }
      f.close();
      }
