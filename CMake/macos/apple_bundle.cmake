# macOS .app bundle assembly for 2 Ship 2 Harkinian (2ship target).
#
# Builds "2 Ship 2 Harkinian.app" directly from the normal build (MACOSX_BUNDLE): compiles the
# Liquid Glass app icon from the Icon Composer package, bundles the runtime resources, relinks
# non-system dylibs (incl. SDL3 for the sdl2-compat shim) into Contents/Frameworks, and ad-hoc
# codesigns the result so it launches without "damaged app" warnings.
#
# Runtime layout (libultraship):
#   * GetAppBundlePath()    -> <App>.app/Contents/Resources  (read-only: 2ship.o2r, gamecontrollerdb.txt)
#   * GetAppDirectoryPath() -> SHIP_HOME (~/Library/Application Support/com.2ship2harkinian.2s2h):
#                              mm.o2r extracted from the user's ROM on first run, config/saves/logs/mods

set(MACOS_DIR ${CMAKE_SOURCE_DIR}/CMake/macos)
set(ENTITLEMENTS_FILE ${MACOS_DIR}/entitlements.plist)

option(S2H_BUNDLE_DEPS "Relink and bundle dylibs into the .app so it is portable" ON)

# ---------------------------------------------------------------------------
# Liquid Glass app icon (Icon Composer .icon -> Assets.car + icns fallback via actool)
# ---------------------------------------------------------------------------
set(ICON_SOURCE ${CMAKE_SOURCE_DIR}/mm/macosx/2s2hicon.icon)
set(ICON_COMPILE_DIR ${CMAKE_BINARY_DIR}/AppIconAssets)
set(ASSETS_CAR ${ICON_COMPILE_DIR}/Assets.car)
set(ICNS_FALLBACK ${ICON_COMPILE_DIR}/2s2hicon.icns)
add_custom_command(
    OUTPUT ${ASSETS_CAR} ${ICNS_FALLBACK}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ICON_COMPILE_DIR}
    COMMAND xcrun actool ${ICON_SOURCE}
            --compile ${ICON_COMPILE_DIR}
            --app-icon 2s2hicon
            --output-partial-info-plist ${ICON_COMPILE_DIR}/icon-partial.plist
            --platform macosx --target-device mac
            --minimum-deployment-target ${CMAKE_OSX_DEPLOYMENT_TARGET}
            --errors --warnings
    DEPENDS ${ICON_SOURCE}/icon.json
    COMMENT "Compiling Liquid Glass app icon (Icon Composer) with actool"
)
add_custom_target(s2hAppIcon DEPENDS ${ASSETS_CAR} ${ICNS_FALLBACK})
add_dependencies(2ship s2hAppIcon)
set_source_files_properties(${ASSETS_CAR} ${ICNS_FALLBACK} PROPERTIES
    GENERATED TRUE MACOSX_PACKAGE_LOCATION "Resources")
target_sources(2ship PRIVATE ${ASSETS_CAR} ${ICNS_FALLBACK})

# ---------------------------------------------------------------------------
# Bundle metadata. OUTPUT_NAME "2 Ship 2 Harkinian" -> "2 Ship 2 Harkinian.app" with
# Contents/MacOS/"2 Ship 2 Harkinian" (matches CFBundleExecutable in the configured Info.plist).
# ---------------------------------------------------------------------------
set_target_properties(2ship PROPERTIES
    OUTPUT_NAME "2 Ship 2 Harkinian"
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_BINARY_DIR}/macosx/Info.plist
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "-"
    XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS ${ENTITLEMENTS_FILE}
)

# NB: add_dependencies(2ship Generate2ShipOtr) is added from the root CMakeLists, because
# Generate2ShipOtr is defined there (after this file is included from mm/CMakeLists.txt).

