SET(LIB_NAME ElevatorRoom)
SET(PLUGIN_HEADERS
    ElevatorRoom.h
    AudioHandler.h
    ModelHandler.h
    OSCPack.h
)

ADD_LIBRARY(${LIB_NAME}
    "SHARED"
    ${PLUGIN_HEADERS}
    ElevatorRoom.cpp
    AudioHandler.cpp
    ModelHandler.cpp
    OSCPack.cpp
)

FIND_PACKAGE(OASCLIENT)

INCLUDE_DIRECTORIES(${OSG_INCLUDE_DIR}
			${OASCLIENT_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(${LIB_NAME} ${OSG_LIBRARIES}
			${OASCLIENT_LIBRARY})

ADD_CALVR_LIBRARIES(${LIB_NAME})

FIND_PACKAGE(Spacenav)
FIND_PACKAGE(FTD2XX)

INCLUDE_DIRECTORIES(${OSG_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${SPACENAV_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${FTD2XX_INCLUDE_DIR})

TARGET_LINK_LIBRARIES(${LIB_NAME} ${OSG_LIBRARIES})
TARGET_LINK_LIBRARIES(${LIB_NAME} ${SPACENAV_LIBRARY})
TARGET_LINK_LIBRARIES(${LIB_NAME} ${FTD2XX_LIBRARY})

INSTALL(TARGETS ${LIB_NAME} DESTINATION lib/plugins)
