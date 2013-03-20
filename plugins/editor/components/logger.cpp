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

#include "cssysdef.h"
#include "csutil/scf.h"
#include "ieditor/context.h"

#include "logger.h"

#include <wx/frame.h>

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

SCF_IMPLEMENT_FACTORY (Logger)

Logger::Logger (iBase* parent)
  : scfImplementationType (this, parent), maxReportCount (100)
{
  errorMessages.Push ("Bug");
  errorMessages.Push ("Error");
  errorMessages.Push ("Warning");
  errorMessages.Push ("Notify");
  errorMessages.Push ("Debug");
}

Logger::~Logger()
{
}

bool Logger::Initialize (iEditor* editor)
{
  this->editor = editor;
  object_reg = editor->GetContext ()->GetObjectRegistry ();

  // Register as a reporter listener
  csRef<iReporter> reporter = csQueryRegistry<iReporter> (object_reg);
  reporter->AddReporterListener (this);

  // Put down the login level of wx in order to display only errors
  wxLog::GetActiveTarget ()->SetLogLevel (wxLOG_Error);

  return true;
}

void Logger::Update ()
{
}

void Logger::Save (iDocumentNode* node) const
{
}

bool Logger::Load (iDocumentNode* node)
{
  return false;
}

void Logger::SetMaximumReportCount (size_t count)
{
  maxReportCount = count;
}

size_t Logger::GetMaximumReportCount () const
{
  return maxReportCount;
}

wxTextCtrl* Logger::CreateTextCtrl (wxWindow* parent)
{
  wxTextCtrl* textCtrl = new Logger::LogTextCtrl (parent, this);
  textCtrl->SetAutoLayout (true);

  for (size_t i = 0; i < reports.GetSize (); i++)
  {
    const wxString& text = FormatReport (reports[i]);
    textCtrl->AppendText (text);
  }

  return textCtrl;
}

THREADED_CALLABLE_IMPL4 (Logger, Report, iReporter*, int severity,
  const char* msgId, const char* description)
{
  // Read the timestamp of the report
  wxDateTime timeinfo = wxDateTime::Now ();

  // Forward the report to the wx login system
  switch (severity)
  {
  case CS_REPORTER_SEVERITY_BUG:
    wxLogError (wxString::FromUTF8 (csString ().Format ("%s - %s", msgId, description)));
    break;
  case CS_REPORTER_SEVERITY_ERROR:
    wxLogError (wxString::FromUTF8 (csString ().Format ("%s - %s", msgId, description)));
    break;
  case CS_REPORTER_SEVERITY_WARNING:
    wxLogWarning (wxString::FromUTF8 (csString ().Format ("%s - %s", msgId, description)));
    break;
  case CS_REPORTER_SEVERITY_NOTIFY:
    wxLogMessage (wxString::FromUTF8 (csString ().Format ("%s - %s", msgId, description)));
    break;
  case CS_REPORTER_SEVERITY_DEBUG:
    wxLogDebug (wxString::FromUTF8 (csString ().Format ("%s - %s", msgId, description)));
    break;
  }

  // Add this report to the list of stored reports
  ReportEntry report (timeinfo, severity, msgId, description);
  reports.Push (report);
  if (maxReportCount && reports.GetSize () > maxReportCount)
    reports.DeleteIndex (0);

  // Add this report to all text logger instances
  const wxString& text = FormatReport (report);
  for (size_t i = 0; i < textCtrls.GetSize (); i++)
    textCtrls[i]->AppendText (text);

  return true;
}

const wxString& Logger::FormatReport (ReportEntry& report)
{
  errorText = report.timeinfo.FormatTime ();

  csString text;
  if (report.msgId == "status")
    text.Format (" - [Status] %s\n",
		 report.description.GetData ());

  else text.Format (" - [%s] %s - %s\n",
		    errorMessages[report.severity],
		    report.msgId.GetData (),
		    report.description.GetData ());
  errorText += wxString::FromUTF8 (text.GetData ());

  return errorText;
}

//---------------------------------------------------------------

BEGIN_EVENT_TABLE (Logger::LogTextCtrl, wxTextCtrl)
  EVT_ENTER_WINDOW (Logger::LogTextCtrl::OnEnterWindow)
END_EVENT_TABLE ()

Logger::LogTextCtrl::LogTextCtrl (wxWindow* parent, Logger* logger)
  : wxTextCtrl (parent, -1, wxT (""), wxDefaultPosition, wxDefaultSize,
		wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL),
  logger (logger)
{
  logger->textCtrls.Push (this);
}

Logger::LogTextCtrl::~LogTextCtrl ()
{
  logger->textCtrls.Delete (this);
}

void Logger::LogTextCtrl::OnEnterWindow (wxMouseEvent& event)
{
  // Put back the focus on this window
  //wxTopLevelWindow* window = dynamic_cast<wxTopLevelWindow*> (GetGrandParent ());
  //if (window && window->IsActive ()) SetFocus ();
  SetFocus ();
}

}
CS_PLUGIN_NAMESPACE_END (CSEditor)

