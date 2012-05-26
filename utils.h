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

extern int xgets(char* buffer, int limit, FILE*);
extern int hextobin(int c);
QString urlGen(QString path, QString name);
extern bool strend(const char* src, const char* test);
extern char* tab_expand(const char* buffer, const char* path);
