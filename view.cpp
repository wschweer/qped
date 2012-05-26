/***-------------------------------------------------------
 ***  New PEd Editor
 ***  $Id: view.c 1.1 Fri, 24 Nov 2000 15:23:56 +0100 ws $
 ***
 ***  (C) - Copyright Werner Schweer 1997; ws@seh.de
 ***-------------------------------------------------------*/

#include "line.h"
#include "text.h"
#include "ped.h"
#include "cmd.h"

static const int MAX_IFELSE = 128;
static int clevel_stack[MAX_IFELSE];
static int clevel_stack_index;

#define EOL '\n'

/*---------------------------------------------------------
 *    push_clevel
 *---------------------------------------------------------*/

static void push_clevel(int clevel)
      {
      clevel_stack[clevel_stack_index++] = clevel;
      if (clevel_stack_index >= MAX_IFELSE)
            --clevel_stack_index;        // silently avoid overflow
      }

/*---------------------------------------------------------
 *    pop_clevel
 *---------------------------------------------------------*/

static int pop_clevel(int clevel)
      {
      if (clevel_stack_index)
            return clevel_stack[--clevel_stack_index];
      return clevel;                      // silently ignore underflow
      }

/*---------------------------------------------------------
 *    skip_escape
 *---------------------------------------------------------*/

static void skip_escape(LinePos& p)
      {
      if (*p == '0') {              // Oktal - Konstante?
            while ((*p).isDigit())
                  ++p;
            }
      else
            ++p;
      }

/*---------------------------------------------------------
 *    skip_char_const
 *---------------------------------------------------------*/

static void skip_char_const(LinePos& p)
      {
      while (!(*p).isNull()) {
            QChar c = *p++;
            if (c ==  '\'')
                  return;
            else if (c == '\\')
                  skip_escape(p);
            }
      }

/*---------------------------------------------------------
 *    skip_string
 *---------------------------------------------------------*/

static void skip_string(LinePos& p)
      {
      while (!(*p).isNull()) {
            QChar c = *p++;
            if (c == '"')
                  return;
            else if (c == '\\')
                  skip_escape(p);
            }
      }

/*---------------------------------------------------------
 *    test_definition
 *    versucht herauszufinden, ob es sich um eine
 *    Prozedurdeklaration oder um eine Definition handelt
 *---------------------------------------------------------*/

static int test_definition(LinePos p)
      {
      p.to_lastchar();
      while ((*p).isSpace())
            --p;
      QChar c = *p;
      --p;
      if ((c == '/') && (*p == '*')) {
            --p;
            for (; !(*p).isNull() && *p != '\n'; --p) {
                  if ((*p == '*') && (*(p-1) == '/'))
                        break;
                  }
            p -= 2;
            for (; !(*p).isNull() && *p != '\n'; --p)
                  if (!(*p).isSpace())
                        break;
            c = *p;
            }
      return (c != ';' && c != ',');
      }

/*---------------------------------------------------------
 *    skip_comment
 *    scannt Text, bis schließen kommentarklammer erreicht
 *---------------------------------------------------------*/

void skip_comment(LinePos& p)
      {
      /*---------------------------------------------------
       *    der Zeilenlevel der aktuellen Zeile soll
       *    nicht mehr ...
       *    für     mops()   {       / * Kommentar
       *---------------------------------------------------*/

	while (!(*p).isNull()) {
            QChar c = *p++;
            if (c == '*')
                  if (*p == '/') {
                        ++p;
                        return;
                  }
          	}
      }

//---------------------------------------------------------
//   is_ctype
//---------------------------------------------------------

static bool is_ctype(QChar c)
	{
      return c.isLetterOrNumber() || c == '_';
      }

//---------------------------------------------------------
//	add_line
//	Das Wort vor der ersten aufgehenden Klammer wird
//	einger�zedurname)
//---------------------------------------------------------

