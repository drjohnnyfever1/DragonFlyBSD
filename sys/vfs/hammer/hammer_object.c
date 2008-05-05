/*
 * Copyright (c) 2007-2008 The DragonFly Project.  All rights reserved.
 * 
 * This code is derived from software contributed to The DragonFly Project
 * by Matthew Dillon <dillon@backplane.com>
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of The DragonFly Project nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific, prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 * $DragonFly: src/sys/vfs/hammer/hammer_object.c,v 1.55 2008/05/05 20:34:48 dillon Exp $
 */

#include "hammer.h"

static int hammer_mem_add(hammer_transaction_t trans, hammer_record_t record);
static int hammer_mem_lookup(hammer_cursor_t cursor, hammer_inode_t ip);
static int hammer_mem_first(hammer_cursor_t cursor);

/*
 * Red-black tree support.
 */
static int
hammer_rec_rb_compare(hammer_record_t rec1, hammer_record_t rec2)
{
	if (rec1->rec.base.base.rec_type < rec2->rec.base.base.rec_type)
		return(-1);
	if (rec1->rec.base.base.rec_type > rec2->rec.base.base.rec_type)
		return(1);

	if (rec1->rec.base.base.key < rec2->rec.base.base.key)
		return(-1);
	if (rec1->rec.base.base.key > rec2->rec.base.base.key)
		return(1);

	if (rec1->rec.base.base.create_tid == 0) {
		if (rec2->rec.base.base.create_tid == 0)
			return(0);
		return(1);
	}
	if (rec2->rec.base.base.create_tid == 0)
		return(-1);

	if (rec1->rec.base.base.create_tid < rec2->rec.base.base.create_tid)
		return(-1);
	if (rec1->rec.base.base.create_tid > rec2->rec.base.base.create_tid)
		return(1);

	/*
	 * Never match against an item deleted by the front-end.
	 */
	if (rec1->flags & HAMMER_RECF_DELETED_FE)
		return(1);
	if (rec2->flags & HAMMER_RECF_DELETED_FE)
		return(-1);

        return(0);
}

static int
hammer_rec_compare(hammer_base_elm_t info, hammer_record_t rec)
{
	if (info->rec_type < rec->rec.base.base.rec_type)
		return(-3);
	if (info->rec_type > rec->rec.base.base.rec_type)
		return(3);

        if (info->key < rec->rec.base.base.key)
                return(-2);
        if (info->key > rec->rec.base.base.key)
                return(2);

	if (info->create_tid == 0) {
		if (rec->rec.base.base.create_tid == 0)
			return(0);
		return(1);
	}
	if (rec->rec.base.base.create_tid == 0)
		return(-1);
	if (info->create_tid < rec->rec.base.base.create_tid)
		return(-1);
	if (info->create_tid > rec->rec.base.base.create_tid)
		return(1);
        return(0);
}

/*
 * RB_SCAN comparison code for hammer_mem_first().  The argument order
 * is reversed so the comparison result has to be negated.  key_beg and
 * key_end are both range-inclusive.
 *
 * The creation timestamp can cause hammer_rec_compare() to return -1 or +1.
 * These do not stop the scan.
 *
 * Localized deletions are not cached in-memory.
 */
static
int
hammer_rec_scan_cmp(hammer_record_t rec, void *data)
{
	hammer_cursor_t cursor = data;
	int r;

	r = hammer_rec_compare(&cursor->key_beg, rec);
	if (r > 1)
		return(-1);
	r = hammer_rec_compare(&cursor->key_end, rec);
	if (r < -1)
		return(1);
	return(0);
}

/*
 * This compare function is used when simply looking up key_beg.
 */
static
int
hammer_rec_find_cmp(hammer_record_t rec, void *data)
{
	hammer_cursor_t cursor = data;
	int r;

	r = hammer_rec_compare(&cursor->key_beg, rec);
	if (r > 1)
		return(-1);
	if (r < -1)
		return(1);
	return(0);
}

RB_GENERATE(hammer_rec_rb_tree, hammer_record, rb_node, hammer_rec_rb_compare);
RB_GENERATE_XLOOKUP(hammer_rec_rb_tree, INFO, hammer_record, rb_node,
		    hammer_rec_compare, hammer_base_elm_t);

/*
 * Allocate a record for the caller to finish filling in.  The record is
 * returned referenced.
 */
hammer_record_t
hammer_alloc_mem_record(hammer_inode_t ip)
{
	hammer_record_t record;

	++hammer_count_records;
	record = kmalloc(sizeof(*record), M_HAMMER, M_WAITOK|M_ZERO);
	record->flush_state = HAMMER_FST_IDLE;
	record->ip = ip;
	record->rec.base.base.btype = HAMMER_BTREE_TYPE_RECORD;
	hammer_ref(&record->lock);
	return (record);
}

void
hammer_wait_mem_record(hammer_record_t record)
{
	while (record->flush_state == HAMMER_FST_FLUSH) {
		record->flags |= HAMMER_RECF_WANTED;
		tsleep(record, 0, "hmrrc2", 0);
	}
}

/*
 * Called from the backend, hammer_inode.c, after a record has been
 * flushed to disk.  The record has been exclusively locked by the
 * caller and interlocked with BE.
 *
 * We clean up the state, unlock, and release the record (the record
 * was referenced by the fact that it was in the HAMMER_FST_FLUSH state).
 */
void
hammer_flush_record_done(hammer_record_t record, int error)
{
	hammer_inode_t target_ip;

	KKASSERT(record->flush_state == HAMMER_FST_FLUSH);
	KKASSERT(record->flags & HAMMER_RECF_INTERLOCK_BE);

	if (error) {
		/*
		 * An error occured, the backend was unable to sync the
		 * record to its media.  Leave the record intact.
		 */
		Debugger("flush_record_done error");
	}

	if (record->flags & HAMMER_RECF_DELETED_BE) {
		if ((target_ip = record->target_ip) != NULL) {
			TAILQ_REMOVE(&target_ip->target_list, record,
				     target_entry);
			record->target_ip = NULL;
			hammer_test_inode(target_ip);
		}
		record->flush_state = HAMMER_FST_IDLE;
	} else {
		if (record->target_ip) {
			record->flush_state = HAMMER_FST_SETUP;
			hammer_test_inode(record->ip);
			hammer_test_inode(record->target_ip);
		} else {
			record->flush_state = HAMMER_FST_IDLE;
		}
	}
	record->flags &= ~HAMMER_RECF_INTERLOCK_BE;
	if (record->flags & HAMMER_RECF_WANTED) {
		record->flags &= ~HAMMER_RECF_WANTED;
		wakeup(record);
	}
	hammer_rel_mem_record(record);
}

/*
 * Release a memory record.  Records marked for deletion are immediately
 * removed from the RB-Tree but otherwise left intact until the last ref
 * goes away.
 */
void
hammer_rel_mem_record(struct hammer_record *record)
{
	hammer_inode_t ip, target_ip;

	hammer_unref(&record->lock);

	if (record->flags & HAMMER_RECF_DELETED_FE) {
		if (record->lock.refs == 0) {
			KKASSERT(record->flush_state != HAMMER_FST_FLUSH);

			ip = record->ip;
			if ((target_ip = record->target_ip) != NULL) {
				TAILQ_REMOVE(&target_ip->target_list,
					     record, target_entry);
				record->target_ip = NULL;
				hammer_test_inode(target_ip);
			}

			if (record->flags & HAMMER_RECF_ONRBTREE) {
				RB_REMOVE(hammer_rec_rb_tree,
					  &record->ip->rec_tree,
					  record);
				record->flags &= ~HAMMER_RECF_ONRBTREE;
				if (RB_EMPTY(&record->ip->rec_tree)) {
					record->ip->flags &= ~HAMMER_INODE_XDIRTY;
					hammer_test_inode(record->ip);
				}
			}
			if (record->flags & HAMMER_RECF_ALLOCDATA) {
				--hammer_count_record_datas;
				kfree(record->data, M_HAMMER);
				record->flags &= ~HAMMER_RECF_ALLOCDATA;
			}
			record->data = NULL;
			--hammer_count_records;
			kfree(record, M_HAMMER);
			return;
		}
	}
}

