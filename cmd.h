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

enum {
      CMD_NEXT = 0x100,
      CMD_BACK,
      CMD_UP,
      CMD_DOWN,
      CMD_PAGE_UP,
      CMD_PAGE_DOWN,
      CMD_SCROLL_UP,
      CMD_SCROLL_DOWN,
      CMD_BACK_WORD,
      CMD_NEXT_WORD,
      CMD_START_LINE,
      CMD_END_LINE,
      CMD_START_WINDOW,
      CMD_END_WINDOW,
      CMD_START_FILE,
      CMD_END_FILE,
      CMD_GOTO_LINK,
      CMD_MIX_LINES,
      CMD_DEL_RESTLINE,
      CMD_DEL_STARTLINE,
      CMD_RUBOUT,
      CMD_DEL_CHAR,
      CMD_DEL_WORD,
      CMD_DEL_LINE,
      CMD_INS_SPECIAL,
      CMD_NEWLINE,
      CMD_VOR_HIST,
      CMD_BACK_HIST,
      CMD_VIEW_TOGGLE,
      CMD_RELOAD,
      CMD_SAVE,
      CMD_SAVE_ALL,
      CMD_CKLAMMER,
      CMD_CKOMPOUND,
      CMD_LINE_MARK,
      CMD_COLUMN_MARK,
      CMD_SEARCH_F,
      CMD_SEARCH_R,
      CMD_PICK,
      CMD_PUT,
      CMD_UNDELETE,
      CMD_REM_ALTFIL,
      CMD_SCRIPT,
      CMD_TABSIZE,
      CMD_MAN,
      CMD_NEW_ALTFIL,
      CMD_NEW_HIST,
      CMD_ENTER_KOLLAPS,
      CMD_FUNCTION,
      CMD_GOTO,
      CMD_UNDO,
      CMD_REDO,
      CMD_MAIL,
      CMD_TAB,
      CMD_SORT,
      CMD_THREAD,
      CMD_ASCENDING,
      CMD_DEL_DOUBLES,
      CMD_SLEEP,
      CMD_RECORD,
      CMD_PLAY,
      CMD_COMPOSE,
      CMD_WRAPFLAG,
      CMD_SOURCEFLAG,
      CMD_NEXT_KONTEXT,
      CMD_SAVE_EXIT,
      CMD_QUIT,
      CMD_PREV_KONTEXT,
      CMD_SHELL,
      CMD_ENTER_INPUT,
      CMD_STREAM,
      CMD_LINK,
      CMD_INSERT_LINE,
      CMD_GET_WORD,
      CMD_INPUT_STRING,
      CMD_MAXCODE
      };

const int ESCAPE = 0x1b;

extern int cmd_by_name(const char* name);
extern void cmd_table_init();

