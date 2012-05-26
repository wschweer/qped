//=========================================================
//    Lmed
//    Linux {music, midi, media} editor
//    $Id: utils.h 1.1 Fri, 24 Nov 2000 15:23:56 +0100 ws $
//
//    defines top level data structures
//    (C) Copyright ws@seh.de, 1999
//=========================================================

extern int xgets(char* buffer, int limit, FILE*);
extern int hextobin(int c);
QString urlGen(QString path, QString name);
extern bool strend(const char* src, const char* test);
extern char* tab_expand(const char* buffer, const char* path);
