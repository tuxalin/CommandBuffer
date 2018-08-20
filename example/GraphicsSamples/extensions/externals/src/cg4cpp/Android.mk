LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libcg4cpp
LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../include

ifneq ($(BUILD_FOR_NDK), true)
	LOCAL_C_INCLUDES += \
		$(TOP)/external/stlport/stlport \
		$(TOP)/bionic
endif

LOCAL_SRC_FILES := \
	abs.cpp acos.cpp all.cpp any.cpp asin.cpp atan.cpp atan2.cpp ceil.cpp \
	clamp.cpp cos.cpp cosh.cpp cross.cpp degrees.cpp determinant.cpp \
	distance.cpp dot.cpp exp.cpp exp2.cpp faceforward.cpp floor.cpp \
	fmod.cpp frac.cpp fresnel.cpp frexp.cpp iostream.cpp \
	isfinite.cpp isinf.cpp isnan.cpp ldexp.cpp length.cpp lerp.cpp \
	lit.cpp log.cpp log10.cpp log2.cpp max.cpp min.cpp modf.cpp \
	mul.cpp normalize.cpp pow.cpp radians.cpp reflect.cpp \
	refract.cpp round.cpp rsqrt.cpp \
	saturate.cpp \
	inverse.cpp \
	sign.cpp sin.cpp sincos.cpp sinh.cpp smoothstep.cpp sqrt.cpp \
	step.cpp tan.cpp tanh.cpp transpose.cpp trunc.cpp \
	floatToIntBits.cpp \
	floatToRawIntBits.cpp \
	intBitsToFloat.cpp

# Sources that use <GL/gl.h>
#	sampler1D.cpp
#	sampler1DARRAY.cpp
#	sampler2D.cpp
#	sampler2DARRAY.cpp
#	sampler3D.cpp
#	samplerBUF.cpp
#	samplerCUBE.cpp
#	samplerRECT.cpp
#	sampler_state.cpp
#	sampler_gl.cpp

ifeq ($(BUILD_FOR_NDK), true)
	LOCAL_CFLAGS := \
		-mfpu=neon
endif

include $(BUILD_STATIC_LIBRARY)