static void add_line(LineList* d, LinePos& lpos)
      {
      Line* l   = lpos.cur_line();
      int tag   = lpos.line();
      char* p   = l->cstring();
      char* src = p;
      char buffer[strlen(p)+1024];
      char* dst = buffer;

	if (*src == 0) {
            delete p;
            return;
            }

      while (*src && *src != '(')
            ++src;
      if (*src == '(') {
            --src;
            while (src > p && is_ctype(*src))
                  --src;
            if (!is_ctype(*src))
                  ++src;
        	memcpy(dst, p, src-p);
            dst += src - p;
            for (int i = dst-buffer; i < 20; ++i)
                  *dst++ = ' ';
		while (*src)
      		*dst++ = *src++;
	      *dst = 0;
            }
      else
            strcpy(buffer, p);
      delete p;
      Line* nl = new Line(buffer);
      nl->setTag(tag);
      d->push_back(nl);
      }

//---------------------------------------------------------
//	getProcName
//---------------------------------------------------------

static char* getProcName(const LinePos& lpos)
      {
      Line*l    = lpos.cur_line();
      const char* p   = l->text().toLatin1().data();
      const char* src = p;

	if (*src == 0) {
            delete p;
            return 0;
            }

      while (*src && *src != '(')
            ++src;
      if (*src != '(')
            return 0;
      const char* ep = src;
      --src;
      while (src > p && is_ctype(*src))
            --src;
      if (!is_ctype(*src))
            ++src;
      char buffer[ep-src+1];
      memcpy(buffer, src, ep - src);
      buffer[ep-src] = 0;
      return strdup(buffer);
      }

//---------------------------------------------------------
//    Kontext::generate_cview
//---------------------------------------------------------

int Kontext::generate_cview(LineList* dst, LineList* src, int zl)
      {
      int view_zl        = 0;
      int clevel         = 0;      // aktueller Kompound-Level
      int namesp         = 0;

      clevel_stack_index = 0;
      LinePos p(src, src->begin(), 0, 0);

      while (!(*p).isNull()) {
            if (*p == '#') {        // Pr�r-Zeile
                  ++p;
                  while (*p == ' ' || *p == '\t')
                        ++p;
                  if (p.match("if"))
                        push_clevel(clevel);
                  else if (p.match("endif"))
                        clevel = pop_clevel(clevel);
                  else if (p.match("else")) {
                        clevel = pop_clevel(clevel);
                        push_clevel(clevel);
                        }
			p.skipline();
                  continue;
                  }

            if (p.match("namespace") && clevel == 0) {
                  clevel = -1;
                  namesp = 1;
                  }
            if (p.match("class")) {
                  add_line(dst, p);
                  if (p.line() <= zl)
                        view_zl = dst->size() - 1;
                  }
            else if (p.match("struct")) {
                  add_line(dst, p);
                  if (p.line() <= zl)
                        view_zl = dst->size() - 1;
                  }
            else if (p.match("typedef")) {
                  add_line(dst, p);
                  if (p.line() <= zl)
                        view_zl = dst->size() - 1;
                  }
            while (!(*p).isNull()) {
                  switch ((*p++).toAscii()) {
                  	case '\n':
                        	goto nextline;
                        case '{':         // aufgehende Kompound-Klammer
                              ++clevel;
                              break;
                        case '}':         // zugehende Kompound-Klammer
                              clevel--;
                              break;
                        case '(':
                        case ')':
                              if (clevel == 0) {
                                    if (test_definition(p)) {
                                          add_line(dst, p);
                                          if (p.line() <= zl)
                                                view_zl = dst->size() - 1;
#if 0
                                          if (*(p-1) == ')') {
                                                iLineList pl = p.pos();
                                                --pl;
                                                if (pl != src->end()) {
                                                      add_line(dst, pl);
                                                      if (p.line() <= zl)
                                                            view_zl = dst->size() - 1;
                                                      }
                                                }
#endif
                                          // skip bis Zeilenende oder {
                                          while (!(*p).isNull()) {
                                                if (*p == '{' || *p == '\n')
                                                      break;
                                                ++p;
                                                }
                                          }
                                    }
                              break;
                        case '\'':
                              skip_char_const(p);
                              break;
                        case '"':
                              skip_string(p);
                              break;
                        case '/':
                              if (*p == '/') {
                                    p.skipline();
                                    goto nextline;
                                    }
                              if (*p == '*')
                                    skip_comment(++p);
                              break;
                        }
                  // next char
                  }
nextline:
            ;
            }
      if (clevel)
		ped->msg(2000, QString("{} nicht in Balance(%1)").arg(clevel));
      return view_zl;
      }

