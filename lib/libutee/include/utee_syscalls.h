/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2015, Linaro Limited
 * Copyright (c) 2014, STMicroelectronics International N.V.
 */
#ifndef UTEE_SYSCALLS_H
#define UTEE_SYSCALLS_H

#include <compiler.h>
#include <stddef.h>
#include <stdint.h>

#include <utee_types.h>
#include <tee_api_types.h>
#include <trace.h>
#include <tzvfs_types.h>

/*
 * Arguments must use the native register width, unless it's a signed
 * argument then it must be a 32-bit value instead to avoid problems with
 * sign extension. To keep it simple, only use pointers, int32_t, unsigned
 * long and size_t. Pointers may only point structures or types based on
 * fixed width integer types. Only exception are buffers with opaque data.
 *
 * Return values should not use a fixed width larger than 32 bits, unsigned
 * long and pointers are OK though.
 *
 * Members in structs on the other hand should only use fixed width integer
 * types; uint32_t, uint64_t etc. To keep it simple, use uint64_t for all
 * length fields.
 */

void _utee_return(unsigned long ret) __noreturn;

void _utee_log(const void *buf, size_t len);

/* This is not __noreturn because AArch32 stack unwinding fails otherwise */
void _utee_panic(unsigned long code);

/* prop_set is TEE_PROPSET_xxx*/
TEE_Result _utee_get_property(unsigned long prop_set, unsigned long index,
			      void *name, uint32_t *name_len, void *buf,
			      uint32_t *blen, uint32_t *prop_type);

TEE_Result _utee_get_property_name_to_index(unsigned long prop_set,
					    const void *name,
					    unsigned long name_len,
					    uint32_t *index);

/* sess has type TEE_TASessionHandle */
TEE_Result _utee_open_ta_session(const TEE_UUID *dest,
				 unsigned long cancel_req_to,
				 struct utee_params *params, uint32_t *sess,
				 uint32_t *ret_orig);

/* sess has type TEE_TASessionHandle */
TEE_Result _utee_close_ta_session(unsigned long sess);

/* sess has type TEE_TASessionHandle */
TEE_Result _utee_invoke_ta_command(unsigned long sess,
				   unsigned long cancel_req_to,
				   unsigned long cmd_id,
				   struct utee_params *params,
				   uint32_t *ret_orig);

TEE_Result _utee_check_access_rights(uint32_t flags, const void *buf,
				     size_t len);

/* cancel has type bool */
TEE_Result _utee_get_cancellation_flag(uint32_t *cancel);

/* old_mask has type bool */
TEE_Result _utee_unmask_cancellation(uint32_t *old_mask);

/* old_mask has type bool */
TEE_Result _utee_mask_cancellation(uint32_t *old_mask);

TEE_Result _utee_wait(unsigned long timeout);

/* cat has type enum _utee_time_category */
TEE_Result _utee_get_time(unsigned long cat, TEE_Time *time);

TEE_Result _utee_set_ta_time(const TEE_Time *time);

TEE_Result _utee_cryp_state_alloc(unsigned long algo, unsigned long op_mode,
				  unsigned long key1, unsigned long key2,
				  uint32_t *state);
TEE_Result _utee_cryp_state_copy(unsigned long dst, unsigned long src);
TEE_Result _utee_cryp_state_free(unsigned long state);

/* iv and iv_len are ignored for some algorithms */
TEE_Result _utee_hash_init(unsigned long state, const void *iv, size_t iv_len);
TEE_Result _utee_hash_update(unsigned long state, const void *chunk,
			     size_t chunk_size);
TEE_Result _utee_hash_final(unsigned long state, const void *chunk,
			    size_t chunk_size, void *hash, uint64_t *hash_len);

TEE_Result _utee_cipher_init(unsigned long state, const void *iv,
			     size_t iv_len);
TEE_Result _utee_cipher_update(unsigned long state, const void *src,
			       size_t src_len, void *dest, uint64_t *dest_len);
TEE_Result _utee_cipher_final(unsigned long state, const void *src,
			      size_t src_len, void *dest, uint64_t *dest_len);