/*
 * Record visibility depends on whether the record is being accessed by
 * the backend or the frontend.
 *
 * Return non-zero if the record is visible, zero if it isn't or if it is
 * deleted.
 */
static __inline
int
hammer_ip_iterate_mem_good(hammer_cursor_t cursor, hammer_record_t record)
{
	if (cursor->flags & HAMMER_CURSOR_BACKEND) {
		if (record->flags & HAMMER_RECF_DELETED_BE)
			return(0);
#if 0
		if ((record->flags & HAMMER_RECF_INTERLOCK_BE) == 0)
			return(0);
#endif
	} else {
		if (record->flags & HAMMER_RECF_DELETED_FE)
			return(0);
	}
	return(1);
}

/*
 * This callback is used as part of the RB_SCAN function for in-memory
 * records.  We terminate it (return -1) as soon as we get a match.
 *
 * This routine is used by frontend code.
 *
 * The primary compare code does not account for ASOF lookups.  This
 * code handles that case as well as a few others.
 */
static
int
hammer_rec_scan_callback(hammer_record_t rec, void *data)
{
	hammer_cursor_t cursor = data;

	/*
	 * We terminate on success, so this should be NULL on entry.
	 */
	KKASSERT(cursor->iprec == NULL);

	/*
	 * Skip if the record was marked deleted.
	 */
	if (hammer_ip_iterate_mem_good(cursor, rec) == 0)
		return(0);

	/*
	 * Skip if not visible due to our as-of TID
	 */
        if (cursor->flags & HAMMER_CURSOR_ASOF) {
                if (cursor->asof < rec->rec.base.base.create_tid)
                        return(0);
                if (rec->rec.base.base.delete_tid &&
		    cursor->asof >= rec->rec.base.base.delete_tid) {
                        return(0);
		}
        }

	/*
	 * If the record is queued to the flusher we have to block until
	 * it isn't.  Otherwise we may see duplication between our memory
	 * cache and the media.
	 */
	hammer_ref(&rec->lock);

#warning "This deadlocks"
#if 0
	if (rec->flush_state == HAMMER_FST_FLUSH)
		hammer_wait_mem_record(rec);
#endif

	/*
	 * The record may have been deleted while we were blocked.
	 */
	if (hammer_ip_iterate_mem_good(cursor, rec) == 0) {
		hammer_rel_mem_record(rec);
		return(0);
	}

	/*
	 * Set the matching record and stop the scan.
	 */
	cursor->iprec = rec;
	return(-1);
}


/*
 * Lookup an in-memory record given the key specified in the cursor.  Works
 * just like hammer_btree_lookup() but operates on an inode's in-memory
 * record list.
 *
 * The lookup must fail if the record is marked for deferred deletion.
 */
static
int
hammer_mem_lookup(hammer_cursor_t cursor, hammer_inode_t ip)
{
	int error;

	if (cursor->iprec) {
		hammer_rel_mem_record(cursor->iprec);
		cursor->iprec = NULL;
	}
	if (cursor->ip) {
		KKASSERT(cursor->ip->cursor_ip_refs > 0);
		--cursor->ip->cursor_ip_refs;
#if 0
		hammer_rec_rb_tree_scan_info_done(&cursor->scan,
						  &cursor->ip->rec_tree);
#endif
	}
	cursor->ip = ip;
#if 0
	hammer_rec_rb_tree_scan_info_link(&cursor->scan, &ip->rec_tree);
#endif
	++ip->cursor_ip_refs;

#if 0
	cursor->scan.node = NULL;
#endif
	hammer_rec_rb_tree_RB_SCAN(&ip->rec_tree, hammer_rec_find_cmp,
				   hammer_rec_scan_callback, cursor);

	if (cursor->iprec == NULL)
		error = ENOENT;
	else
		error = 0;
	return(error);
}

/*
 * hammer_mem_first() - locate the first in-memory record matching the
 * cursor within the bounds of the key range.
 */
static
int
hammer_mem_first(hammer_cursor_t cursor)
{
	hammer_inode_t ip;

	ip = cursor->ip;
	KKASSERT(ip != NULL);

	if (cursor->iprec) {
		hammer_rel_mem_record(cursor->iprec);
		cursor->iprec = NULL;
	}

	hammer_rec_rb_tree_RB_SCAN(&ip->rec_tree, hammer_rec_scan_cmp,
				   hammer_rec_scan_callback, cursor);

	/*
	 * Adjust scan.node and keep it linked into the RB-tree so we can
	 * hold the cursor through third party modifications of the RB-tree.
	 */
	if (cursor->iprec)
		return(0);
	return(ENOENT);
}

void
hammer_mem_done(hammer_cursor_t cursor)
{
        if (cursor->iprec) {
		hammer_rel_mem_record(cursor->iprec);
		cursor->iprec = NULL;
	}
}

/************************************************************************
 *		     HAMMER IN-MEMORY RECORD FUNCTIONS			*
 ************************************************************************
 *
 * These functions manipulate in-memory records.  Such records typically
 * exist prior to being committed to disk or indexed via the on-disk B-Tree.
 */

/*
 * Add a directory entry (dip,ncp) which references inode (ip).
 *
 * Note that the low 32 bits of the namekey are set temporarily to create
 * a unique in-memory record, and may be modified a second time when the
 * record is synchronized to disk.  In particular, the low 32 bits cannot be
 * all 0's when synching to disk, which is not handled here.
 */
int
hammer_ip_add_directory(struct hammer_transaction *trans,
		     struct hammer_inode *dip, struct namecache *ncp,
		     struct hammer_inode *ip)
{
	hammer_record_t record;
	int error;
	int bytes;

	record = hammer_alloc_mem_record(dip);

	bytes = ncp->nc_nlen;	/* NOTE: terminating \0 is NOT included */
	if (++trans->hmp->namekey_iterator == 0)
		++trans->hmp->namekey_iterator;

	record->type = HAMMER_MEM_RECORD_ADD;
	record->rec.entry.base.signature = HAMMER_RECORD_SIGNATURE_GOOD;
	record->rec.entry.base.base.obj_id = dip->obj_id;
	record->rec.entry.base.base.key =
		hammer_directory_namekey(ncp->nc_name, bytes);
	record->rec.entry.base.base.key += trans->hmp->namekey_iterator;
	record->rec.entry.base.base.rec_type = HAMMER_RECTYPE_DIRENTRY;
	record->rec.entry.base.base.obj_type = ip->ino_rec.base.base.obj_type;
	record->rec.entry.obj_id = ip->obj_id;
	record->data = (void *)ncp->nc_name;
	record->rec.entry.base.data_len = bytes;
	++ip->ino_rec.ino_nlinks;
	hammer_modify_inode(trans, ip, HAMMER_INODE_RDIRTY);

	/*
	 * The target inode and the directory entry are bound together.
	 */
	record->target_ip = ip;
	record->flush_state = HAMMER_FST_SETUP;
	TAILQ_INSERT_TAIL(&ip->target_list, record, target_entry);

	/*
	 * The inode now has a dependancy and must be taken out of the idle
	 * state.  An inode not in an idle state is given an extra reference.
	 */
	if (ip->flush_state == HAMMER_FST_IDLE) {
		hammer_ref(&ip->lock);
		ip->flush_state = HAMMER_FST_SETUP;
	}

	/* NOTE: copies record->data */
	error = hammer_mem_add(trans, record);
	return(error);
}

