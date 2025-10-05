# add the library sources
set(
    headers
        QOsc
        qosc.h

        qosc_global.h

        QOscInterface
        qoscinterface.h

        QOscTcpInterface
        qosctcpinterface.h

        QOscUdpInterface
        qoscudpinterface.h

        QOscValue
        qoscvalue.h

        QOscMessage
        qoscmessage.h

        QOscBundle
        qoscbundle.h

        QOscMethod
        qoscmethod.h
)

add_library(QOsc
    SHARED
        ${headers}
        qosc_global.cpp

        qoscinterface.cpp

        qosctcpinterface.cpp

        qoscudpinterface.cpp

        qoscvalue.cpp
        qoscvalue_p.h

        qoscmessage.cpp
        qoscmessage_p.h

        qoscbundle.cpp
        qoscbundle_p.h

        qoscmethod.cpp
)

# define library target
target_link_libraries(
    QOsc
    PUBLIC
        Qt::Core
        Qt::Gui
        Qt::Network
)
target_include_directories(
    QOsc
    PUBLIC
        $<INSTALL_INTERFACE:${include_dest}>

    PRIVATE
        ${${Qt}Core_PRIVATE_INCLUDE_DIRS}
)
target_compile_definitions(QOsc PRIVATE QOSC_LIBRARY)
target_compile_definitions(QOsc PRIVATE SOFT_VERSION=${PROJECT_VERSION})
target_compile_definitions(QOsc PRIVATE GIT_VERSION=${GIT_VERSION})

# install
install(TARGETS QOsc EXPORT QOsc DESTINATION ${lib_dest})
install(FILES ${headers} DESTINATION ${include_dest})
