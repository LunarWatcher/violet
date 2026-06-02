set(ASSETS
    _default/violet.theme.json

    _default/_partials/footer.inja
    _default/_partials/head.inja
    _default/_partials/header.inja
    _default/_partials/paginator.inja
    _default/_partials/tree_builder.inja

    _default/_templates/_default/base.inja
    _default/_templates/_default/page_list.inja
    _default/_templates/_default/single_page.inja

    _default/assets/_default_page.js
    _default/assets/_default_style.css
    _default/assets/vendor/_default-highlightjs.js
)
foreach(ASSET IN LISTS ASSETS)
    message(STATUS "Copying ${ASSET}")
    configure_file(${ASSET} ${CMAKE_BINARY_DIR}/share/violet/themes/${ASSET} COPYONLY)
endforeach()

install(DIRECTORY "${CMAKE_BINARY_DIR}/share/" DESTINATION share)
