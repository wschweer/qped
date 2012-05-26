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

