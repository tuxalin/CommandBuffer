/* 
 * Copyright 2008 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_GL_sampler_hpp__
#define __Cg_GL_sampler_hpp__

namespace Cg {

# ifdef __sampler1D_hpp__
sampler1D Sampler1DFromGLTextureUnit(GLenum texUnit);
sampler1D Sampler1DFromGLTextureObject(GLuint texobj);
sampler1D Sampler1DFromGLActiveTexture();;
# endif

# ifdef __sampler1DARRAY_hpp__
sampler1DARRAY Sampler1DARRAYFromGLTextureUnit(GLenum texUnit);
sampler1DARRAY Sampler1DARRAYFromGLTextureObject(GLuint texobj);
sampler1DARRAY Sampler1DARRAYFromGLActiveTexture();
# endif

# ifdef __sampler2D_hpp__
sampler2D Sampler2DFromGLTextureUnit(GLenum texUnit);
sampler2D Sampler2DFromGLTextureObject(GLuint texobj);
sampler2D Sampler2DFromGLActiveTexture();
# endif

# ifdef __sampler2DARRAY_hpp__
sampler2DARRAY Sampler2DARRAYFromGLTextureUnit(GLenum texUnit);
sampler2DARRAY Sampler2DARRAYFromGLTextureObject(GLuint texobj);
sampler2DARRAY Sampler2DARRAYFromGLActiveTexture();
# endif

# ifdef __sampler3D_hpp__
sampler3D Sampler3DFromGLTextureUnit(GLenum texUnit);
sampler3D Sampler3DFromGLTextureObject(GLuint texobj);
sampler3D Sampler3DFromGLActiveTexture();
# endif

# ifdef __samplerRECT_hpp__
samplerRECT SamplerRECTFromGLTextureUnit(GLenum texUnit);
samplerRECT SamplerRECTFromGLTextureObject(GLuint texobj);
samplerRECT SamplerRECTFromGLActiveTexture();
# endif

# ifdef __samplerCUBE_hpp__
samplerCUBE SamplerCUBEFromGLTextureUnit(GLenum texUnit);
samplerCUBE SamplerCUBEFromGLTextureObject(GLuint texobj);
samplerCUBE SamplerCUBEFromGLActiveTexture();
# endif

} // namespace Cg

#endif // __Cg_GL_sampler_hpp__

