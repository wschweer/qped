//=========================================================
//  MusE
//  Linux Music Editor
//  $Id: xml.h,v 1.4 2006/01/25 16:24:32 wschweer Exp $
//
//  (C) Copyright 2005 Werner Schweer (ws@seh.de)
//=========================================================

#ifndef __XML_H__
#define __XML_H__

static const int BS = 2048;

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

