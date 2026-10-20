set(ASSETS
    _default/violet.theme.json

    _default/_partials/footer.iris
    _default/_partials/head.iris
    _default/_partials/header.iris
    _default/_partials/paginator.iris
    _default/_partials/tree_builder.iris

    _default/_templates/_default/base.iris
    _default/_templates/_default/feed.iris
    _default/_templates/_default/page_list.iris
    _default/_templates/_default/single_page.iris

    _default/assets/_default_page.js
    _default/assets/_default_style.css
    _default/assets/vendor/_default-highlightjs.js
)
foreach(ASSET IN LISTS ASSETS)
    message(STATUS "Copying ${ASSET}")
    configure_file(${ASSET} ${CMAKE_BINARY_DIR}/share/violet/themes/${ASSET} COPYONLY)
endforeach()

install(DIRECTORY "${CMAKE_BINARY_DIR}/share/" DESTINATION share)
