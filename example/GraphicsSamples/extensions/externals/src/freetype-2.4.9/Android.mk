LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

FREETYPE_SRC_PATH := $(LOCAL_PATH)/

LOCAL_ARM_MODE := arm
LOCAL_MODULE := gs_ft2
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS := -DANDROID_NDK \
		-DFT2_BUILD_LIBRARY=1 \
		-DHAVE_STDINT_H \
		-mfpu=neon \
		-mcpu=cortex-a9

LOCAL_C_INCLUDES := \
		$(FREETYPE_SRC_PATH)include \
		$(FREETYPE_SRC_PATH)src

LOCAL_SRC_FILES := \
		src/autofit/autofit.c \
		src/bdf/bdf.c \
		src/base/basepic.c \
		src/base/ftapi.c \
		src/base/ftbase.c \
		src/base/ftbbox.c \
		src/base/ftbitmap.c \
		src/base/ftdbgmem.c \
		src/base/ftdebug.c \
		src/base/ftfstype.c \
		src/base/ftgasp.c \
		src/base/ftglyph.c \
		src/base/ftgxval.c \
		src/gzip/ftgzip.c \
		src/base/ftinit.c \
		src/base/ftlcdfil.c \
		src/lzw/ftlzw.c \
		src/base/ftmm.c \
		src/base/ftotval.c \
		src/base/ftpatent.c \
		src/base/ftpfr.c \
		src/base/ftpic.c \
		src/base/ftstroke.c \
		src/base/ftsynth.c \
		src/base/ftsystem.c \
		src/base/fttype1.c \
		src/base/ftxf86.c \
		src/cff/cff.c \
		src/pcf/pcf.c \
		src/pfr/pfr.c \
		src/psaux/psaux.c \
		src/pshinter/pshinter.c \
		src/psnames/psnames.c \
		src/psnames/psmodule.c \
		src/raster/raster.c \
		src/sfnt/sfnt.c \
		src/smooth/smooth.c \
		src/truetype/truetype.c \
		src/type1/type1.c \
		src/cid/type1cid.c


LOCAL_LDLIBS := -ldl -llog

include $(BUILD_STATIC_LIBRARY)
