/*
    Copyright (C) 2001 by Brandon Ehle <azverkan@yahoo.com>

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

#include "cal3d/cal3d.h"
#include <stdio.h>
#include <stdarg.h>

#include "qsqrt.h"

void ifprintf(FILE *f, int indent, const char *fmt, ...) {
  for(int i=0; i<indent; i++) {
    fputc(' ', f);
  }

  va_list va;
  va_start(va, fmt);
  vfprintf(f, fmt, va);
  va_end(va);
}

void StripExt(char *dst, const char *src) {
  strcpy(dst, src);
  char *tmp=strrchr(dst, '.');
  if(tmp)
    *tmp=0;
}

void ReplaceExt(char *dst, const char *src, const char *ext) {
  StripExt(dst, src);
  strcat(dst, ext);
}

void ToLower(char *dst, const char *src) {
  char *d=dst;
  const char *s=src;
  for(; *s; s++, d++) {
    *d=tolower(*s);
  }
  *d=0;
}

void WriteBone(FILE *f, int ind, float scale, CalCoreSkeleton *skel, CalCoreBone *bone) {
  ifprintf(f, ind, "LIMB '%s' (\n", bone->getName().c_str());
#if 1
  const CalVector &pos=bone->getTranslation();
  const CalQuaternion &rot=bone->getRotation();
  ifprintf(f, ind, "TRANSFORM(V(%f,%f,%f)Q(%f,%f,%f,%f))\n", pos.x*scale, pos.y*scale, pos.z*scale, rot.x, rot.y, rot.z, rot.w);
#endif

  int *ptr=(int*)bone->getUserData();
  if(ptr) {
    ifprintf(f, ind, "VERTICES (");
    for(int i=0; i<ptr[0]; i++) {
      fprintf(f, "%d,", ptr[i+1]);
    }
    fprintf(f, ")\n");
  }

  std::list<int>::iterator iteratorChildId;
  for(iteratorChildId = bone->getListChildId().begin(); iteratorChildId != bone->getListChildId().end(); ++iteratorChildId)
  {
    CalCoreBone *child=skel->getCoreBone(*iteratorChildId);
    WriteBone(f, ind+1, scale, skel, child);
  }

  ifprintf(f, ind, ")\n");
}

void CalQuatInverse(CalQuaternion &qdest, const CalQuaternion &qin) {
  float inverselen=1.0f / qsqrt(qin[0]*qin[0]+qin[1]*qin[1]+qin[2]*qin[2]+qin[3]*qin[3]);
  qdest[0]=-inverselen*qin[0];
  qdest[1]=-inverselen*qin[1];
  qdest[2]=-inverselen*qin[2];
  qdest[3]=inverselen*qin[3];
}

int ExportSprite(const char* filename, float scale, float timescale, CalCoreModel &calCoreModel, CalModel & calModel) {
  char name[255];
  StripExt(name, filename);

  FILE *f=fopen(filename, "w");
  int ind=0;
  ifprintf(f, ind++, "LIBRARY 'soldier' (\n");

  ifprintf(f, ind++, "PLUGINS (\n");
  ifprintf(f, ind, "   PLUGIN 'sprite_3d_factory' ('crystalspace.mesh.loader.factory.sprite.3d')\n");
  ifprintf(f, --ind, ")\n");

//Write Texture Information
  ifprintf(f, ind++, "TEXTURES (\n");
  int i;
  for(i=0; i<calCoreModel.getCoreMaterialCount(); i++) {
    CalCoreMaterial *mat=calCoreModel.getCoreMaterial(i);
    for(int j=0; j<mat->getMapCount(); j++) {
      char texfile[255];
      ToLower(texfile, mat->getMapFilename(j).c_str());
      char texname[255];
      StripExt(texname, mat->getMapFilename(j).c_str());
      ifprintf(f, ind, "TEXTURE 'T_%s' (FILE (/lev/flarge/%s))\n", texname, texfile);
    }
  }
  ifprintf(f, --ind, ")\n");

//Write Material Information
  ifprintf(f, ind++, "MATERIALS (\n");
  for(i=0; i<calCoreModel.getCoreMaterialCount(); i++) {
    CalCoreMaterial *mat=calCoreModel.getCoreMaterial(i);
    const char *texfile=mat->getMapFilename(0).c_str();
    char texname[255];
    StripExt(texname, texfile);
    ifprintf(f, ind, "MATERIAL 'M_%s' (TEXTURE ('T_%s'))\n", texname, texname);
  }
  ifprintf(f, --ind, ")\n");

  CalCoreMesh *mesh=calCoreModel.getCoreMesh(0);
  if(!mesh) {
    printf("Core Mesh not found!\n");
    return 1;
  }
  CalCoreSubmesh *submesh=mesh->getCoreSubmesh(0);
  if(!submesh) {
    printf("Core Submesh not found!\n");
    return 1;
  }

//Write Mesh Information
  ifprintf(f, ind++, "MESHFACT '%s' (\n", name);
  ifprintf(f, ind, "PLUGIN ('sprite_3d_factory')\n");
  ifprintf(f, ind++, "PARAMS (\n");

  char texname[255];
  StripExt(texname, calCoreModel.getCoreMaterial(0)->getMapFilename(0).c_str());
  ifprintf(f, ind, "MATERIAL ('M_%s')\n", texname);

  calModel.update(1);

//Write Vertices
  ifprintf(f, ind++, "FRAME 'base' (\n");
  for(i=0; i<submesh->getVertexCount(); i++) {
    CalCoreSubmesh::Vertex *v=&submesh->getVectorVertex()[i];
    CalCoreSubmesh::TextureCoordinate *t=&submesh->getVectorVectorTextureCoordinate()[0][i];

//CS doesn't support per-vertex influences yet, so pick the biggest one
    CalCoreSubmesh::Influence *useinf=NULL;
    float useweight=0.0f;
    for(unsigned int j=0; j<v->vectorInfluence.size(); j++) {
      CalCoreSubmesh::Influence *i=&v->vectorInfluence[j];
      if(i->weight>=useweight) {
        useweight=i->weight;
        useinf=i;
      }
    }

//Store xyz
#if 1 //TODO Azverkan this is for trying to export the model in its "standard pose"
    CalBone *bone=calModel.getSkeleton()->getVectorBone()[useinf->boneId];
    CalVector pos(v->position);
    pos *= bone->getCoreBone()->getRotationBoneSpace();
    pos += bone->getCoreBone()->getTranslationBoneSpace();

    ifprintf(f, ind, "V(%f,%f,%f:%f,%f)\n", pos.x*scale, pos.y*scale, pos.z*scale, t->u, 1.0f-t->v);
#else
    ifprintf(f, ind, "V(%f,%f,%f:%f,%f)\n", v->position.x*scale, v->position.y*scale, v->position.z*scale, t->u, 1.0f-t->v);
#endif

//Add to bone list
    CalCoreBone *coreBone=calCoreModel.getCoreSkeleton()->getVectorCoreBone()[useinf->boneId];
    int *ptr=(int*)coreBone->getUserData();
    if(!ptr) {
      ptr=(int*)malloc((submesh->getVertexCount()+1)*sizeof(int)); //SEMI-HARDCODED
      coreBone->setUserData(ptr);
      ptr[0]=0; //Store size in ptr[0]
    }
    ptr[++ptr[0]]=i; //Append vertex to list
  }
  ifprintf(f, --ind, ")\n");

  ifprintf(f, ind, "ACTION 'default' (F (base,1000))\n");

//Write faces
  for(i=0; i<submesh->getFaceCount(); i++) {
    CalCoreSubmesh::Face *t=&submesh->getVectorFace()[i];
    ifprintf(f, ind, "TRIANGLE (%d,%d,%d)\n",t->vertexId[0],t->vertexId[1],t->vertexId[2]);
  }

//Write skeleton
  CalCoreSkeleton* skel=calCoreModel.getCoreSkeleton();
  if(skel->getListRootCoreBoneId().size()!=1) {
    printf("Skeleton needs just one root bone\n");
    return 1;
  }
  CalCoreBone* root=skel->getVectorCoreBone()[0];
  ifprintf(f, ind++, "SKELETON 'S_%s' (\n", root->getName().c_str());
  WriteBone(f, ind, scale, skel, root);
  ifprintf(f, --ind, ")\n");

  ifprintf(f, --ind, ")\n");
  ifprintf(f, --ind, ")\n");

//Write animation
  char motname[255];
  StripExt(motname, filename);
  ifprintf(f, ind++, "ADDON (\n");
  ifprintf(f, ind, "PLUGIN ('crystalspace.motion.loader.default')\n");
  ifprintf(f, ind++, "PARAMS (\n");
  for(i=0; i<calCoreModel.getCoreAnimationCount(); i++) {
    CalCoreAnimation *anim=calCoreModel.getCoreAnimation(i);
    ifprintf(f, ind++, "MOTION '%s_%d' (\n", motname, i);
    ifprintf(f, ind, "DURATION '%f' (LOOP())\n", anim->getDuration()*timescale);

    std::list<CalCoreTrack *>& listCoreTrack = anim->getListCoreTrack();
    std::list<CalCoreTrack *>::iterator iteratorCoreTrack;
    for(iteratorCoreTrack = listCoreTrack.begin(); iteratorCoreTrack != listCoreTrack.end(); ++iteratorCoreTrack) {
      int boneId=(*iteratorCoreTrack)->getCoreBoneId();

      CalBone *bone=calModel.getSkeleton()->getVectorBone()[boneId];
      CalCoreBone *coreBone = calCoreModel.getCoreSkeleton()->getCoreBone(boneId);
      assert(bone->getCoreBone()==coreBone);

      ifprintf(f, ind++, "BONE '%s' (\n", coreBone->getName().c_str());

      std::map<float, CalCoreKeyframe *>& mapCoreKeyframe = (*iteratorCoreTrack)->getMapCoreKeyframe();
      std::map<float, CalCoreKeyframe *>::iterator iteratorCoreKeyframe;
      for(iteratorCoreKeyframe = mapCoreKeyframe.begin(); iteratorCoreKeyframe != mapCoreKeyframe.end(); ++iteratorCoreKeyframe)
      {
        CalCoreKeyframe* keyframe=iteratorCoreKeyframe->second;
        CalQuaternion q;
	CalVector v;
	float time=keyframe->getTime()*timescale;

#if 0 //TODO Azverkan this is for trying to export the model in its "standard pose"
//	SkeletonCalToCrystal(q, keyframe->getRotation(), bone->getRotation(), v, keyframe->getTranslation(), bone->getTranslation());

	v = coreBone->getTranslationBoneSpace();
	v *= keyframe->getRotation();
	v += keyframe->getTranslation();

	q = coreBone->getRotationBoneSpace();
	q *= keyframe->getRotation();
#else
	q=keyframe->getRotation();
	v=keyframe->getTranslation();
#endif
	v.x*=scale;
	v.y*=scale;
	v.z*=scale;

        ifprintf(f, ind, "FRAME '%f' (ROT(Q(%f,%f,%f,%f)) POS(%f,%f,%f))\n",time,q.x,q.y,q.z,q.w,v.x,v.y,v.z);
      }

      ifprintf(f, --ind, ")\n");
    }

    ifprintf(f, --ind, ")\n");
  }
  ifprintf(f, --ind, ")\n");

  ifprintf(f, --ind, ")\n");
  fclose(f);
  return 0;
}

int ConvertModel(const char *filename) {
  FILE *f=fopen(filename, "r");
  if(!f) {
    printf("Could not read config file %s\n", filename);
    return 1;
  }

  // create a core model instance
  CalCoreModel calCoreModel;
  if(!calCoreModel.create(filename))
  {
    CalError::printLastError();
    return 1;
  }

  float scale=1.0f;
  float timescale=1.0f;

  while(1) {
    char buf[1000];
    if(!fgets(buf, 1000, f))
      break;

    if(buf[0]=='#') //skip comments
      continue;

    char *cmd, *param;
    for(cmd=buf; *cmd==32; cmd++) {} //skip whitespace

    param=strchr(cmd, '=');
    if(!param)
      continue;

    *(param++)=0;
    for(;*param==32; param++) {} //skip whitespace
    size_t s=strcspn(param, "\r\n");
    param[s]=0;

    if(strcmp(cmd, "geomscale")==0) {
      scale=atof(param);
      printf("Scale Geom to %f\n", scale);
    } else if(strcmp(cmd, "timescale")==0) {
      timescale=atof(param);
      printf("Scale Time to %f\n", timescale);
    } else if(strcmp(cmd, "skeleton")==0) {
      if(!calCoreModel.loadCoreSkeleton(param)) {
        CalError::printLastError();
	calCoreModel.destroy();
        return 1;
      }
      printf("Load skeleton %s\n", param);
    } else if(strcmp(cmd, "animation")==0) {
      if(calCoreModel.loadCoreAnimation(param)==-1) {
        CalError::printLastError();
	calCoreModel.destroy();
        return 1;
      }
      printf("Load animation %s\n", param);
    } else if(strcmp(cmd, "mesh")==0) {
      if(calCoreModel.loadCoreMesh(param)==-1) {
        CalError::printLastError();
	calCoreModel.destroy();
        return 1;
      }
      printf("Load mesh %s\n", param);
    } else if(strcmp(cmd, "material")==0) {
      if(calCoreModel.loadCoreMaterial(param)==-1) {
        CalError::printLastError();
	calCoreModel.destroy();
        return 1;
      }
      printf("Load material %s\n", param);
    }
  }
  fclose(f);
  f=NULL;

  //Verify there is data to export
  if(!calCoreModel.getCoreMeshCount()) {
    printf("No meshes to export for %s\n", filename);
    calCoreModel.destroy();
    return 1;
  }

  printf("Now converting\n");
  CalModel calModel;
  if(!calModel.create(&calCoreModel)) {
    CalError::printLastError();
    calCoreModel.destroy();
    return 1;
  }

  char outname[255];
  ReplaceExt(outname, filename, ".csk");

  printf("Now exporting %s\n", outname);
  int ret=ExportSprite(outname, scale, timescale, calCoreModel, calModel);
  calModel.destroy();
  calCoreModel.destroy();
  return ret;
}

int main(int argc, char *argv[]) {

  int i=1;
  for(;i<argc; i++) {
    if(argv[i][0]!='-')
      break;
  }

  if(i>=argc) {
    printf("caltocs: Nothing to do!\n");
    return 1;
  }

  for(;i<argc; i++) {
    ConvertModel(argv[i]);
  }

  return 0;
}

