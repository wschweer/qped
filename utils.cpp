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

//---------------------------------------------------------
//   xgets
//    wie fgets, nur werden nachlaufende '\n' '\r' und ' '
//    abgeschnitten
//   Return:
//    number of char's in line; can be zero!
//    -1 - EOF
//    -2 - Error
//---------------------------------------------------------

int xgets(char* buffer, int limit, FILE* f)
      {
      if (fgets(buffer, limit, f) == 0) {
            if (ferror(f)) {
                  perror("error fgets");
                  }
            if (feof(f))
                  return -1;
            return -2;
            }
      int n = strlen(buffer);
      while (n && (buffer[n-1] == '\n' || buffer[n-1] == '\r' || buffer[n-1] == ' ')) {
            --n;
            buffer[n] = 0;
            }
      return n;
      }

//---------------------------------------------------------
//	strndup
//---------------------------------------------------------

char* strndup(const char*p, int n)
      {
      char* buf = new char[n+1];
      memcpy(buf, p, n);
      buf[n] = 0;
      return buf;
      }

//---------------------------------------------------------
//	hextobin
//---------------------------------------------------------

int hextobin(int c)
	{
      const char* hexdigits = "0123456789ABCDEF";
      c = toupper(c);
      const char* p = strchr(hexdigits, c);
      if (p)
            return p - hexdigits;
      return -1;
      }

/*---------------------------------------------------------
 *	is_limit
 *---------------------------------------------------------*/

static int is_limit(int c)
      {
      return (c == ' ' || c == '\t' || c == '/' || c == 0);
      }

/*---------------------------------------------------------
 *    expand_buffer
 *    expandiert Shell-Variablen auf Kommandozeile
 *---------------------------------------------------------*/

const int MAX_EXPAND_BUFFER = 1024;
const int MAX_SHELLVAR = 128;

static char* expand_buffer(const char* s)
      {
      char Buffer[MAX_EXPAND_BUFFER];
      char token[MAX_SHELLVAR+1];
      char *env;
      int c;

      char* d = Buffer;

      //
      // ~/  expandiert nach $HOME/
      //
      if ((*s == '~') && (*(s+1) == '/')) {
            if ((env = getenv("HOME")) != 0) {
                  while (*env) {
                        *d++ = *env++;
                        if (d >= (Buffer+MAX_EXPAND_BUFFER-1)) {
                              *d = 0;
                              return strdup(Buffer);
                              }
                        }
                  }
            ++s;              // / noch mitnehmen
            }
      while ((c = *s++)) {
            if (c == '$') {
                  char* t = token;
                  for (int i = 0; i < MAX_SHELLVAR; ++i) {
                        if (is_limit(*s)) {
                              *t = 0;
                              if ((env = getenv(token)) != 0) {
                                    while (*env) {
                                          *d++ = *env++;
                                          if (d >= (Buffer+MAX_EXPAND_BUFFER-1)) {
                                                *d = 0;
                                                return strdup(Buffer);
                                                }
                                          }
                                    }
                              break;
                              }
                        *t++ = *s++;
                        }
                  continue;
                  }
            *d++ = c;
            if (d >= (Buffer+MAX_EXPAND_BUFFER-1))
                  break;
           }
      *d = 0;
      return strdup(Buffer);
      }

/*---------------------------------------------------------
 *    tab_expand
 *    versucht, die Enter-Textzeile bei Eingabe eines TABS
 *    zu expandieren, analog der BASH-Shell Funktion
 *---------------------------------------------------------*/

char* tab_expand(const char* buffer, const char* cur_dir)
      {
      char* s = expand_buffer(buffer);
      char* p = strrchr(s, '/');
      // char* dirname;

      if (p) {
            // dirname = strndup(s, size_t(p - s + 1));
            ++p;
            }
      else {
            // dirname = strdup(cur_dir);
            p = s;
            }

      char buf[512];
      buf[0] = 0;             // enth�t Erg�zungsstring
      int n = strlen(p);

      bool flag = false;

      QDir dir(cur_dir);
      dir.setFilter(QDir::Files);
      QFileInfoList list = dir.entryInfoList();

      for (int i = 0; i < list.size(); ++i) {
            QFileInfo fi = list.at(i);
            const char* key = fi.fileName().toLatin1().data();
            if (strncmp(key, p, n) == 0) {
                  if (flag) {
                        const char* s1 = key + n;
                        char* s2 = buf;
                        while (*s1 && *s2) {
                              if (*s1 != *s2)
                                    break;
                              ++s1;
                              ++s2;
                              }
                        *s2 = 0;
                        }
                  else {
                        strcpy(buf, key + n);
                        flag = true;
                        }
                  }
            }
      char* rv = new char[strlen(s)+strlen(buf)+1];
      strcpy(rv, s);
      strcat(rv, buf);
      delete s;
      return rv;
      }

//---------------------------------------------------------
//   url_gen
//---------------------------------------------------------

QString urlGen(QString path, QString name)
      {
      if (name[0] == '/')
            return name;
      return path + "/" + name;
      }


