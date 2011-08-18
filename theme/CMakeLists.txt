
set(DATA_FILES
    beijing.png
    ch_beijing.png
    gtkrc
    writer.png
)

install(FILES ${DATA_FILES} DESTINATION ${DATADIR}/fcitx/handwrite)

add_subdirectory(Buttons)