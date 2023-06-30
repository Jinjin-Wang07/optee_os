#ifndef TEE_SQLITE_H
#define TEE_SQLITE_H

#include <tee_api_types.h> 

TEE_Result syscall_sqlite_exec(const void *sql, size_t sql_size, void *res, size_t res_size);

TEE_Result syscall_sqlite_exec_v2(const void *sql, size_t sql_size, void *res, size_t res_size);

TEE_Result syscall_sqlite_insert(const void *shared_buffer, size_t size);

#endif