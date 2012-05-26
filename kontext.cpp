/***-------------------------------------------------------
 ***  PEd Editor
 ***        KDE - Version
 ***  $Id:$
 ***
 ***  (C) - Copyright Werner Schweer 2005; ws@seh.de
 ***-------------------------------------------------------*/

#include "ped.h"
#include "kontext.h"
#include "editwin.h"

//---------------------------------------------------------
//   Kontext
//---------------------------------------------------------

Kontext::Kontext(File* file, Editor* w, Ped* p, const Position* npos)
      {
      f              = file;
      editor         = w;
      ped            = p;
	openflag       = false;
      auto_indent    = true;
      insert_flag    = true;
      update_flags   = UPDATE_ALL;
      cursor_visible = false;
      view_mode      = 0;
      mark_mode      = MARK_NONE;
      text           = 0;
      paren_line     = -1;

      if (npos)
            pos = *npos;
      f->subscribe(this);
      }

Kontext::~Kontext()
	{
      f->unsubscribe(this);
      }

int Kontext::yoffset() const  { return editor->win->yoffset(); }
int Kontext::xoffset() const  { return editor->win->xoffset(); }
int Kontext::zeilen() const   { return editor->win->zeilen();  }
int Kontext::spalten() const  { return editor->win->spalten(); }

//---------------------------------------------------------
//   path
//---------------------------------------------------------

QString Kontext::path() const
      {
      return f->path();
      }

//---------------------------------------------------------
//   name
//---------------------------------------------------------

QString Kontext::name() const
      {
      return f->name();
      }

void Kontext::saveAs(const char* p)
      {
      f->saveAs(p);
      }

KontextList::~KontextList()
	{
	for (iKontextList i = begin(); i != end(); ++i)
		delete(*i);
	}

iKontextList KontextList::at(int i)
	{
	iKontextList k = begin();
	while (i-- > 0 && k != end())
		++k;
	return k;
	}

KontextListList::~KontextListList()
	{
	for (iKontextListList i = begin(); i != end(); ++i)
		delete(*i);
	}

void KontextListList::append(KontextList* cl)
	{
      push_back(cl);
	cc = --end();
	}

iKontextListList KontextListList::at(int i)
	{
	iKontextListList l = begin();
	while (i-- > 0 && l != end())
		++l;
  	return l;
	}

int KontextList::cur()
	{
      return this ? distance(begin(), _cont) : 0;
      }

int KontextListList::cur()
	{
      return distance(begin(), cc);
	}

//---------------------------------------------------------
//   delete_cur_context
//---------------------------------------------------------

void KontextListList::delete_cur_context()
	{
	if (empty())
		return;
      unsigned idx = cur();
      delete *cc;
      erase(cc);
      if (idx >= size())
            idx = 0;
      setcur(idx);
	}

bool KontextList::first_kontext()
	{
	return ((_cont == begin()) || (_cont == end()));
	}

bool KontextList::last_kontext()
	{
	return ((_cont == end()) || (_cont == --end()));
	}

//---------------------------------------------------------
//    operator==
//---------------------------------------------------------

bool operator==(const Position& p1, const Position& p2)
	{
      return p1.zeile == p2.zeile && p1.spalte == p2.spalte
         && p1.yoffset == p2.yoffset && p1.xoffset == p2.xoffset;
      }

//---------------------------------------------------------
//    KontextListList::append_kontext
//---------------------------------------------------------

void KontextListList::append_kontext(Kontext* c)
      {
      (*cc)->append(c);
      }

//---------------------------------------------------------
//   KontextListList::clear_all
//---------------------------------------------------------

void KontextListList::clear_all()
      {
      for (iKontextListList cc = begin(); cc != end(); ++cc) {
            KontextList* kl = *cc;
            for (iKontextList i = kl->begin(); i != kl->end(); ++i) {
                  delete *i;
                  }
            kl->clear();
            }
      clear();
      files.clear();
      }

void KontextList::setcur(int n)
      {
      (*_cont)->setOpenflag(false);
      _cont = at(n);
      }
void KontextList::back()
      {
      if (!first_kontext()) {
            (*_cont)->setOpenflag(false);
            --_cont;
            }
      }
void KontextList::vor()
      {
      if (!last_kontext()) {
            (*_cont)->setOpenflag(false);
            ++_cont;
            }
      }
void KontextList::append(Kontext* k)
	{
      if (_cont != end()) {
            (*_cont)->setOpenflag(false);
            ++_cont;
            }
      erase(_cont, end());
      push_back(k);
	_cont = --end();
	}

bool KontextListList::backhist_ok() const
      {
      return !((*cc)->first_kontext());
      }

bool KontextListList::vorhist_ok() const
      {
      return !((*cc)->last_kontext());
      }

void KontextListList::backhist()
      {
      (*cc)->back();
      }

void KontextListList::vorhist()
      {
      (*cc)->vor();
      }

void KontextListList::setcur(int n)
      {
      (***cc)->setOpenflag(false);
      cc = at(n);
      }

void KontextListList::setcur(int n, int k)
      {
      (***cc)->setOpenflag(false);
      cc = at(n);
      (*cc)->setcur(k);
      }

void KontextListList::setcur_hist(int i, int n)
      {
      iKontextListList l = at(i);
      (*l)->setcur(n);
      }

void Kontext::save(const char* p)
      {
      f->save(p);
      }

bool Kontext::undo_ok() const
      {
      return f->undo_ok();
      }

bool Kontext::redo_ok() const
      {
      return f->redo_ok();
      }