/*
 * Delete the directory entry and update the inode link count.  The
 * cursor must be seeked to the directory entry record being deleted.
 *
 * The related inode should be share-locked by the caller.  The caller is
 * on the frontend.
 *
 * This function can return EDEADLK requiring the caller to terminate
 * the cursor, any locks, wait on the returned record, and retry.
 */
int
hammer_ip_del_directory(struct hammer_transaction *trans,
		     hammer_cursor_t cursor, struct hammer_inode *dip,
		     struct hammer_inode *ip)
{
	hammer_record_t record;
	int error;

	if (cursor->record == &cursor->iprec->rec) {
		/*
		 * In-memory (unsynchronized) records can simply be freed.
		 * Even though the HAMMER_RECF_DELETED_FE flag is ignored
		 * by the backend, we must still avoid races against the
		 * backend potentially syncing the record to the media. 
		 *
		 * We cannot call hammer_ip_delete_record(), that routine may
		 * only be called from the backend.
		 */
		record = cursor->iprec;
		if (record->flags & HAMMER_RECF_INTERLOCK_BE) {
			KKASSERT(cursor->deadlk_rec == NULL);
			hammer_ref(&record->lock);
			cursor->deadlk_rec = record;
			error = EDEADLK;
		} else {
			KKASSERT(record->type == HAMMER_MEM_RECORD_ADD);
			record->flags |= HAMMER_RECF_DELETED_FE;
			error = 0;
		}
	} else {
		/*
		 * If the record is on-disk we have to queue the deletion by
		 * the record's key.  This also causes lookups to skip the
		 * record.
		 */
		record = hammer_alloc_mem_record(dip);
		record->type = HAMMER_MEM_RECORD_DEL;
		record->rec.entry.base.signature = HAMMER_RECORD_SIGNATURE_GOOD;
		record->rec.entry.base.base = cursor->record->base.base;
		hammer_modify_inode(trans, ip, HAMMER_INODE_RDIRTY);

		record->target_ip = ip;
		record->flush_state = HAMMER_FST_SETUP;
		TAILQ_INSERT_TAIL(&ip->target_list, record, target_entry);

		/*
		 * The inode now has a dependancy and must be taken out of
		 * the idle state.  An inode not in an idle state is given
		 * an extra reference.
		 */
		if (ip->flush_state == HAMMER_FST_IDLE) {
			hammer_ref(&ip->lock);
			ip->flush_state = HAMMER_FST_SETUP;
		}

		error = hammer_mem_add(trans, record);
	}

	/*
	 * One less link.  The file may still be open in the OS even after
	 * all links have gone away.
	 *
	 * We have to terminate the cursor before syncing the inode to
	 * avoid deadlocking against ourselves.  XXX this may no longer
	 * be true.
	 *
	 * If nlinks drops to zero and the vnode is inactive (or there is
	 * no vnode), call hammer_inode_unloadable_check() to zonk the
	 * inode.  If we don't do this here the inode will not be destroyed
	 * on-media until we unmount.
	 */
	if (error == 0) {
		--ip->ino_rec.ino_nlinks;
		hammer_modify_inode(trans, ip, HAMMER_INODE_RDIRTY);
		if (ip->ino_rec.ino_nlinks == 0 &&
		    (ip->vp == NULL || (ip->vp->v_flag & VINACTIVE))) {
			hammer_done_cursor(cursor);
			hammer_inode_unloadable_check(ip, 1);
			hammer_flush_inode(ip, 0);
		}

	}
	return(error);
}

/*
 * Add a record to an inode.
 *
 * The caller must allocate the record with hammer_alloc_mem_record(ip) and
 * initialize the following additional fields:
 *
 * The related inode should be share-locked by the caller.  The caller is
 * on the frontend.
 *
 * record->rec.entry.base.base.key
 * record->rec.entry.base.base.rec_type
 * record->rec.entry.base.base.data_len
 * record->data		(a copy will be kmalloc'd if it cannot be embedded)
 */
int
hammer_ip_add_record(struct hammer_transaction *trans, hammer_record_t record)
{
	hammer_inode_t ip = record->ip;
	int error;

	record->rec.base.base.obj_id = ip->obj_id;
	record->rec.base.base.obj_type = ip->ino_rec.base.base.obj_type;

	hammer_modify_inode(trans, ip, HAMMER_INODE_RDIRTY);

	/* NOTE: copies record->data */
	error = hammer_mem_add(trans, record);
	return(error);
}

/*
 * Sync data from a buffer cache buffer (typically) to the filesystem.  This
 * is called via the strategy called from a cached data source.  This code
 * is responsible for actually writing a data record out to the disk.
 *
 * This can only occur non-historically (i.e. 'current' data only).
 *
 * The file offset must be HAMMER_BUFSIZE aligned but the data length
 * can be truncated.  The record (currently) always represents a BUFSIZE
 * swath of space whether the data is truncated or not.
 */
int
hammer_ip_sync_data(hammer_cursor_t cursor, hammer_inode_t ip,
		       int64_t offset, void *data, int bytes)
{
	hammer_transaction_t trans = cursor->trans;
	hammer_record_ondisk_t rec;
	union hammer_btree_elm elm;
	hammer_off_t rec_offset, data_offset;
	void *bdata;
	int error;

	KKASSERT((offset & HAMMER_BUFMASK) == 0);
	KKASSERT(trans->type == HAMMER_TRANS_FLS);
	KKASSERT(bytes != 0);
retry:
	hammer_normalize_cursor(cursor);
	cursor->key_beg.obj_id = ip->obj_id;
	cursor->key_beg.key = offset + bytes;
	cursor->key_beg.create_tid = trans->tid;
	cursor->key_beg.delete_tid = 0;
	cursor->key_beg.rec_type = HAMMER_RECTYPE_DATA;
	cursor->asof = trans->tid;
	cursor->flags &= ~HAMMER_CURSOR_INITMASK;
	cursor->flags |= HAMMER_CURSOR_INSERT;
	cursor->flags |= HAMMER_CURSOR_BACKEND;

	/*
	 * Issue a lookup to position the cursor.
	 */
	error = hammer_btree_lookup(cursor);
	if (error == 0) {
		kprintf("hammer_ip_sync_data: duplicate data at "
			"(%lld,%d) tid %016llx\n",
			offset, bytes, trans->tid);
		hammer_print_btree_elm(&cursor->node->ondisk->
					elms[cursor->index],
				       HAMMER_BTREE_TYPE_LEAF, cursor->index);
		panic("Duplicate data");
		error = EIO;
	}
	if (error != ENOENT)
		goto done;

	/*
	 * Allocate record and data space.  HAMMER_RECTYPE_DATA records
	 * can cross buffer boundaries so we may have to split our bcopy.
	 */
	rec = hammer_alloc_record(trans, &rec_offset, HAMMER_RECTYPE_DATA,
				  &cursor->record_buffer,
				  bytes, &bdata,
				  &data_offset, &cursor->data_buffer, &error);
	if (rec == NULL)
		goto done;
	if (hammer_debug_general & 0x1000)
		kprintf("OOB RECOR2 DATA REC %016llx DATA %016llx LEN=%d\n", rec_offset, rec->base.data_off, rec->base.data_len);

	/*
	 * Fill everything in and insert our B-Tree node.
	 *
	 * NOTE: hammer_alloc_record() has already marked the related
	 * buffers as modified.  If we do it again we will generate
	 * unnecessary undo elements.
	 */
	hammer_modify_buffer(trans, cursor->record_buffer, NULL, 0);
	rec->base.base.btype = HAMMER_BTREE_TYPE_RECORD;
	rec->base.base.obj_id = ip->obj_id;
	rec->base.base.key = offset + bytes;
	rec->base.base.create_tid = trans->tid;
	rec->base.base.delete_tid = 0;
	rec->base.base.rec_type = HAMMER_RECTYPE_DATA;
	rec->base.data_crc = crc32(data, bytes);
	rec->base.signature = HAMMER_RECORD_SIGNATURE_GOOD;
	rec->base.rec_crc = crc32(&rec->base.data_crc,
				  HAMMER_RECORD_CRCSIZE);
	hammer_modify_buffer_done(cursor->record_buffer);
	KKASSERT(rec->base.data_len == bytes);

	hammer_modify_buffer(trans, cursor->data_buffer, NULL, 0);
	bcopy(data, bdata, bytes);
	hammer_modify_buffer_done(cursor->data_buffer);

	elm.leaf.base = rec->base.base;
	elm.leaf.rec_offset = rec_offset;
	elm.leaf.data_offset = rec->base.data_off;
	elm.leaf.data_len = bytes;
	elm.leaf.data_crc = rec->base.data_crc;

	/*
	 * Data records can wind up on-disk before the inode itself is
	 * on-disk.  One must assume data records may be on-disk if either
	 * HAMMER_INODE_DONDISK or HAMMER_INODE_ONDISK is set
	 */
	ip->flags |= HAMMER_INODE_DONDISK;

	error = hammer_btree_insert(cursor, &elm);
	if (error == 0)
		goto done;

	hammer_modify_buffer(trans, cursor->record_buffer, NULL, 0);
	rec->base.signature = HAMMER_RECORD_SIGNATURE_DESTROYED;
	hammer_modify_buffer_done(cursor->record_buffer);

	hammer_blockmap_free(trans, rec_offset, HAMMER_RECORD_SIZE);
	hammer_blockmap_free(trans, data_offset, bytes);
done:
	if (error == EDEADLK) {
		hammer_done_cursor(cursor);
		error = hammer_init_cursor(trans, cursor, &ip->cache[0], ip);
		if (error == 0)
			goto retry;
	}
	return(error);
}

