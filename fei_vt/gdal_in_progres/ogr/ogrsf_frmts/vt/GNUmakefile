

include ../../../GDALmake.opt

# ifeq ($(LIBJSONC_SETTING),internal)
# SUBDIRS-yes	:= libjson
# endif


# OBJ	=	\
# 		kvfactory.o \
# 		ogrvtdriver.o \
# 		ogrvtdatasource.o \
# 		ogrvtlayer.o \
# 		ogrgeojsonreader.o \
# 		filekv.o \
# 		hashtable.o \
# 		tileid.o \
# 		vectortile.o \
# 		ogrvtlayer_add.o \
# 		geojsonvectortile.o \
# 		rediskv.o \
# 		HadoopKV.o \
# 		TileCalculator.o 
		
OBJ	=	\
		ogrvtdriver.o \
		tileid.o \
		metadata.o \
		ogrvtdatasource.o \
		ogrvtlayer.o \
		ogrvtlayer_add.o \
		vectortile.o \
		test_tileid.o \


# CPPFLAGS	:= $(JSON_INCLUDE) -I. -I.. -I../.. $(GDAL_INCLUDE) $(CPPFLAGS)
CPPFLAGS	:= -I. -I.. -I../.. $(GDAL_INCLUDE) $(CPPFLAGS)

# default:	$(O_OBJ:.o=.$(OBJ_EXT))

default:        $(foreach d,$(SUBDIRS-yes),$(d)-target) $(O_OBJ:.o=.$(OBJ_EXT))

clean: $(foreach d,$(SUBDIRS-yes),$(d)-clean)
	rm -f *.o $(O_OBJ)
	rm -f *~

$(O_OBJ):	ogr_vt.h

# clean:
# 	rm -f *.o $(O_OBJ)




# default:        $(foreach d,$(SUBDIRS-yes),$(d)-target) $(O_OBJ:.o=.$(OBJ_EXT))

# clean: $(foreach d,$(SUBDIRS-yes),$(d)-clean)
# 	rm -f *.o $(O_OBJ)
# 	rm -f *~

# $(O_OBJ):       ogr_geojson.h
