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

const int CMD_NEXT          = 0x100;
const int CMD_BACK          = 0x101;
const int CMD_UP            = 0x102;
const int CMD_DOWN          = 0x103;
const int CMD_PAGE_UP       = 0x104;
const int CMD_PAGE_DOWN     = 0x105;
const int CMD_SCROLL_UP     = 0x106;
const int CMD_SCROLL_DOWN   = 0x107;
const int CMD_BACK_WORD     = 0x108;
const int CMD_NEXT_WORD     = 0x109;
const int CMD_START_LINE    = 0x10a;
const int CMD_END_LINE      = 0x10b;
const int CMD_START_WINDOW  = 0x10c;
const int CMD_END_WINDOW    = 0x10d;
const int CMD_START_FILE    = 0x10e;
const int CMD_END_FILE      = 0x10f;
const int CMD_GOTO_LINK     = 0x111;
const int CMD_MIX_LINES     = 0x112;
const int CMD_DEL_RESTLINE  = 0x113;
const int CMD_DEL_STARTLINE = 0x114;
const int CMD_RUBOUT        = 0x115;
const int CMD_DEL_CHAR      = 0x116;
const int CMD_DEL_WORD      = 0x117;
const int CMD_DEL_LINE      = 0x118;
const int CMD_INS_SPECIAL   = 0x119;
const int CMD_NEWLINE       = 0x11a;
const int CMD_VOR_HIST      = 0x11b;
const int CMD_BACK_HIST     = 0x11c;
const int CMD_VIEW_TOGGLE   = 0x11d;
const int CMD_RELOAD        = 0x11e;
const int CMD_SAVE          = 0x11f;
const int CMD_SAVE_ALL      = 0x120;
const int CMD_CKLAMMER      = 0x123;
const int CMD_CKOMPOUND     = 0x124;
const int CMD_INSERTMODE    = 0x125;
const int CMD_LINE_MARK     = 0x127;
const int CMD_COLUMN_MARK   = 0x128;
const int CMD_SEARCH_F      = 0x129;
const int CMD_SEARCH_R      = 0x12a;
const int CMD_PICK          = 0x12b;
const int CMD_PUT           = 0x12c;
const int CMD_UNDELETE      = 0x12d;
const int CMD_REM_ALTFIL    = 0x12e;
const int CMD_SCRIPT        = 0x12f;

const int CMD_TABSIZE       = 0x131;
const int CMD_MAN           = 0x132;
const int CMD_NEW_ALTFIL    = 0x133;
const int CMD_NEW_HIST      = 0x134;
const int CMD_ENTER_KOLLAPS = 0x135;
const int CMD_FUNCTION      = 0x136;
const int CMD_GOTO          = 0x137;
const int CMD_UNDO          = 0x138;
const int CMD_REDO          = 0x139;
const int CMD_MAIL          = 0x13a;
const int CMD_TAB           = 0x13b;
const int CMD_SORT          = 0x13c;
const int CMD_THREAD        = 0x13d;
const int CMD_ASCENDING     = 0x13e;
const int CMD_DEL_DOUBLES   = 0x13f;
const int CMD_SLEEP         = 0x140;
const int CMD_RECORD        = 0x141;
const int CMD_PLAY          = 0x142;
const int CMD_COMPOSE       = 0x144;
const int CMD_WRAPFLAG      = 0x145;
const int CMD_SOURCEFLAG    = 0x146;
const int CMD_NEXT_KONTEXT  = 0x148;
const int CMD_SAVE_EXIT     = 0x149;
const int CMD_QUIT          = 0x14a;
const int CMD_PREV_KONTEXT  = 0x14b;
const int CMD_SHELL         = 0x14c;
const int CMD_ENTER_INPUT   = 0x14d;
const int CMD_STREAM        = 0x14e;
const int CMD_LINK          = 0x14f;
const int CMD_INSERT_LINE   = 0x150;
const int CMD_GET_WORD      = 0x151;

const int CMD_MAXCODE       = 0x151;


//----------------------------------------------------------
//   control codes
//----------------------------------------------------------

const int C_A = 0x1;
const int C_B = 0x2;
const int C_C = 0x3;
const int C_D = 0x4;
const int C_E = 0x5;
const int C_F = 0x6;
const int C_G = 0x7;
const int C_H = 0x8;
const int C_I = 0x9;
const int C_J = 0xa;
const int C_K = 0xb;
const int C_L = 0xc;
const int C_M = 0xd;
const int C_N = 0xe;
const int C_O = 0xf;
const int C_P = 0x10;
const int C_Q = 0x11;
const int C_R = 0x12;
const int C_S = 0x13;
const int C_T = 0x14;
const int C_U = 0x15;
const int C_V = 0x16;
const int C_W = 0x17;
const int C_X = 0x18;
const int C_Y = 0x19;
const int C_Z = 0x1a;
const int ESCAPE = 0x1b;

extern int cmd_by_name(const char* name);
extern void cmd_table_init();