#if 0
/*
 * Sync an in-memory record to the disk.  This is called by the backend.
 * This code is responsible for actually writing a record out to the disk.
 *
 * This routine can only be called by the backend and the record
 * must have been interlocked with BE.  It will remain interlocked on
 * return.  If no error occurs the record will be marked deleted but
 * the caller is responsible for its final disposition.
 *
 * Multiple calls may be aggregated with the same cursor using
 * hammer_ip_sync_record_cursor().  The caller must handle EDEADLK
 * in that case.
 */
int
hammer_ip_sync_record(hammer_transaction_t trans, hammer_record_t record)
{
	struct hammer_cursor cursor;
	int error;

	do {
		error = hammer_init_cursor(trans, &cursor,
					   &record->ip->cache[0], record->ip);
		if (error)
			return(error);
		error = hammer_ip_sync_record_cursor(&cursor, record);
		hammer_done_cursor(&cursor);
	} while (error == EDEADLK);
	return (error);
}

#endif

int
hammer_ip_sync_record_cursor(hammer_cursor_t cursor, hammer_record_t record)
{
	hammer_transaction_t trans = cursor->trans;
	hammer_record_ondisk_t rec;
	union hammer_btree_elm elm;
	hammer_off_t rec_offset;
	hammer_off_t data_offset;
	void *bdata;
	int error;

	KKASSERT(record->flush_state == HAMMER_FST_FLUSH);
	KKASSERT(record->flags & HAMMER_RECF_INTERLOCK_BE);
	KKASSERT(record->rec.base.signature == HAMMER_RECORD_SIGNATURE_GOOD);

	hammer_normalize_cursor(cursor);
	cursor->key_beg = record->rec.base.base;
	cursor->flags &= ~HAMMER_CURSOR_INITMASK;
	cursor->flags |= HAMMER_CURSOR_BACKEND;
	cursor->flags &= ~HAMMER_CURSOR_INSERT;

	/*
	 * If we are deleting an exact match must be found on-disk.
	 */
	if (record->type == HAMMER_MEM_RECORD_DEL) {
		error = hammer_btree_lookup(cursor);
		if (error == 0) {
			error = hammer_ip_delete_record(cursor, trans->tid);
			if (error == 0) {
				record->flags |= HAMMER_RECF_DELETED_FE;
				record->flags |= HAMMER_RECF_DELETED_BE;
			}
		}
		goto done;
	}

	/*
	 * We are inserting.
	 *
	 * Issue a lookup to position the cursor and locate the cluster.  The
	 * target key should not exist.  If we are creating a directory entry
	 * we may have to iterate the low 32 bits of the key to find an unused
	 * key.
	 */
	cursor->flags |= HAMMER_CURSOR_INSERT;

	for (;;) {
		error = hammer_btree_lookup(cursor);
		if (hammer_debug_inode)
			kprintf("DOINSERT LOOKUP %d\n", error);
		if (error)
			break;
		if (record->rec.base.base.rec_type != HAMMER_RECTYPE_DIRENTRY) {
			kprintf("hammer_ip_sync_record: duplicate rec "
				"at (%016llx)\n", record->rec.base.base.key);
			Debugger("duplicate record1");
			error = EIO;
			break;
		}
		if (++trans->hmp->namekey_iterator == 0)
			++trans->hmp->namekey_iterator;
		record->rec.base.base.key &= ~(0xFFFFFFFFLL);
		record->rec.base.base.key |= trans->hmp->namekey_iterator;
		cursor->key_beg.key = record->rec.base.base.key;
	}
	if (error != ENOENT)
		goto done;

	/*
	 * Allocate the record and data.  The result buffers will be
	 * marked as being modified and further calls to
	 * hammer_modify_buffer() will result in unneeded UNDO records.
	 *
	 * Support zero-fill records (data == NULL and data_len != 0)
	 */
	if (record->data == NULL) {
		rec = hammer_alloc_record(trans, &rec_offset,
					  record->rec.base.base.rec_type,
					  &cursor->record_buffer,
					  0, &bdata,
					  NULL, NULL, &error);
		if (hammer_debug_general & 0x1000)
			kprintf("NULL RECORD DATA\n");
	} else if (record->flags & HAMMER_RECF_INBAND) {
		rec = hammer_alloc_record(trans, &rec_offset,
					  record->rec.base.base.rec_type,
					  &cursor->record_buffer,
					  record->rec.base.data_len, &bdata,
					  NULL, NULL, &error);
		if (hammer_debug_general & 0x1000)
			kprintf("INBAND RECORD DATA %016llx DATA %016llx LEN=%d\n", rec_offset, rec->base.data_off, record->rec.base.data_len);
	} else {
		rec = hammer_alloc_record(trans, &rec_offset,
					  record->rec.base.base.rec_type,
					  &cursor->record_buffer,
					  record->rec.base.data_len, &bdata,
					  &data_offset,
					  &cursor->data_buffer, &error);
		if (hammer_debug_general & 0x1000)
			kprintf("OOB RECORD DATA REC %016llx DATA %016llx LEN=%d\n", rec_offset, rec->base.data_off, record->rec.base.data_len);
	}

	if (rec == NULL)
		goto done;

	/*
	 * Fill in the remaining fields and insert our B-Tree node.
	 */
	if (hammer_debug_inode)
		kprintf("COPYREC %p\n", rec);
	hammer_modify_buffer(trans, cursor->record_buffer, NULL, 0);
	rec->base.base = record->rec.base.base;
	bcopy(&record->rec.base + 1, &rec->base + 1,
	      HAMMER_RECORD_SIZE - sizeof(record->rec.base));

	/*
	 * Copy the data and deal with zero-fill support.
	 */
	if (record->data && (record->flags & HAMMER_RECF_INBAND)) {
		rec->base.data_crc = crc32(record->data, rec->base.data_len);
		bcopy(record->data, bdata, rec->base.data_len);
	} else if (record->data) {
		rec->base.data_crc = crc32(record->data, rec->base.data_len);
		hammer_modify_buffer(trans, cursor->data_buffer, NULL, 0);
		bcopy(record->data, bdata, rec->base.data_len);
		hammer_modify_buffer_done(cursor->data_buffer);
	} else {
		rec->base.data_len = record->rec.base.data_len;
	}
	rec->base.signature = HAMMER_RECORD_SIGNATURE_GOOD;
	rec->base.rec_crc = crc32(&rec->base.data_crc,
				  HAMMER_RECORD_CRCSIZE);
	hammer_modify_buffer_done(cursor->record_buffer);

	elm.leaf.base = record->rec.base.base;
	elm.leaf.rec_offset = rec_offset;
	elm.leaf.data_offset = rec->base.data_off;
	elm.leaf.data_len = rec->base.data_len;
	elm.leaf.data_crc = rec->base.data_crc;

	error = hammer_btree_insert(cursor, &elm);
	if (hammer_debug_inode)
		kprintf("BTREE INSERT error %d @ %016llx:%d\n", error, cursor->node->node_offset, cursor->index);

	/*
	 * This occurs when the frontend creates a record and queues it to
	 * the backend, then tries to delete the record.  The backend must
	 * still sync the record to the media as if it were not deleted,
	 * but must interlock with the frontend to ensure that the 
	 * synchronized record is not visible to the frontend, which means
	 * converting it from an ADD record to a DEL record.
	 *
	 * The DEL record then masks the record synced to disk until another
	 * round can delete it for real.
	 */
	if (error == 0) {
		if (record->flags & HAMMER_RECF_CONVERT_DELETE) {
			KKASSERT(record->type == HAMMER_MEM_RECORD_ADD);
			record->flags &= ~HAMMER_RECF_DELETED_FE;
			record->type = HAMMER_MEM_RECORD_DEL;
			KKASSERT(record->flush_state == HAMMER_FST_FLUSH);
			record->flags &= ~HAMMER_RECF_CONVERT_DELETE;
			/* hammer_flush_record_done takes care of the rest */
		} else {
			record->flags |= HAMMER_RECF_DELETED_FE;
			record->flags |= HAMMER_RECF_DELETED_BE;
		}
	} else {
		hammer_modify_buffer(trans, cursor->record_buffer, NULL, 0);
		rec->base.signature = HAMMER_RECORD_SIGNATURE_DESTROYED;
		hammer_modify_buffer_done(cursor->record_buffer);
		hammer_blockmap_free(trans, rec_offset, HAMMER_RECORD_SIZE);
		if (record->data && (record->flags & HAMMER_RECF_INBAND) == 0) {
			hammer_blockmap_free(trans, data_offset,
					     record->rec.base.data_len);
		}
	}

done:
	return(error);
}

