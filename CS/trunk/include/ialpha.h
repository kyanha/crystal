#include "cscom/com.h"
#include "types.h"

#ifndef IALPHA_H
#define IALPHA_H

extern const IID IID_IAlphaMapHandle;

/** 
 * A texture handle as returned by ITextureManager.
 */
interface IAlphaMapHandle : public IUnknown
{
};

#endif //IALPHA_H
