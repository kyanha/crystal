%module cspace
%{
  #include "css.h"
//  #include "ispawn.h"

//***** SCF Wrappers
  int MakeVersion(int version0, int version1, int version2) {
    return SCF_CONSTRUCT_VERSION(version0, version1, version2);
  }

#include "isystem.h"
#include "csparser/csloader.h"
#include "cspython.h"
iSystem* GetSystem() {
	return thisclass->Sys;
}

void* GetMyPtr() { return NULL; }

%}

%include pointer.i

//***** SCF
struct iBase {};

struct iSCF:public iBase {
  void* scfCreateInstance(const char *iClassID, const char *iInterfaceID,
    int iVersion);
};
int MakeVersion(int version0, int version1, int version2);

//***** Classes
class csVector3 {
public:
  float x,y,z;

  csVector3(float x, float y, float z);
/*
  %addmethods {
    char *__str__() {
      static char temp[256];
      sprintf(temp,"[ %g, %g, %g ]", self->x, self->y, self->z);
      return temp;
    }
  }*/
};

struct csRGBpixel {
  unsigned char red, green, blue, alpha;
};

//***** Interfaces
struct iPlugIn:public iBase {
  bool Initialize(iSystem *iSys);
  bool HandleEvent(iEvent&);
};

struct iTextureHandle : public iBase
{
  bool GetMipMapDimensions (int mipmap, int &mw, int &mh);
  void GetMeanColor (UByte &red, UByte &green, UByte &blue);
  void *GetCacheData ();
  void SetCacheData (void *d);
  void *GetPrivateObject ();
};

struct iGraphics3D:public iPlugIn {
  bool Initialize (iSystem *pSystem);
  bool Open (const char *Title);
  void Close ();
  void SetDimensions (int width, int height);
  bool BeginDraw (int DrawFlags);
  void FinishDraw ();
  void Print (csRect *area);
  void DrawPolygon (G3DPolygonDP& poly);
  void DrawPolygonDebug (G3DPolygonDP& poly);
  void DrawLine (const csVector3& v1, const csVector3& v2,
    float fov, int color);
//  void StartPolygonFX (iTextureHandle* handle, UInt mode);
//  void FinishPolygonFX ();
//  void DrawPolygonFX (G3DPolygonDPFX& poly);
  void DrawTriangleMesh (G3DTriangleMesh& mesh);
  void DrawPolygonMesh (G3DPolygonMesh& mesh);
  void OpenFogObject (CS_ID id, csFog* fog);
  void DrawFogPolygon (CS_ID id, G3DPolygonDFP& poly, int fogtype);
  void CloseFogObject (CS_ID id);
//  bool SetRenderState (G3D_RENDERSTATEOPTION op, long val);
//  long GetRenderState (G3D_RENDERSTATEOPTION op);
  csGraphics3DCaps *GetCaps ();
  unsigned long *GetZBuffAt (int x, int y);
  float GetZBuffValue (int x, int y);
  void DumpCache ();
  void ClearCache ();
  void RemoveFromCache (iPolygonTexture* poly_texture);
  int GetWidth ();
  int GetHeight ();
  void SetPerspectiveCenter (int x, int y);
  void SetPerspectiveAspect (float aspect);
  void SetObjectToCamera (csReversibleTransform* o2c);
  void SetClipper (csVector2* vertices, int num_vertices);
  iGraphics2D *GetDriver2D ();
  iTextureManager *GetTextureManager ();
  iHalo *CreateHalo (float iR, float iG, float iB,
    unsigned char *iAlpha, int iWidth, int iHeight);
  void DrawPixmap (iTextureHandle *hTex, int sx, int sy, int sw, int sh,
    int tx, int ty, int tw, int th);
};

struct iCamera:public iBase {
  float GetAspect ();
  float GetInvAspect ();
};

struct iPolygonSet:public iBase {
  const char *GetName () const;
  void SetName (const char *iName);
  void CompressVertices ();
  int GetPolygonCount ();
  iPolygon3D *GetPolygon (int idx);
  iPolygon3D *CreatePolygon (const char *iName);
  int GetVertexCount ();
  csVector3 &GetVertex (int idx);
  csVector3 &GetVertexW (int idx);
  csVector3 &GetVertexC (int idx);
  int CreateVertex (csVector3 &iVertex);
  bool CreateKey (const char *iName, const char *iValue);
};

struct iSector:public iBase {
public:
  void CreateBSP();

  %addmethods {
    iPolygonSet* Query_iPolygonSet() {
      return QUERY_INTERFACE(self, iPolygonSet);
    }
  }
}