/*
 * Add the record to the inode's rec_tree.  The low 32 bits of a directory
 * entry's key is used to deal with hash collisions in the upper 32 bits.
 * A unique 64 bit key is generated in-memory and may be regenerated a
 * second time when the directory record is flushed to the on-disk B-Tree.
 *
 * A referenced record is passed to this function.  This function
 * eats the reference.  If an error occurs the record will be deleted.
 *
 * A copy of the temporary record->data pointer provided by the caller
 * will be made.
 */
static
int
hammer_mem_add(struct hammer_transaction *trans, hammer_record_t record)
{
	void *data;
	int bytes;
	int reclen;
		
	KKASSERT(record->rec.base.signature == HAMMER_RECORD_SIGNATURE_GOOD);

	/*
	 * Make a private copy of record->data
	 */
	if (record->data) {
		/*
		 * Try to embed the data in extra space in the record
		 * union, otherwise allocate a copy.
		 */
		bytes = record->rec.base.data_len;
		switch(record->rec.base.base.rec_type) {
		case HAMMER_RECTYPE_DIRENTRY:
			reclen = offsetof(struct hammer_entry_record, name[0]);
			break;
		case HAMMER_RECTYPE_DATA:
			reclen = offsetof(struct hammer_data_record, data[0]);
			break;
		default:
			reclen = sizeof(record->rec);
			break;
		}
		if (reclen + bytes <= HAMMER_RECORD_SIZE) {
			bcopy(record->data, (char *)&record->rec + reclen,
			      bytes);
			record->data = (void *)((char *)&record->rec + reclen);
			record->flags |= HAMMER_RECF_INBAND;
		} else {
			++hammer_count_record_datas;
			data = kmalloc(bytes, M_HAMMER, M_WAITOK);
			record->flags |= HAMMER_RECF_ALLOCDATA;
			bcopy(record->data, data, bytes);
			record->data = data;
		}
	}

	/*
	 * Insert into the RB tree, find an unused iterator if this is
	 * a directory entry.
	 */
	while (RB_INSERT(hammer_rec_rb_tree, &record->ip->rec_tree, record)) {
		if (record->rec.base.base.rec_type != HAMMER_RECTYPE_DIRENTRY){
			record->flags |= HAMMER_RECF_DELETED_FE;
			hammer_rel_mem_record(record);
			return (EEXIST);
		}
		if (++trans->hmp->namekey_iterator == 0)
			++trans->hmp->namekey_iterator;
		record->rec.base.base.key &= ~(0xFFFFFFFFLL);
		record->rec.base.base.key |= trans->hmp->namekey_iterator;
	}
	record->flags |= HAMMER_RECF_ONRBTREE;
	hammer_modify_inode(trans, record->ip, HAMMER_INODE_XDIRTY);
	hammer_rel_mem_record(record);
	return(0);
}

/************************************************************************
 *		     HAMMER INODE MERGED-RECORD FUNCTIONS		*
 ************************************************************************
 *
 * These functions augment the B-Tree scanning functions in hammer_btree.c
 * by merging in-memory records with on-disk records.
 */

/*
 * Locate a particular record either in-memory or on-disk.
 *
 * NOTE: This is basically a standalone routine, hammer_ip_next() may
 * NOT be called to iterate results.
 */
int
hammer_ip_lookup(hammer_cursor_t cursor, struct hammer_inode *ip)
{
	int error;

	/*
	 * If the element is in-memory return it without searching the
	 * on-disk B-Tree
	 */
	error = hammer_mem_lookup(cursor, ip);
	if (error == 0) {
		cursor->record = &cursor->iprec->rec;
		return(error);
	}
	if (error != ENOENT)
		return(error);

	/*
	 * If the inode has on-disk components search the on-disk B-Tree.
	 */
	if ((ip->flags & (HAMMER_INODE_ONDISK|HAMMER_INODE_DONDISK)) == 0)
		return(error);
	error = hammer_btree_lookup(cursor);
	if (error == 0)
		error = hammer_btree_extract(cursor, HAMMER_CURSOR_GET_RECORD);
	return(error);
}

/*
 * Locate the first record within the cursor's key_beg/key_end range,
 * restricted to a particular inode.  0 is returned on success, ENOENT
 * if no records matched the requested range, or some other error.
 *
 * When 0 is returned hammer_ip_next() may be used to iterate additional
 * records within the requested range.
 *
 * This function can return EDEADLK, requiring the caller to terminate
 * the cursor and try again.
 */
