
# GNUmakefile for g++ 4.0

OBJ_SUFFIX := .o
DEP_SUFFIX := .P
DEP_FILES :=
DEP_DIR := .deps

SRCS = abs.cpp acos.cpp all.cpp any.cpp asin.cpp atan.cpp atan2.cpp ceil.cpp \
       clamp.cpp cos.cpp cosh.cpp cross.cpp degrees.cpp determinant.cpp \
       distance.cpp dot.cpp exp.cpp exp2.cpp faceforward.cpp floor.cpp \
       fmod.cpp frac.cpp fresnel.cpp frexp.cpp iostream.cpp \
       isfinite.cpp isinf.cpp isnan.cpp ldexp.cpp length.cpp lerp.cpp \
       lit.cpp log.cpp log10.cpp log2.cpp max.cpp min.cpp modf.cpp \
       mul.cpp normalize.cpp pow.cpp radians.cpp reflect.cpp \
       refract.cpp round.cpp rsqrt.cpp \
       sampler1D.cpp \
       sampler1DARRAY.cpp \
       sampler2D.cpp \
       sampler2DARRAY.cpp \
       sampler3D.cpp \
       samplerCUBE.cpp \
       samplerRECT.cpp \
       samplerBUF.cpp \
       sampler_state.cpp \
       sampler_gl.cpp \
       saturate.cpp \
       inverse.cpp \
       sign.cpp sin.cpp sincos.cpp sinh.cpp smoothstep.cpp sqrt.cpp \
       step.cpp tan.cpp tanh.cpp transpose.cpp trunc.cpp \
       floatToIntBits.cpp \
       floatToRawIntBits.cpp \
       intBitsToFloat.cpp \
       $(NULL)

OBJS = $(SRCS:.cpp=.o)

CXXFLAGS = -I../include

ARFLAGS := scr

TARGETS = libcg4cpp.a

$(FRC): $(TARGETS)

libcg4cpp.a : $(OBJS)
	@ echo "Archiving $@..."
	@ $(AR) $(ARFLAGS) $@ $(OBJS)

DEP_FILES += $(addsuffix $(DEP_SUFFIX),\
                         $(addprefix $(DEP_DIR)/,$(notdir $(SRCS))))

CC_GEN_DEPS = -Wp,-MD,$(DEP_DIR)/$(<F).pp
CC_FIX_DEPS = -sed -e 's;$(*F)\.o:;$@:;' $(DEP_DIR)/$(<F).pp \
                    > $(DEP_DIR)/$(<F).P; \
               tr ' ' '\012' < $(DEP_DIR)/$(<F).pp \
                 | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' \
                       -e 's/$$/ :/' \
                 >> $(DEP_DIR)/$(<F).P; \
               rm $(DEP_DIR)/$(<F).pp

CPP_GEN_DEPS = -Wp,-MD,$(DEP_DIR)/$(<F).pp
CPP_FIX_DEPS = -sed -e 's;$(<F)\.o:;$@:;' $(DEP_DIR)/$(<F).pp \
                    > $(DEP_DIR)/$(<F).P; \
                tr ' ' '\012' < $(DEP_DIR)/$(<F).pp \
                  | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' \
                        -e 's/$$/ :/' \
                  >> $(DEP_DIR)/$(<F).P; \
                rm $(DEP_DIR)/$(<F).pp
RM = -rm -rf

clean:
	$(RM) $(OBJS)
	$(RM) $(TARGETS)

clobber: clean
	$(RM) $(DEP_DIR)

# make sure DEP_DIR is always created
DEP_DIR_MAGIC := $(shell mkdir -p $(DEP_DIR))
aDEP_DIR_MAGIC := $(shell echo -p $(DEP_DIR))

%$(OBJ_SUFFIX): %.cpp
	echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(CC_GEN_DEPS)
	$(CC_FIX_DEPS)

ifndef VERBOSE
.SILENT:
endif

-include $(sort $(DEP_FILES))
