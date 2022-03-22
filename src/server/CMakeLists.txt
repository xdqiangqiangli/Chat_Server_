
aux_source_directory(. SCR_LIST)
aux_source_directory(./database DABS)
aux_source_directory(./model MODEL)
aux_source_directory(./redis REDIS)
add_executable(Chat_Server ${SCR_LIST} ${DABS} ${MODEL} ${REDIS})

target_link_libraries(Chat_Server muduo_net muduo_base mysqlclient hiredis pthread)