int
hammer_ip_first(hammer_cursor_t cursor)
{
	hammer_inode_t ip = cursor->ip;
	int error;

	KKASSERT(ip != NULL);

	/*
	 * Clean up fields and setup for merged scan
	 */
	cursor->flags &= ~HAMMER_CURSOR_DELBTREE;
	cursor->flags |= HAMMER_CURSOR_ATEDISK | HAMMER_CURSOR_ATEMEM;
	cursor->flags |= HAMMER_CURSOR_DISKEOF | HAMMER_CURSOR_MEMEOF;
	if (cursor->iprec) {
		hammer_rel_mem_record(cursor->iprec);
		cursor->iprec = NULL;
	}

	/*
	 * Search the on-disk B-Tree.  hammer_btree_lookup() only does an
	 * exact lookup so if we get ENOENT we have to call the iterate
	 * function to validate the first record after the begin key.
	 *
	 * The ATEDISK flag is used by hammer_btree_iterate to determine
	 * whether it must index forwards or not.  It is also used here
	 * to select the next record from in-memory or on-disk.
	 *
	 * EDEADLK can only occur if the lookup hit an empty internal
	 * element and couldn't delete it.  Since this could only occur
	 * in-range, we can just iterate from the failure point.
	 */
	if (ip->flags & (HAMMER_INODE_ONDISK|HAMMER_INODE_DONDISK)) {
		error = hammer_btree_lookup(cursor);
		if (error == ENOENT || error == EDEADLK) {
			cursor->flags &= ~HAMMER_CURSOR_ATEDISK;
			if (hammer_debug_general & 0x2000)
				kprintf("error %d node %p %016llx index %d\n", error, cursor->node, cursor->node->node_offset, cursor->index);
			error = hammer_btree_iterate(cursor);
		}
		if (error && error != ENOENT) 
			return(error);
		if (error == 0) {
			cursor->flags &= ~HAMMER_CURSOR_DISKEOF;
			cursor->flags &= ~HAMMER_CURSOR_ATEDISK;
		} else {
			cursor->flags |= HAMMER_CURSOR_ATEDISK;
		}
	}

	/*
	 * Search the in-memory record list (Red-Black tree).  Unlike the
	 * B-Tree search, mem_first checks for records in the range.
	 */
	error = hammer_mem_first(cursor);
	if (error && error != ENOENT)
		return(error);
	if (error == 0) {
		cursor->flags &= ~HAMMER_CURSOR_MEMEOF;
		cursor->flags &= ~HAMMER_CURSOR_ATEMEM;
		if (hammer_ip_iterate_mem_good(cursor, cursor->iprec) == 0)
			cursor->flags |= HAMMER_CURSOR_ATEMEM;
	}

	/*
	 * This will return the first matching record.
	 */
	return(hammer_ip_next(cursor));
}

/*
 * Retrieve the next record in a merged iteration within the bounds of the
 * cursor.  This call may be made multiple times after the cursor has been
 * initially searched with hammer_ip_first().
 *
 * 0 is returned on success, ENOENT if no further records match the
 * requested range, or some other error code is returned.
 */
int
hammer_ip_next(hammer_cursor_t cursor)
{
	hammer_btree_elm_t elm;
	hammer_record_t rec, save;
	int error;
	int r;

next_btree:
	/*
	 * Load the current on-disk and in-memory record.  If we ate any
	 * records we have to get the next one. 
	 *
	 * If we deleted the last on-disk record we had scanned ATEDISK will
	 * be clear and DELBTREE will be set, forcing a call to iterate. The
	 * fact that ATEDISK is clear causes iterate to re-test the 'current'
	 * element.  If ATEDISK is set, iterate will skip the 'current'
	 * element.
	 *
	 * Get the next on-disk record
	 */
	if (cursor->flags & (HAMMER_CURSOR_ATEDISK|HAMMER_CURSOR_DELBTREE)) {
		if ((cursor->flags & HAMMER_CURSOR_DISKEOF) == 0) {
			error = hammer_btree_iterate(cursor);
			cursor->flags &= ~HAMMER_CURSOR_DELBTREE;
			if (error == 0)
				cursor->flags &= ~HAMMER_CURSOR_ATEDISK;
			else
				cursor->flags |= HAMMER_CURSOR_DISKEOF |
						 HAMMER_CURSOR_ATEDISK;
		}
	}

next_memory:
	/*
	 * Get the next in-memory record.  The record can be ripped out
	 * of the RB tree so we maintain a scan_info structure to track
	 * the next node.
	 *
	 * hammer_rec_scan_cmp:  Is the record still in our general range,
	 *			 (non-inclusive of snapshot exclusions)?
	 * hammer_rec_scan_callback: Is the record in our snapshot?
	 */
	if (cursor->flags & HAMMER_CURSOR_ATEMEM) {
		if ((cursor->flags & HAMMER_CURSOR_MEMEOF) == 0) {
			save = cursor->iprec;
			cursor->iprec = NULL;
			rec = save ? hammer_rec_rb_tree_RB_NEXT(save) : NULL;
			while (rec) {
				if (hammer_rec_scan_cmp(rec, cursor) != 0)
					break;
				if (hammer_rec_scan_callback(rec, cursor) != 0)
					break;
				rec = hammer_rec_rb_tree_RB_NEXT(rec);
			}
			if (save)
				hammer_rel_mem_record(save);
			if (cursor->iprec) {
				KKASSERT(cursor->iprec == rec);
				cursor->flags &= ~HAMMER_CURSOR_ATEMEM;
			} else {
				cursor->flags |= HAMMER_CURSOR_MEMEOF;
			}
		}
	}

	/*
	 * The memory record may have become stale while being held in
	 * cursor->iprec.  We are interlocked against the backend on 
	 * with regards to B-Tree entries.
	 */
	if ((cursor->flags & HAMMER_CURSOR_ATEMEM) == 0) {
		if (hammer_ip_iterate_mem_good(cursor, cursor->iprec) == 0) {
			cursor->flags |= HAMMER_CURSOR_ATEMEM;
			goto next_memory;
		}
	}

	/*
	 * Extract either the disk or memory record depending on their
	 * relative position.
	 */
	error = 0;
	switch(cursor->flags & (HAMMER_CURSOR_ATEDISK | HAMMER_CURSOR_ATEMEM)) {
	case 0:
		/*
		 * Both entries valid.   Return the btree entry if it is
		 * in front of the memory entry.
		 */
		elm = &cursor->node->ondisk->elms[cursor->index];
		r = hammer_btree_cmp(&elm->base, &cursor->iprec->rec.base.base);
		if (r < 0) {
			error = hammer_btree_extract(cursor,
						     HAMMER_CURSOR_GET_RECORD);
			cursor->flags |= HAMMER_CURSOR_ATEDISK;
			break;
		}

		/*
		 * If the entries match exactly the memory entry typically
		 * specifies an on-disk deletion and we eat both entries.
		 *
		 * If the in-memory record is not an on-disk deletion we
		 * probably caught the syncer while it was syncing it to
		 * the media.  Since we hold a shared lock on the cursor,
		 * the in-memory record had better be marked deleted at
		 * this point.
		 */
		if (r == 0) {
			if (cursor->iprec->type == HAMMER_MEM_RECORD_DEL) {
				if ((cursor->flags & HAMMER_CURSOR_DELETE_VISIBILITY) == 0) {
					cursor->flags |= HAMMER_CURSOR_ATEDISK;
					cursor->flags |= HAMMER_CURSOR_ATEMEM;
					goto next_btree;
				}
			} else {
				panic("hammer_ip_next: duplicate mem/b-tree entry");
				cursor->flags |= HAMMER_CURSOR_ATEMEM;
				goto next_memory;
			}
		}
		/* fall through to the memory entry */
	case HAMMER_CURSOR_ATEDISK:
		/*
		 * Only the memory entry is valid.
		 */
		cursor->record = &cursor->iprec->rec;
		cursor->flags |= HAMMER_CURSOR_ATEMEM;

		/*
		 * If the memory entry is an on-disk deletion we should have
		 * also had found a B-Tree record.  If the backend beat us
		 * to it it would have interlocked the cursor and we should
		 * have seen the in-memory record marked DELETED_FE.
		 */
		if (cursor->iprec->type == HAMMER_MEM_RECORD_DEL) {
			panic("hammer_ip_next: del-on-disk with no b-tree entry");
		}
		break;
	case HAMMER_CURSOR_ATEMEM:
		/*
		 * Only the disk entry is valid
		 */
		error = hammer_btree_extract(cursor, HAMMER_CURSOR_GET_RECORD);
		cursor->flags |= HAMMER_CURSOR_ATEDISK;
		break;
	default:
		/*
		 * Neither entry is valid
		 *
		 * XXX error not set properly
		 */
		cursor->record = NULL;
		error = ENOENT;
		break;
	}
	return(error);
}

