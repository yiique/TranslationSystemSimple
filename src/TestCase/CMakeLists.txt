INCLUDE_DIRECTORIES(../)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# test : blocking_queue
#SET(BLOCKING_QUEUE_TEST_SRC_LIST blocking_deque.cc)
#ADD_EXECUTABLE(blocking_deque ${BLOCKING_QUEUE_TEST_SRC_LIST})
#TARGET_LINK_LIBRARIES(blocking_deque ${LIB_BOOST_THREAD})
#TARGET_LINK_LIBRARIES(blocking_deque ${LIB_BOOST_DATETIME})

# test : rwlock
#SET(RWLOCK_TEST_SRC_LIST rwlock_test.cc)
#ADD_EXECUTABLE(rwlock_test ${RWLOCK_TEST_SRC_LIST})
#TARGET_LINK_LIBRARIES(rwlock_test ${LIB_BOOST_THREAD})
#TARGET_LINK_LIBRARIES(rwlock_test ${LIB_BOOST_DATETIME})
#TARGET_LINK_LIBRARIES(rwlock_test lib_common)

# test : event_engine
#SET(EVENTENGINE_TEST_SRC_LIST event_engine_test.cc)
#ADD_EXECUTABLE(event_engine_test ${EVENTENGINE_TEST_SRC_LIST})
#TARGET_LINK_LIBRARIES(event_engine_test ${LIB_BOOST_THREAD})
#TARGET_LINK_LIBRARIES(event_engine_test ${LIB_BOOST_DATETIME})
#TARGET_LINK_LIBRARIES(event_engine_test lib_eventengine)

# test : uuid_test
#SET(UUID_TEST_SRC_LIST uuid_test.cc)
#ADD_EXECUTABLE(uuid_test ${UUID_TEST_SRC_LIST})
#TARGET_LINK_LIBRARIES(uuid_test ${LIB_BOOST_DATETIME})
#TARGET_LINK_LIBRARIES(uuid_test ${LIB_BOOST_THREAD})
#TARGET_LINK_LIBRARIES(uuid_test lib_common)
#TARGET_LINK_LIBRARIES(uuid_test lib_log)
#TARGET_LINK_LIBRARIES(uuid_test lib_eventengine)

# test : time_test
#SET(TIME_TEST_SRC_LIST time_test.cc)
#ADD_EXECUTABLE(time_test ${TIME_TEST_SRC_LIST})
#TARGET_LINK_LIBRARIES(time_test ${BOOST_LIB_PATH}/thread/build/msvc-10.0/release/link-static/threading-multi/libboost_thread-vc100-mt-1_47.lib)
#TARGET_LINK_LIBRARIES(time_test ${LIB_BOOST_DATETIME}/date_time/build/msvc-10.0/release/link-static/threading-multi/libboost_date_time-vc100-mt-1_47.lib)
#TARGET_LINK_LIBRARIES(time_test lib_transtext)

# test : log_test
#SET(LOG_TEST_SRC_LIST log_test.cc)
#ADD_EXECUTABLE(log_test ${LOG_TEST_SRC_LIST})
#TARGET_LINK_LIBRARIES(log_test ${BOOST_LIB_PATH}/thread/build/msvc-10.0/release/link-static/threading-multi/libboost_thread-vc100-mt-1_47.lib)
#TARGET_LINK_LIBRARIES(log_test ${LIB_BOOST_DATETIME}/date_time/build/msvc-10.0/release/link-static/threading-multi/libboost_date_time-vc100-mt-1_47.lib)
#TARGET_LINK_LIBRARIES(log_test lib_log)
#TARGET_LINK_LIBRARIES(log_test lib_eventengine)

# test : net_test
#SET(NET_TEST_SRC_LIST net_test.cc)
#ADD_EXECUTABLE(net_test ${NET_TEST_SRC_LIST})
#TARGET_LINK_LIBRARIES(net_test ${LIB_BOOST_REGEX})
#TARGET_LINK_LIBRARIES(net_test ${LIB_BOOST_THREAD})
#TARGET_LINK_LIBRARIES(net_test ${LIB_BOOST_SYSTEM})
#TARGET_LINK_LIBRARIES(net_test ${LIB_BOOST_DATETIME})
#TARGET_LINK_LIBRARIES(net_test lib_log)
#TARGET_LINK_LIBRARIES(net_test lib_netdriver)
#TARGET_LINK_LIBRARIES(net_test lib_eventengine)
#TARGET_LINK_LIBRARIES(net_test lib_common)

# test : db_test
#SET(DB_TEST_SRC_LIST db_test.cc)
#ADD_EXECUTABLE(db_test ${DB_TEST_SRC_LIST})
#TARGET_LINK_LIBRARIES(db_test lib_log)
#TARGET_LINK_LIBRARIES(db_test lib_database)