/* Generic Object Functions */
TEE_Result _utee_cryp_obj_get_info(unsigned long obj,
				   struct utee_object_info *info);
TEE_Result _utee_cryp_obj_restrict_usage(unsigned long obj,
					 unsigned long usage);
TEE_Result _utee_cryp_obj_get_attr(unsigned long obj, unsigned long attr_id,
				   void *buffer, uint64_t *size);

/* Transient Object Functions */
/* type has type TEE_ObjectType */
TEE_Result _utee_cryp_obj_alloc(unsigned long type, unsigned long max_size,
				uint32_t *obj);
TEE_Result _utee_cryp_obj_close(unsigned long obj);
TEE_Result _utee_cryp_obj_reset(unsigned long obj);
TEE_Result _utee_cryp_obj_populate(unsigned long obj,
				   struct utee_attribute *attrs,
				   unsigned long attr_count);
TEE_Result _utee_cryp_obj_copy(unsigned long dst_obj, unsigned long src_obj);

TEE_Result _utee_cryp_obj_generate_key(unsigned long obj,
				       unsigned long key_size,
				       const struct utee_attribute *params,
				       unsigned long param_count);

TEE_Result _utee_cryp_derive_key(unsigned long state,
				 const struct utee_attribute *params,
				 unsigned long param_count,
				 unsigned long derived_key);

TEE_Result _utee_cryp_random_number_generate(void *buf, size_t blen);

TEE_Result _utee_authenc_init(unsigned long state, const void *nonce,
			      size_t nonce_len, size_t tag_len, size_t aad_len,
			      size_t payload_len);
TEE_Result _utee_authenc_update_aad(unsigned long state, const void *aad_data,
				    size_t aad_data_len);
TEE_Result _utee_authenc_update_payload(unsigned long state,
					const void *src_data, size_t src_len,
					void *dest_data, uint64_t *dest_len);
TEE_Result _utee_authenc_enc_final(unsigned long state, const void *src_data,
				   size_t src_len, void *dest_data,
				   uint64_t *dest_len, void *tag,
				   uint64_t *tag_len);
TEE_Result _utee_authenc_dec_final(unsigned long state, const void *src_data,
				   size_t src_len, void *dest_data,
				   uint64_t *dest_len, const void *tag,
				   size_t tag_len);

TEE_Result _utee_asymm_operate(unsigned long state,
			       const struct utee_attribute *params,
			       unsigned long num_params, const void *src_data,
			       size_t src_len, void *dest_data,
			       uint64_t *dest_len);

TEE_Result _utee_asymm_verify(unsigned long state,
			      const struct utee_attribute *params,
			      unsigned long num_params, const void *data,
			      size_t data_len, const void *sig, size_t sig_len);

/* Persistant Object Functions */
/* obj is of type TEE_ObjectHandle */
TEE_Result _utee_storage_obj_open(unsigned long storage_id,
				  const void *object_id, size_t object_id_len,
				  unsigned long flags, uint32_t *obj);

/*
 * attr is of type TEE_ObjectHandle
 * obj is of type TEE_ObjectHandle
 */
TEE_Result _utee_storage_obj_create(unsigned long storage_id,
				    const void *object_id,
				    size_t object_id_len, unsigned long flags,
				    unsigned long attr, const void *data,
				    size_t len, uint32_t *obj);

/* obj is of type TEE_ObjectHandle */
TEE_Result _utee_storage_obj_del(unsigned long obj);

/* obj is of type TEE_ObjectHandle */
TEE_Result _utee_storage_obj_rename(unsigned long obj, const void *new_obj_id,
				    size_t new_obj_id_len);

/* Persistent Object Enumeration Functions */
/* obj_enum is of type TEE_ObjectEnumHandle */
TEE_Result _utee_storage_alloc_enum(uint32_t *obj_enum);


/* obj_enum is of type TEE_ObjectEnumHandle */
TEE_Result _utee_storage_free_enum(unsigned long obj_enum);

/* obj_enum is of type TEE_ObjectEnumHandle */
TEE_Result _utee_storage_reset_enum(unsigned long obj_enum);

/* obj_enum is of type TEE_ObjectEnumHandle */
TEE_Result _utee_storage_start_enum(unsigned long obj_enum,
				    unsigned long storage_id);