struct iThing:public iPolygonSet {
};

struct iPolygon3D : public iBase {
  const char *GetName () const;
  void SetName (const char *iName);
  iPolygonSet *GetContainer ();
  iLightMap *GetLightMap ();
  iMaterialHandle *GetMaterialHandle ();
  void SetMaterial (iMaterialWrapper* material);
//  iPolygonTexture *GetTexture ();
//  iTextureHandle *GetTextureHandle ();
  int GetVertexCount ();
  csVector3 &GetVertex (int idx);
  csVector3 &GetVertexW (int idx);
  csVector3 &GetVertexC (int idx);
  %name(CreateVertexByIndex) int CreateVertex (int idx);
  int CreateVertex (const csVector3 &iVertex);
  int GetAlpha ();
  void SetAlpha (int iAlpha);
  void CreatePlane (const csVector3 &iOrigin,
    const csMatrix3 &iMatrix);
  bool SetPlane (const char *iName);
  void SetTextureSpace (csVector3& v_orig, csVector3& v1, float len1);
};

struct iImage : public iBase
{
  void *GetImageData ();
  int GetWidth ();
  int GetHeight ();
  int GetSize ();
  void Rescale (int NewWidth, int NewHeight);
  iImage *MipMap (int step, csRGBpixel *transp);
  void SetName (const char *iName);
  const char *GetName ();
  int GetFormat ();
  csRGBpixel *GetPalette ();
  UByte *GetAlpha ();
  void SetFormat (int iFormat);
  iImage *Clone ();
  iImage *Crop (int x, int y, int width, int height);
};

struct iTextureManager : public iBase
{
  iTextureHandle *RegisterTexture (iImage *image, int flags);
  void UnregisterTexture (iTextureHandle *handle);
  void PrepareTexture (iTextureHandle *handle);
  void PrepareTextures ();
  void FreeImages ();
  void ResetPalette ();
  void ReserveColor (int r, int g, int b);
  int FindRGB (int r, int g, int b);
  void SetPalette ();
  void SetVerbose (bool vb);
  int GetTextureFormat ();
};

struct iPolygonTexture : public iBase
{
  iMaterialHandle *GetMaterialHandle ();
//  iTextureHandle *GetTextureHandle ();
  float GetFDU ();
  float GetFDV ();
  int GetWidth ();
  int GetHeight ();
  int GetShiftU ();
  int GetIMinU ();
  int GetIMinV ();
  void GetTextureBox (float& fMinU, float& fMinV,
    float& fMaxU, float& fMaxV);
  int GetOriginalWidth ();
  iPolygon3D *GetPolygon ();
  bool DynamicLightsDirty ();
  bool RecalculateDynamicLights ();
  iLightMap *GetLightMap ();
  int GetLightCellSize ();
  int GetLightCellShift ();
  void *GetCacheData (int idx);
  void SetCacheData (int idx, void *d);
};

struct iWorld:public iPlugIn {
  virtual int GetTextureFormat () = 0;
  virtual void SelectLibrary (const char *iName) = 0;
  virtual bool DeleteLibrary (const char *iName) = 0;
  virtual void DeleteAll () = 0;
  virtual bool CreateTexture (const char *iName, const char *iFileName, 
    csColor *iTransp, int iFlags) = 0;
  virtual bool CreateCamera (const char *iName, const char *iSector,
    const csVector3 &iPos, const csVector3 &iForward, const csVector3 &iUpward) = 0;
  virtual bool CreateKey (const char *iName, const char *iValue) = 0;
  virtual bool CreatePlane (const char *iName, const csVector3 &iOrigin,
    const csMatrix3 &iMatrix) = 0;
  virtual iSector *CreateSector (const char *iName) = 0;
  virtual iSector *FindSector (const char *iName) = 0;
  virtual iThing *CreateThing (const char *iName, iSector *iParent) = 0;
  virtual iMaterialWrapper *FindMaterial (const char *iName) = 0;
};

//****** System Interface
struct iSystem:public iBase {
public:
  %addmethods {
/*
    iSCF* GetSCF() {
      iSCF *scf = QUERY_INTERFACE (self, iSCF);
      scf->DecRef (); // ugly kludge
      return scf;
    }
  */
    iWorld* Query_iWorld() {
      return QUERY_PLUGIN(self, iWorld);
    }
    iGraphics3D* Query_iGraphics3D() {
      return QUERY_PLUGIN(self, iGraphics3D);
    }
  }
};

iSystem* GetSystem();
void* GetMyPtr();

//***** Spawner

//%include include/ispawn.h

