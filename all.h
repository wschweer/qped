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

#include <stdarg.h>
#include <stdio.h>
#include <vector>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <list>
#include <locale.h>
#include <map>

#include <QtWidgets/QFrame>
#include <QMouseEvent>
#include <QShortcut>
#include <QStackedWidget>
#include <QTabBar>
#include <QTabWidget>
#include <QTemporaryFile>
#include <QFontDialog>
#include <QColorDialog>
#include <QtXml/qdom.h>

#include <qapplication.h>
#include <qclipboard.h>
#include <qdialog.h>
#include <qdir.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qlistview.h>
#include <qmainwindow.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qobject.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpoint.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qrect.h>
#include <qscrollbar.h>
#include <qsocketnotifier.h>
#include <qsplitter.h>
#include <qstatusbar.h>
#include <qtextstream.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qwidget.h>
#include <QTextCodec>
