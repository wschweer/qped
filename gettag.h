/***-------------------------------------------------------
 ***  PEd Editor
 ***        Qt - Version
 ***  $Id: gettag.h 1.1 Fri, 24 Nov 2000 15:23:56 +0100 ws $
 ***
 ***  (C) - Copyright Werner Schweer 1999; ws@seh.de
 ***-------------------------------------------------------*/


#ifndef __GETTAG_H__
#define __GETTAG_H__

//---------------------------------------------------------
//   Link
//---------------------------------------------------------

class Link {
      QString _url;
      int _line;

   public:
      Link(const QString& s, int l) : _url(s), _line(l) {}
      QString url() const { return _url; }
      int line() const { return _line; }
      };

extern Link* gettag(const QString& t);

#endif

