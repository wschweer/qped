//=========================================================
//  MusE
//  Linux Music Editor
//  $Id: xml.cpp,v 1.9 2006/01/31 20:33:22 wschweer Exp $
//
//  (C) Copyright 2004-2006 Werner Schweer (ws@seh.de)
//=========================================================

#include "xml.h"


//---------------------------------------------------------
//   Xml
//---------------------------------------------------------

Xml::Xml(QIODevice* device)
   : QTextStream(device)
      {
      level = 0;
      }

//---------------------------------------------------------
//   header
//---------------------------------------------------------

void Xml::header()
      {
      *this << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
      }

//---------------------------------------------------------
//   put
//---------------------------------------------------------

void Xml::put(const char* format, ...)
      {
      va_list args;
      va_start(args, format);
      putLevel();
      vsnprintf(buffer, BS, format, args);
      va_end(args);
    	*this << buffer;
      *this << '\n';
      }

//---------------------------------------------------------
//   nput
//---------------------------------------------------------

void Xml::nput(const char* format, ...)
      {
      va_list args;
      va_start(args, format);
      vsnprintf(buffer, BS, format, args);
    	*this << buffer;
      va_end(args);
      }

//---------------------------------------------------------
//   tdata
//---------------------------------------------------------

void Xml::tdata(const QString& s)
      {
      putLevel();
      *this << s << endl;
      }

//---------------------------------------------------------
//   tag
//---------------------------------------------------------

void Xml::tag(const char* format, ...)
      {
      va_list args;
      va_start(args, format);
      putLevel();
      *this << '<';
      vsnprintf(buffer, BS, format, args);
    	*this << buffer;
      va_end(args);
      *this << '>' << endl;
      ++level;
      }

void Xml::tagE(const char* format, ...)
      {
      va_list args;
      va_start(args, format);
      putLevel();
      *this << '<';
      vsnprintf(buffer, BS, format, args);
    	*this << buffer;
      va_end(args);
      *this << "/>" << endl;
      }

//---------------------------------------------------------
//   etag
//---------------------------------------------------------

void Xml::etag(const char* format, ...)
      {
      va_list args;
      va_start(args, format);
      putLevel();
      *this << "</";
      vsnprintf(buffer, BS, format, args);
    	*this << buffer;
      va_end(args);
      *this << '>' << endl;
      --level;
      }

void Xml::putLevel()
      {
      for (int i = 0; i < level*2; ++i)
            *this << ' ';
      }

void Xml::intTag(const char* name, int val)
      {
      putLevel();
      *this << "<" << name << ">" << val << "</" << name << ">\n";
      }

void Xml::floatTag(const char* name, float val)
      {
      putLevel();
      *this << QString("<%1>%2</%3>\n").arg(name).arg(val).arg(name);
      }

void Xml::doubleTag(const char* name, double val)
      {
      putLevel();
      QString s("<%1>%2</%3>\n");
      *this << s.arg(name).arg(val).arg(name);
      }

void Xml::strTag(const char* name, const char* val)
      {
      putLevel();
      *this << "<" << name << ">";
      if (val) {
            while (*val) {
                  switch(*val) {
                        case '&':
                              *this << "&amp;";
                              break;
                        case '<':
                              *this << "&lt;";
                              break;
                        case '>':
                              *this << "&gt;";
                              break;
                        case '"':
                              *this << "&quot;";
                              break;
                        case '\'':
                              *this << "&apos;";
                              break;
                        default:
                              *this << *val;
                              break;
                        }
                  ++val;
                  }
            }
      *this << "</" << name << ">\n";
      }

//---------------------------------------------------------
//   colorTag
//---------------------------------------------------------

void Xml::colorTag(const char* name, const QColor& color)
      {
      putLevel();
      snprintf(buffer, BS, "<%s r=\"%d\" g=\"%d\" b=\"%d\" />\n",
	    name, color.red(), color.green(), color.blue());
    	*this << buffer;
      }

//---------------------------------------------------------
//   geometryTag
//---------------------------------------------------------

void Xml::geometryTag(const char* name, const QWidget* g)
      {
      qrectTag(name, QRect(g->pos(), g->size()));
      }

//---------------------------------------------------------
//   qrectTag
//---------------------------------------------------------

void Xml::qrectTag(const char* name, const QRect& r)
      {
      putLevel();
   	*this << "<" << name;
      snprintf(buffer, BS, " x=\"%d\" y=\"%d\" w=\"%d\" h=\"%d\" />\n",
         r.x(), r.y(), r.width(), r.height());
    	*this << buffer;
      }

//---------------------------------------------------------
//   strTag
//---------------------------------------------------------

void Xml::strTag(const char* name, const QString& val)
      {
      strTag(name, val.toLatin1().data());
      }

//---------------------------------------------------------
//   readGeometry
//---------------------------------------------------------

QRect readGeometry(QDomNode node)
      {
      QDomElement e = node.toElement();
      int x = e.attribute("x","0").toInt();
      int y = e.attribute("y","0").toInt();
      int w = e.attribute("w","50").toInt();
      int h = e.attribute("h","50").toInt();
      return QRect(x, y, w, h);
      }

//---------------------------------------------------------
//   domError
//---------------------------------------------------------

void domError(QDomNode node)
      {
            QDomElement e = node.toElement();
            QString tag(e.tagName());
            QString s;
            while (!node.parentNode().isNull()) {
                        node = node.parentNode();
                        const QDomElement e = node.toElement();
                        const QString k(e.tagName());
                        if (!s.isEmpty())
                                    s += ":";
                        s += k;
                        }
            printf("%s: Unknown Node <%s>\n", s.toLatin1().data(), tag.toLatin1().data());
      //      abort();
            }


