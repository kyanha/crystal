#include <wx/wx.h>

//----------------- Processes CS frames ---------------------
class Pump : public wxTimer
{
public:
  csWeakRef<ModifiableTestFrame> s;
  Pump() { };
  virtual void Notify()
  {
    if (s) s->PushFrame();
  }
};