/*
 * Resolve the cursor->data pointer for the current cursor position in
 * a merged iteration.
 */
int
hammer_ip_resolve_data(hammer_cursor_t cursor)
{
	int error;

	if (cursor->iprec && cursor->record == &cursor->iprec->rec) {
		cursor->data = cursor->iprec->data;
		error = 0;
	} else {
		error = hammer_btree_extract(cursor, HAMMER_CURSOR_GET_DATA);
	}
	return(error);
}

int
hammer_ip_resolve_record_and_data(hammer_cursor_t cursor)
{
	int error;

	if (cursor->iprec && cursor->record == &cursor->iprec->rec) {
		cursor->data = cursor->iprec->data;
		error = 0;
	} else {
		error = hammer_btree_extract(cursor, HAMMER_CURSOR_GET_DATA |
						     HAMMER_CURSOR_GET_RECORD);
	}
	return(error);
}

/*
 * Delete all records within the specified range for inode ip.
 *
 * NOTE: An unaligned range will cause new records to be added to cover
 * the edge cases. (XXX not implemented yet).
 *
 * NOTE: ran_end is inclusive (e.g. 0,1023 instead of 0,1024).
 *
 * NOTE: Record keys for regular file data have to be special-cased since
 * they indicate the end of the range (key = base + bytes).
 */
int
hammer_ip_delete_range(hammer_cursor_t cursor, hammer_inode_t ip,
		       int64_t ran_beg, int64_t ran_end)
{
	hammer_transaction_t trans = cursor->trans;
	hammer_record_ondisk_t rec;
	hammer_base_elm_t base;
	int error;
	int64_t off;

#if 0
	kprintf("delete_range %p %016llx-%016llx\n", ip, ran_beg, ran_end);
#endif

	KKASSERT(trans->type == HAMMER_TRANS_FLS);
retry:
	hammer_normalize_cursor(cursor);
	cursor->key_beg.obj_id = ip->obj_id;
	cursor->key_beg.create_tid = 0;
	cursor->key_beg.delete_tid = 0;
	cursor->key_beg.obj_type = 0;
	cursor->asof = ip->obj_asof;
	cursor->flags &= ~HAMMER_CURSOR_INITMASK;
	cursor->flags |= HAMMER_CURSOR_ASOF;
	cursor->flags |= HAMMER_CURSOR_DELETE_VISIBILITY;
	cursor->flags |= HAMMER_CURSOR_BACKEND;

	cursor->key_end = cursor->key_beg;
	if (ip->ino_rec.base.base.obj_type == HAMMER_OBJTYPE_DBFILE) {
		cursor->key_beg.key = ran_beg;
		cursor->key_beg.rec_type = HAMMER_RECTYPE_DB;
		cursor->key_end.rec_type = HAMMER_RECTYPE_DB;
		cursor->key_end.key = ran_end;
	} else {
		/*
		 * The key in the B-Tree is (base+bytes), so the first possible
		 * matching key is ran_beg + 1.
		 */
		int64_t tmp64;

		cursor->key_beg.key = ran_beg + 1;
		cursor->key_beg.rec_type = HAMMER_RECTYPE_DATA;
		cursor->key_end.rec_type = HAMMER_RECTYPE_DATA;

		tmp64 = ran_end + MAXPHYS + 1;	/* work around GCC-4 bug */
		if (tmp64 < ran_end)
			cursor->key_end.key = 0x7FFFFFFFFFFFFFFFLL;
		else
			cursor->key_end.key = ran_end + MAXPHYS + 1;
	}
	cursor->flags |= HAMMER_CURSOR_END_INCLUSIVE;

	error = hammer_ip_first(cursor);

	/*
	 * Iterate through matching records and mark them as deleted.
	 */
	while (error == 0) {
		rec = cursor->record;
		base = &rec->base.base;

		KKASSERT(base->delete_tid == 0);

		/*
		 * There may be overlap cases for regular file data.  Also
		 * remember the key for a regular file record is the offset
		 * of the last byte of the record (base + len - 1), NOT the
		 * base offset.
		 */
#if 0
		kprintf("delete_range rec_type %02x\n", base->rec_type);
#endif
		if (base->rec_type == HAMMER_RECTYPE_DATA) {
#if 0
			kprintf("delete_range loop key %016llx,%d\n",
				base->key - rec->base.data_len, rec->base.data_len);
#endif
			off = base->key - rec->base.data_len;
			/*
			 * Check the left edge case.  We currently do not
			 * split existing records.
			 */
			if (off < ran_beg) {
				panic("hammer left edge case %016llx %d\n",
					base->key, rec->base.data_len);
			}

			/*
			 * Check the right edge case.  Note that the
			 * record can be completely out of bounds, which
			 * terminates the search.
			 *
			 * base->key is exclusive of the right edge while
			 * ran_end is inclusive of the right edge.  The
			 * (key - data_len) left boundary is inclusive.
			 *
			 * XXX theory-check this test at some point, are
			 * we missing a + 1 somewhere?  Note that ran_end
			 * could overflow.
			 */
			if (base->key - 1 > ran_end) {
				if (base->key - rec->base.data_len > ran_end)
					break;
				panic("hammer right edge case\n");
			}
		}

		/*
		 * Mark the record and B-Tree entry as deleted.  This will
		 * also physically delete the B-Tree entry, record, and
		 * data if the retention policy dictates.  The function
		 * will set HAMMER_CURSOR_DELBTREE which hammer_ip_next()
		 * uses to perform a fixup.
		 */
		error = hammer_ip_delete_record(cursor, trans->tid);
		if (error)
			break;
		error = hammer_ip_next(cursor);
	}
	if (error == EDEADLK) {
		hammer_done_cursor(cursor);
		error = hammer_init_cursor(trans, cursor, &ip->cache[0], ip);
		if (error == 0)
			goto retry;
	}
	if (error == ENOENT)
		error = 0;
	return(error);
}

/*
 * Delete all user records associated with an inode except the inode record
 * itself.  Directory entries are not deleted (they must be properly disposed
 * of or nlinks would get upset).
 */
int
hammer_ip_delete_range_all(hammer_cursor_t cursor, hammer_inode_t ip,
			   int *countp)
{
	hammer_transaction_t trans = cursor->trans;
	hammer_record_ondisk_t rec;
	hammer_base_elm_t base;
	int error;

	KKASSERT(trans->type == HAMMER_TRANS_FLS);
retry:
	hammer_normalize_cursor(cursor);
	cursor->key_beg.obj_id = ip->obj_id;
	cursor->key_beg.create_tid = 0;
	cursor->key_beg.delete_tid = 0;
	cursor->key_beg.obj_type = 0;
	cursor->key_beg.rec_type = HAMMER_RECTYPE_INODE + 1;
	cursor->key_beg.key = HAMMER_MIN_KEY;

	cursor->key_end = cursor->key_beg;
	cursor->key_end.rec_type = 0xFFFF;
	cursor->key_end.key = HAMMER_MAX_KEY;

	cursor->asof = ip->obj_asof;
	cursor->flags &= ~HAMMER_CURSOR_INITMASK;
	cursor->flags |= HAMMER_CURSOR_END_INCLUSIVE | HAMMER_CURSOR_ASOF;
	cursor->flags |= HAMMER_CURSOR_DELETE_VISIBILITY;
	cursor->flags |= HAMMER_CURSOR_BACKEND;

	error = hammer_ip_first(cursor);

	/*
	 * Iterate through matching records and mark them as deleted.
	 */
	while (error == 0) {
		rec = cursor->record;
		base = &rec->base.base;

		KKASSERT(base->delete_tid == 0);

		/*
		 * Mark the record and B-Tree entry as deleted.  This will
		 * also physically delete the B-Tree entry, record, and
		 * data if the retention policy dictates.  The function
		 * will set HAMMER_CURSOR_DELBTREE which hammer_ip_next()
		 * uses to perform a fixup.
		 *
		 * Directory entries (and delete-on-disk directory entries)
		 * must be synced and cannot be deleted.
		 */
		if (rec->base.base.rec_type != HAMMER_RECTYPE_DIRENTRY) {
			error = hammer_ip_delete_record(cursor, trans->tid);
			++*countp;
		}
		if (error)
			break;
		error = hammer_ip_next(cursor);
	}
	if (error == EDEADLK) {
		hammer_done_cursor(cursor);
		error = hammer_init_cursor(trans, cursor, &ip->cache[0], ip);
		if (error == 0)
			goto retry;
	}
	if (error == ENOENT)
		error = 0;
	return(error);
}