//---------------------------------------------------------
//    Kontext::c_compound
//---------------------------------------------------------

int Kontext::c_compound()
      {
      int level          = 0;

      LinePos p(text, text->begin(), 0, 0);
      LinePos cur(text, cursor, pos.zeile, pos.spalte);
//      int macro          = false;
      clevel_stack_index = 0;

	while (p < cur) {
      	if (p.col() == 0 && *p == '#') {
			while (*p == ' ' || *p == '\t')
                  	++p;
                	if (strncmp(p, "if", 2) == 0)
                  	push_clevel(level);
               	else if (strncmp(p, "endif", 5) == 0)
                  	level = pop_clevel(level);
              	else if (strncmp(p, "else", 5) == 0)
                  	level = pop_clevel(level);
                  for (;;) {
                        p.to_lastchar();
                        if (*p != '\\')
                              break;
                        p.skipline();
                        }
		 	p.skipline();
                  continue;
                  }
            if (level == 0 && p.match("namespace"))
                  --level;
            QChar c(*p++);
		if (c == QChar('{', 0))
                  ++level;
            else if (c == QChar('}', 0))
                  level--;
            else if (c == QChar('\'', 0))
                  skip_char_const(p);
            else if (c == QChar('\"', 0))
                  skip_string(p);
            else if (c == QChar('/')) {
                  if (*p == QChar('/', 0))
                        p.skipline();
		      else if (*p == QChar('*', 0)) {
                        ++p;
                        skip_comment(p);
                        }
                  }
      	}
      return level;
      }

//---------------------------------------------------------
//   cmd_compound
//---------------------------------------------------------

void Kontext::cmd_compound()
      {
      int level;
      switch (f->getFileType()) {
            default:
            case FILE_C:
            case FILE_H:
            case FILE_TEXT:
            case FILE_UNKNOWN:
                  level = c_compound();
                  break;
            }
      ped->msg(2000, QString("{} Balance %1").arg(level));
      }

char* Kontext::findProc(LinePos& p, int line)
      {
      char* proc = 0;
      int clevel         = 0;      // aktueller Kompound-Level
      clevel_stack_index = 0;

      QChar c;
      while (!(c = *p++).isNull()) {
            if ((p.col() == 1) && (c == '#')) {    // Pr�r-Zeile
                  while ((*p).isSpace())
                        ++p;
                  if (p.match("if"))
                        push_clevel(clevel);
                  else if (p.match("endif"))
                        clevel = pop_clevel(clevel);
                  else if (p.match("else")) {
                        clevel = pop_clevel(clevel);
                        push_clevel(clevel);
                        }
			p.skipline();
                  continue;
                  }
            switch (c.toAscii()) {
                  case '{':         // aufgehende Kompound-Klammer
                        ++clevel;
                        break;
                  case '}':         // zugehende Kompound-Klammer
                        clevel--;
                        break;
                  case '(':
                  case ')':
                        if ((clevel == 0) && (test_definition(p))) {
                              int n = p.line();
                              if (n >= line) {
                                    if (n == line) {
                                          if (proc)
                                                delete proc;
                                          return getProcName(p);
                                          }
                                    return proc;
                                    }
                              if (proc)
                                    delete proc;
                              proc = getProcName(p);
                              // skip bis Zeilenende oder {
                              while (!(*p).isNull()) {
                                    if (*p == '{' || *p == '\n')
                                          break;
                                    ++p;
                                    }
                              }
                        break;
                  case '\'':
                        skip_char_const(p);
                        break;
                  case '"':
                        skip_string(p);
                        break;
                  case '/':
                        if (*p == '/') {
                              p.skipline();
                              break;
                              }
                        if (*p == '*')
                              skip_comment(++p);
                        break;
                  }
            }
      return proc;
      }

