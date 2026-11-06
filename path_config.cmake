macro(CMHashSetup CMHASH_ROOT_PATH)
    message("CMHash root path: " ${CMHASH_ROOT_PATH})

    set(CMHASH_ROOT_INCLUDE_PATH ${CMHASH_ROOT_PATH}/inc)
    set(CMHASH_ROOT_SOURCE_PATH  ${CMHASH_ROOT_PATH}/src)

    include_directories(${CMHASH_ROOT_INCLUDE_PATH})
endmacro()