/*
 * Delete the record at the current cursor.  On success the cursor will
 * be positioned appropriately for an iteration but may no longer be at
 * a leaf node.
 *
 * This routine is only called from the backend.
 *
 * NOTE: This can return EDEADLK, requiring the caller to terminate the
 * cursor and retry.
 */
int
hammer_ip_delete_record(hammer_cursor_t cursor, hammer_tid_t tid)
{
	hammer_btree_elm_t elm;
	hammer_mount_t hmp;
	int error;
	int dodelete;

	KKASSERT(cursor->flags & HAMMER_CURSOR_BACKEND);

	/*
	 * In-memory (unsynchronized) records can simply be freed.  This
	 * only occurs in range iterations since all other records are
	 * individually synchronized.  Thus there should be no confusion with
	 * the interlock.
	 */
	if (cursor->record == &cursor->iprec->rec) {
		KKASSERT((cursor->iprec->flags & HAMMER_RECF_INTERLOCK_BE) ==0);
		cursor->iprec->flags |= HAMMER_RECF_DELETED_FE;
		cursor->iprec->flags |= HAMMER_RECF_DELETED_BE;
		return(0);
	}

	/*
	 * On-disk records are marked as deleted by updating their delete_tid.
	 * This does not effect their position in the B-Tree (which is based
	 * on their create_tid).
	 */
	error = hammer_btree_extract(cursor, HAMMER_CURSOR_GET_RECORD);
	elm = NULL;
	hmp = cursor->node->hmp;

	/*
	 * If we were mounted with the nohistory option, we physically
	 * delete the record.
	 */
	if (hmp->hflags & HMNT_NOHISTORY)
		dodelete = 1;
	else
		dodelete = 0;

	if (error == 0) {
		error = hammer_cursor_upgrade(cursor);
		if (error == 0) {
			elm = &cursor->node->ondisk->elms[cursor->index];
			hammer_modify_node(cursor->trans, cursor->node,
					   &elm->leaf.base.delete_tid,
					   sizeof(elm->leaf.base.delete_tid));
			elm->leaf.base.delete_tid = tid;
			hammer_modify_node_done(cursor->node);

			/*
			 * An on-disk record cannot have the same delete_tid
			 * as its create_tid.  In a chain of record updates
			 * this could result in a duplicate record.
			 */
			KKASSERT(elm->leaf.base.delete_tid != elm->leaf.base.create_tid);

			hammer_modify_record_field(cursor->trans, cursor->record_buffer, cursor->record, base.base.delete_tid, dodelete);
			cursor->record->base.base.delete_tid = tid;
			if (dodelete) {
				cursor->record->base.signature =
					    HAMMER_RECORD_SIGNATURE_DESTROYED;
			}
			hammer_modify_record_done(cursor->record_buffer,
						  cursor->record);
		}
	}

	if (error == 0 && dodelete) {
		error = hammer_delete_at_cursor(cursor, NULL);
		if (error) {
			panic("hammer_ip_delete_record: unable to physically delete the record!\n");
			error = 0;
		}
	}
	return(error);
}

int
hammer_delete_at_cursor(hammer_cursor_t cursor, int64_t *stat_bytes)
{
	hammer_btree_elm_t elm;
	hammer_off_t rec_offset;
	hammer_off_t data_offset;
	int32_t data_len;
	u_int16_t rec_type;
	int error;

	elm = &cursor->node->ondisk->elms[cursor->index];
	KKASSERT(elm->base.btype == HAMMER_BTREE_TYPE_RECORD);

	rec_offset = elm->leaf.rec_offset;
	data_offset = elm->leaf.data_offset;
	data_len = elm->leaf.data_len;
	rec_type = elm->leaf.base.rec_type;

	error = hammer_btree_delete(cursor);
	if (error == 0) {
		/*
		 * This forces a fixup for the iteration because
		 * the cursor is now either sitting at the 'next'
		 * element or sitting at the end of a leaf.
		 */
		if ((cursor->flags & HAMMER_CURSOR_DISKEOF) == 0) {
			cursor->flags |= HAMMER_CURSOR_DELBTREE;
			cursor->flags &= ~HAMMER_CURSOR_ATEDISK;
		}
	}
	if (error == 0) {
		hammer_blockmap_free(cursor->trans, rec_offset,
				     sizeof(union hammer_record_ondisk));
	}
	if (error == 0) {
		switch(data_offset & HAMMER_OFF_ZONE_MASK) {
		case HAMMER_ZONE_LARGE_DATA:
		case HAMMER_ZONE_SMALL_DATA:
			hammer_blockmap_free(cursor->trans,
					     data_offset, data_len);
			break;
		default:
			break;
		}
	}
#if 0
	kprintf("hammer_delete_at_cursor: %d:%d:%08x %08x/%d "
		"(%d remain in cluster)\n",
		cluster->volume->vol_no, cluster->clu_no,
		rec_offset, data_offset, data_len,
		cluster->ondisk->stat_records);
#endif
	return (error);
}

/*
 * Determine whether we can remove a directory.  This routine checks whether
 * a directory is empty or not and enforces flush connectivity.
 *
 * Flush connectivity requires that we block if the target directory is
 * currently flushing, otherwise it may not end up in the same flush group.
 *
 * Returns 0 on success, ENOTEMPTY or EDEADLK (or other errors) on failure.
 */
int
hammer_ip_check_directory_empty(hammer_transaction_t trans,
			hammer_cursor_t parent_cursor, hammer_inode_t ip)
{
	struct hammer_cursor cursor;
	int error;

	/*
	 * Check directory empty
	 */
	hammer_init_cursor(trans, &cursor, &ip->cache[0], ip);

	cursor.key_beg.obj_id = ip->obj_id;
	cursor.key_beg.create_tid = 0;
	cursor.key_beg.delete_tid = 0;
	cursor.key_beg.obj_type = 0;
	cursor.key_beg.rec_type = HAMMER_RECTYPE_INODE + 1;
	cursor.key_beg.key = HAMMER_MIN_KEY;

	cursor.key_end = cursor.key_beg;
	cursor.key_end.rec_type = 0xFFFF;
	cursor.key_end.key = HAMMER_MAX_KEY;

	cursor.asof = ip->obj_asof;
	cursor.flags |= HAMMER_CURSOR_END_INCLUSIVE | HAMMER_CURSOR_ASOF;

	error = hammer_ip_first(&cursor);
	if (error == ENOENT)
		error = 0;
	else if (error == 0)
		error = ENOTEMPTY;
	hammer_done_cursor(&cursor);
	return(error);
}