# ---------------------------------------------------------------------------
# Copy runtime resources into Contents/Resources after the app links
# ---------------------------------------------------------------------------
set(RES_DIR "$<TARGET_BUNDLE_DIR:2ship>/Contents/Resources")
add_custom_command(TARGET 2ship POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${RES_DIR}"
    # CMake's MACOSX_BUNDLE doesn't reliably emit Contents/Info.plist here (the spaces in the
    # "2 Ship 2 Harkinian" OUTPUT_NAME trip it up), so copy the configured plist in explicitly.
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/macosx/Info.plist" "$<TARGET_BUNDLE_DIR:2ship>/Contents/Info.plist"
    COMMAND bash -c "for p in '${CMAKE_BINARY_DIR}/mm/2ship.o2r' '${CMAKE_SOURCE_DIR}/mm/2ship.o2r' '${CMAKE_BINARY_DIR}/2ship.o2r' '${CMAKE_SOURCE_DIR}/2ship.o2r'; do if [ -f \"$p\" ]; then cp \"$p\" '${RES_DIR}/2ship.o2r'; break; fi; done; [ -f '${RES_DIR}/2ship.o2r' ] || echo 'note: 2ship.o2r not found - build the Generate2ShipOtr target, then rebuild'"
    COMMAND bash -c "[ -f '${CMAKE_BINARY_DIR}/gamecontrollerdb.txt' ] && cp '${CMAKE_BINARY_DIR}/gamecontrollerdb.txt' '${RES_DIR}/gamecontrollerdb.txt' || true"
    # The first-run OTR extractor needs the asset definitions at GetAppBundlePath()/assets
    # (RunExtract checks Contents/Resources/assets). Mirror upstream's install:
    # mm/assets/extractor/ -> Resources/assets/, mm/assets/xml/ -> Resources/assets/xml/.
    COMMAND ${CMAKE_COMMAND} -E make_directory "${RES_DIR}/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/mm/assets/extractor" "${RES_DIR}/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/mm/assets/xml" "${RES_DIR}/assets/xml"
    # Upstream's POST_BUILD (mm/CMakeLists.txt) also copies the extractor assets next to the binary
    # for non-bundle runs, which lands them in Contents/MacOS/. In a bundle that copy is dead weight
    # AND breaks release zips: codesign can only sign loose non-Mach-O files in MacOS/ via extended
    # attributes, which zips either drop (broken seal) or carry as AppleDouble ._ entries that
    # Archive Utility mishandles on symlinks ("damaged" app for downloaders). Remove it here; this
    # runs after upstream's copy because this file is included later in mm/CMakeLists.txt.
    COMMAND ${CMAKE_COMMAND} -E rm -rf "$<TARGET_FILE_DIR:2ship>/assets"
    COMMENT "Bundling 2 Ship 2 Harkinian resources into the .app"
    VERBATIM
)

# ---------------------------------------------------------------------------
# Relink dylibs into Contents/Frameworks (portable .app) + SDL3, then codesign
# ---------------------------------------------------------------------------
if (S2H_BUNDLE_DEPS)
    add_custom_command(TARGET 2ship POST_BUILD
        COMMAND ${CMAKE_COMMAND}
            -DAPP_BUNDLE=$<TARGET_BUNDLE_DIR:2ship>
            "-DEXECUTABLE_NAME=2 Ship 2 Harkinian"
            -P ${MACOS_DIR}/fixup_bundle.cmake
        COMMAND bash -c "install_name_tool -add_rpath '@executable_path/../Frameworks/' '$<TARGET_BUNDLE_DIR:2ship>/Contents/MacOS/2 Ship 2 Harkinian' 2>/dev/null || true"
        # Homebrew's sdl2 is sdl2-compat, a shim that dlopen()s libSDL3.dylib from @loader_path at
        # runtime; fixup_bundle can't follow a dlopen, so copy SDL3 in next to the bundled libSDL2.
        COMMAND bash -c "SDL3_LIB=$(brew --prefix sdl3 2>/dev/null)/lib/libSDL3.0.dylib; if [ -f \"$SDL3_LIB\" ]; then cp \"$SDL3_LIB\" '$<TARGET_BUNDLE_DIR:2ship>/Contents/Frameworks/libSDL3.dylib' && chmod u+w '$<TARGET_BUNDLE_DIR:2ship>/Contents/Frameworks/libSDL3.dylib'; fi"
        COMMENT "Relinking dylibs into the .app bundle (incl. SDL3 for sdl2-compat)"
        VERBATIM
    )
endif()

add_custom_command(TARGET 2ship POST_BUILD
    COMMAND codesign --force --deep --sign - --options runtime --entitlements ${ENTITLEMENTS_FILE} "$<TARGET_BUNDLE_DIR:2ship>"
    COMMENT "Ad-hoc codesigning 2 Ship 2 Harkinian.app"
    VERBATIM
)