/* obj_enum is of type TEE_ObjectEnumHandle */
TEE_Result _utee_storage_next_enum(unsigned long obj_enum,
				   struct utee_object_info *info,
				   void *obj_id, uint64_t *len);

/* Data Stream Access Functions */
/* obj is of type TEE_ObjectHandle */
TEE_Result _utee_storage_obj_read(unsigned long obj, void *data, size_t len,
				  uint64_t *count);

/* obj is of type TEE_ObjectHandle */
TEE_Result _utee_storage_obj_write(unsigned long obj, const void *data,
				   size_t len);

/* obj is of type TEE_ObjectHandle */
TEE_Result _utee_storage_obj_trunc(unsigned long obj, size_t len);

/* obj is of type TEE_ObjectHandle */
/* whence is of type TEE_Whence */
TEE_Result _utee_storage_obj_seek(unsigned long obj, int32_t offset,
				  unsigned long whence);

/* op is of type enum _utee_cache_operation */
TEE_Result _utee_cache_operation(void *va, size_t l, unsigned long op);

TEE_Result _utee_gprof_send(void *buf, size_t size, uint32_t *id);

TEE_Result utee_tpm_get_version(void *buf);

TEE_Result utee_sqlite_exec(const void *sql, size_t sql_size, void *res, size_t res_size);

/* Second version of optee-os api*/
TEE_Result utee_sqlite_exec_v2(const void *sql, size_t sql_size, void *res, size_t res_size);

int utee_tzvfs_open(int *tzvfs_errno, const char *filename, int flags, mode_t mode);

int utee_tzvfs_close(int *tzvfs_errno, int fd);

char *utee_tzvfs_getcwd(int *tzvfs_errno, char *buf, size_t size);

int utee_tzvfs_lstat(int *tzvfs_errno, const char* path, struct tzvfs_stat *buf);

int utee_tzvfs_stat(int *tzvfs_errno, const char *path, struct tzvfs_stat *buf);

int utee_tzvfs_fstat(int *tzvfs_errno, int fd, struct tzvfs_stat *buf);

int utee_tzvfs_fcntl(int *tzvfs_errno, int fd, int cmd, struct tzvfs_flock *arg);

ssize_t utee_tzvfs_read(int *tzvfs_errno, int fd, void *buf, size_t count);

ssize_t utee_tzvfs_write(int *tzvfs_errno, int fd, const void *buf, size_t count);

uid_t utee_tzvfs_geteuid(int *tzvfs_errno);

int utee_tzvfs_unlink(int *tzvfs_errno, const char *pathname);

int utee_tzvfs_access(int *tzvfs_errno, const char *pathname, int mode);

void *utee_tzvfs_mmap(int *tzvfs_errno, void *addr, size_t len, int prot, int flags, int fildes, off_t off);

void *utee_tzvfs_mremap(int *tzvfs_errno, void *old_address, size_t old_size, size_t new_size, int flags);

int utee_tzvfs_munmap(int *tzvfs_errno, void *addr, size_t length);

size_t utee_tzvfs_strcspn(int *tzvfs_errno, const char *str1, const char *str2);

int utee_tzvfs_utimes(int *tzvfs_errno, const char *filename, const struct tzvfs_timeval times[2]);

off_t utee_tzvfs_lseek(int *tzvfs_errno, int fd, off_t offset, int whence);

int utee_tzvfs_fsync(int *tzvfs_errno, int fd);

char* utee_tzvfs_getenv(int *tzvfs_errno, const char *name);

pid_t utee_tzvfs_getpid(int *tzvfs_errno);

time_t utee_tzvfs_time(int *tzvfs_errno, time_t *t);

unsigned int utee_tzvfs_sleep(int *tzvfs_errno, unsigned int seconds);

int utee_tzvfs_gettimeofday(int *tzvfs_errno, struct tzvfs_timeval *tv, struct tzvfs_timezone *tz);

int utee_tzvfs_fchown(int *tzvfs_errno, int fd, uid_t owner, gid_t group);

#endif /* UTEE_SYSCALLS_H */
