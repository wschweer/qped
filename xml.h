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

#ifndef __XML_H__
#define __XML_H__

#include <QRect>
#include <QDomNode>
static const int BS = 2048;

class QColor;
class QWidget;

//---------------------------------------------------------
//   Xml
//---------------------------------------------------------

class Xml : public QTextStream {
    	char buffer[BS];
      int level;

   public:
      Xml(QIODevice*);
      void header();
      void putLevel();
      void put(const char* format, ...);
      void nput(const char* format, ...);
      void tag(const char* format, ...);
      void etag(const char* format, ...);
      void tagE(const char* format, ...);
      void tdata(const QString&);
      void intTag(const char* const name, int val);
      void doubleTag(const char* const name, double val);
      void floatTag(const char* const name, float val);
      void strTag(const char* const name, const char* val);
      void strTag(const char* const name, const QString& s);
      void colorTag(const char* name, const QColor& color);
      void geometryTag(const char* name, const QWidget* g);
      void qrectTag(const char* name, const QRect& r);
      };

extern QRect readGeometry(QDomNode);
extern void domError(QDomNode node);

#endif

