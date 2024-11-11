set(SRCS asym_cipher.cpp certificate.cpp encryptor.cpp log.cpp utils.cpp encryptorwrapper.cpp)
add_library(encrypt SHARED ${SRCS})
target_link_libraries(encrypt ${OPENSSL_LIBRARIES} ${GLOG_LIBRARIES})
target_include_directories(encrypt PUBLIC ${OPENSSL_INCLUDE_DIR} ${GLOG_INCLUDE_DIR})

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} encrypt)
target_include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include)