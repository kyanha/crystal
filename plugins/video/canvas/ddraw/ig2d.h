#ifndef __IDD3G2D_H__
#define __IDD3G2D_H__

#include <ddraw.h>
#include "csutil/scf.h"
#include "cssys/win32/iddetect.h"

/// iGraphics2DDDraw3 interface -- for win32 DDraw-specific properties.
SCF_INTERFACE (iGraphics2DDDraw3, 0, 0, 1) : public iBase
{
  ///
  virtual void GetDirectDrawDriver (LPDIRECTDRAW* lplpDirectDraw) = 0;
  ///
  virtual void GetDirectDrawPrimary (LPDIRECTDRAWSURFACE* lplpDirectDrawPrimary) = 0;
  ///
  virtual void GetDirectDrawBackBuffer (LPDIRECTDRAWSURFACE* lplpDirectDrawBackBuffer) = 0;
  ///
  virtual void GetDirectDetection (IDirectDetectionInternal** lplpDDetection) = 0;
  ///
  virtual HRESULT SetColorPalette () = 0;
};

#endif
