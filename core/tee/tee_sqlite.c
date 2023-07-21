#include <assert.h>
#include <string.h>
#include <trace.h>

#include <optee_rpc_cmd.h>
#include <kernel/thread.h>
#include <kernel/msg_param.h>
#include <tee/tee_svc.h>
#include <mm/tee_mm.h>
#include <mm/mobj.h>
#include <tee/tee_sqlite.h>

TEE_Result syscall_sqlite_exec(const void *sql, size_t sql_size, void *res, size_t res_size)
{
    uint8_t *sql_ree_shm = NULL;
    uint8_t *res_ree_shm = NULL;
    struct mobj *sql_mobj = NULL;
    struct mobj *res_mobj = NULL;
    TEE_Result result;
    struct thread_param params[2];
    memset(params, 0, sizeof(params));
    
    params[0].attr = THREAD_PARAM_ATTR_MEMREF_IN;
    // 分配共享内存
    sql_mobj = thread_rpc_alloc_payload(sql_size);
	if (!sql_mobj)
		return TEE_ERROR_OUT_OF_MEMORY;
	if (sql_mobj->size < sql_size) {
		result = TEE_ERROR_SHORT_BUFFER;
		goto exit1;
	}
    // 获取分配的共享内存的虚拟地址被保存在ree_shm中
    sql_ree_shm = mobj_get_va(sql_mobj, 0, sql_size);
    // 检查虚拟地址是否有效
    assert(sql_ree_shm);
    memcpy(sql_ree_shm, sql, sql_size);
    params[0].u.memref.mobj = sql_mobj;
    params[0].u.memref.size = sql_size;
    params[0].u.memref.offs = 0;

    params[1].attr = THREAD_PARAM_ATTR_MEMREF_OUT;
    // 分配共享内存
    res_mobj = thread_rpc_alloc_payload(res_size);
	if (!res_mobj)
		return TEE_ERROR_OUT_OF_MEMORY;
	if (res_mobj->size < res_size) {
		result = TEE_ERROR_SHORT_BUFFER;
		goto exit2;
	}
    // 获取分配的共享内存的虚拟地址被保存在ree_shm中
    res_ree_shm = mobj_get_va(res_mobj, 0, res_size);
    // 检查虚拟地址是否有效
    assert(res_ree_shm);
    params[1].u.memref.mobj = res_mobj;
    params[1].u.memref.size = res_size;
    params[1].u.memref.offs = 0;

    result = thread_rpc_cmd(OPTEE_MSG_RPC_CMD_SQLITE, 2, params);
    if (result != TEE_SUCCESS)
		goto exit2;
    
    memcpy(res, res_ree_shm, params[1].u.memref.size);
exit2:
	thread_rpc_free_payload(res_mobj);
exit1:
	thread_rpc_free_payload(sql_mobj);
	return result;
}


/**
 *  New sqlite_exec, inside this function, no TA will be created, 
 *  The sql cmd result will be return by result
*/

TEE_Result syscall_sqlite_exec_v2(const void *sql, size_t sql_size, void *res, size_t res_size)
{
    // DMSG("===================================================================\n");
    // DMSG("In syscall_sqlite_exec_v2() function\n");
   
    uint8_t *sql_ree_shm = NULL;
    uint8_t *res_ree_shm = NULL;
    struct mobj *sql_mobj = NULL;
    struct mobj *res_mobj = NULL;
    TEE_Result result;
    struct thread_param params[2];
    memset(params, 0, sizeof(params));
    
    params[0].attr = THREAD_PARAM_ATTR_MEMREF_IN;
    // 分配共享内存
    sql_mobj = thread_rpc_alloc_payload(sql_size);
	if (!sql_mobj)
		return TEE_ERROR_OUT_OF_MEMORY;
	if (sql_mobj->size < sql_size) {
		result = TEE_ERROR_SHORT_BUFFER;
		goto exit1;
	}
    // 获取分配的共享内存的虚拟地址被保存在ree_shm中
    sql_ree_shm = mobj_get_va(sql_mobj, 0, sql_size);  // get vitural address == get_va ; get physic variable == get_pa
    // 检查虚拟地址是否有效
    assert(sql_ree_shm);
    memcpy(sql_ree_shm, sql, sql_size);
    params[0].u.memref.mobj = sql_mobj; 
    params[0].u.memref.size = sql_size;
    params[0].u.memref.offs = 0;

    params[1].attr = THREAD_PARAM_ATTR_MEMREF_OUT;
    // 分配共享内存
    // DMSG("Memory size allocated : %zu\n", res_size);

    res_mobj = thread_rpc_alloc_payload(res_size);
	if (!res_mobj)
		return TEE_ERROR_OUT_OF_MEMORY;
	if (res_mobj->size < res_size) {
		result = TEE_ERROR_SHORT_BUFFER;
		goto exit2;
	}
    // 获取分配的共享内存的虚拟地址被保存在ree_shm中
    res_ree_shm = mobj_get_va(res_mobj, 0, res_size);
    // 检查虚拟地址是否有效
    assert(res_ree_shm);
    params[1].u.memref.mobj = res_mobj;
    params[1].u.memref.size = res_size;
    params[1].u.memref.offs = 0; 


    // DMSG( "==========================================");
    // DMSG("Before call sqlite cmd");
    result = thread_rpc_cmd(OPTEE_MSG_RPC_CMD_SQLITE_NEW, 2, params);
    // DMSG("Result = %s",  result == TEE_SUCCESS ? "SUCCESS" : "FAILED");
    // DMSG( "==========================================");
    if (result != TEE_SUCCESS){
		EMSG("RPC cmd call failed\n");
        goto exit2;
    }
    memcpy(res, res_ree_shm, params[1].u.memref.size);
    // DMSG("===================================================================\n");
exit2:
	thread_rpc_free_payload(res_mobj);
exit1:
	thread_rpc_free_payload(sql_mobj);
	return result;
}