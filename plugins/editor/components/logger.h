/*
    Copyright (C) 2012 by Christian Van Brussel

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#ifndef __COMPONENTS_LOGGER_H__
#define __COMPONENTS_LOGGER_H__

#include "csutil/scf_implementation.h"
#include "csutil/stringarray.h"
#include "csutil/threadmanager.h"
#include "ieditor/components.h"
#include "ieditor/editor.h"
#include "ieditor/space.h"
#include "ivaria/reporter.h"

#include <wx/datetime.h>

#include <wx/log.h>
#include <wx/textctrl.h>

using namespace CS::EditorApp;
using namespace CS::EditorApp::Component;

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

class Logger
  : public ThreadedCallable<Logger>,
    public scfImplementation3<Logger, iEditorComponent, iLogger, iReporterListener>
{
public:
  Logger (iBase* parent);
  virtual ~Logger ();

  //-- iEditorComponent
  virtual bool Initialize (iEditor* editor);
  virtual void Update ();
  virtual void Save (iDocumentNode* node) const;
  virtual bool Load (iDocumentNode* node);

  //-- iLogger
  virtual void SetMaximumReportCount (size_t count);
  virtual size_t GetMaximumReportCount () const;
  virtual wxTextCtrl* CreateTextCtrl (wxWindow* parent);

  //-- ThreadedCallable
  iObjectRegistry* GetObjectRegistry() const { return object_reg; }

  //-- iReporterListener
  THREADED_CALLABLE_DECL4 (Logger, Report, csThreadReturn,
    iReporter*, reporter, int, severity, const char*, msgId, const char*,
    description, HIGH, false, false);

private:
  bool ReportError (const char* description, ...) const
  {
    va_list arg;
    va_start (arg, description);
    csReportV (object_reg, CS_REPORTER_SEVERITY_ERROR,
	       "crystalspace.editor.component.logger",
	       description, arg);
    va_end (arg);
    return false;
  }

  bool ReportWarning (const char* description, ...) const
  {
    va_list arg;
    va_start (arg, description);
    csReportV (object_reg, CS_REPORTER_SEVERITY_WARNING,
	       "crystalspace.editor.component.logger",
	       description, arg);
    va_end (arg);
    return false;
  }

private:
  iObjectRegistry* object_reg;
  iEditor* editor;

  struct ReportEntry
  {
    wxDateTime timeinfo;
    int severity;
    csString msgId;
    csString description;

    ReportEntry (const wxDateTime& timeinfo, int severity,
		 const char* msgId, const char* description)
    : timeinfo (timeinfo), severity (severity), msgId (msgId),
      description (description)
      {
	this->msgId.Trim ();
	this->description.Trim ();
      }
  };
  csArray<ReportEntry> reports;
  size_t maxReportCount;

  class LogTextCtrl : public wxTextCtrl
  {
  public:
    LogTextCtrl (wxWindow* parent, Logger* logger);
    ~LogTextCtrl ();

  private:
    void OnEnterWindow (wxMouseEvent& event);

  private:
    Logger* logger;

    DECLARE_EVENT_TABLE ();
  };
  csArray<wxTextCtrl*> textCtrls;

  csStringArray errorMessages;
  wxString errorText;
  const wxString& FormatReport (ReportEntry& report);
};

}
CS_PLUGIN_NAMESPACE_END (CSEditor)

#endif // __COMPONENTS_LOGGER_H__
