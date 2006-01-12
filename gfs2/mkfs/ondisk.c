/******************************************************************************
*******************************************************************************
**
**  Copyright (C) Sistina Software, Inc.  1997-2003  All rights reserved.
**  Copyright (C) 2004 Red Hat, Inc.  All rights reserved.
**
**  This copyrighted material is made available to anyone wishing to use,
**  modify, copy, or redistribute it subject to the terms and conditions
**  of the GNU General Public License v.2.
**
*******************************************************************************
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include <linux/types.h>
#include "gfs2_mkfs.h"

#include "gfs2_disk_hash.h"


#define CPIN_08(s1, s2, member, count) {memcpy((s1->member), (s2->member), (count));}
#define CPOUT_08(s1, s2, member, count) {memcpy((s2->member), (s1->member), (count));}
#define CPIN_16(s1, s2, member) {(s1->member) = be16_to_cpu((s2->member));}
#define CPOUT_16(s1, s2, member) {(s2->member) = cpu_to_be16((s1->member));}
#define CPIN_32(s1, s2, member) {(s1->member) = be32_to_cpu((s2->member));}
#define CPOUT_32(s1, s2, member) {(s2->member) = cpu_to_be32((s1->member));}
#define CPIN_64(s1, s2, member) {(s1->member) = be64_to_cpu((s2->member));}
#define CPOUT_64(s1, s2, member) {(s2->member) = cpu_to_be64((s1->member));}

#define pv(struct, member, fmt) printf("  "#member" = "fmt"\n", struct->member);
#define pa(struct, member, count) print_array(#member, struct->member, count);

/**
 * print_array - Print out an array of bytes
 * @title: what to print before the array
 * @buf: the array
 * @count: the number of bytes
 *
 */

static void print_array(char *title, unsigned char *buf, int count)
{
	int x;

	printf("  %s =\n", title);
	for (x = 0; x < count; x++) {
		printf("%.2X ", (unsigned char)buf[x]);
		if (x % 16 == 15)
			printf("\n");
	}
	if (x % 16)
		printf("\n");
}

/*
 * gfs2_xxx_in - read in an xxx struct
 * first arg: the cpu-order structure
 * buf: the disk-order buffer
 *
 * gfs2_xxx_out - write out an xxx struct
 * first arg: the cpu-order structure
 * buf: the disk-order buffer
 *
 * gfs2_xxx_print - print out an xxx struct
 * first arg: the cpu-order structure
 */

void gfs2_inum_in(struct gfs2_inum *no, char *buf)
{
	struct gfs2_inum *str = (struct gfs2_inum *)buf;

	CPIN_64(no, str, no_formal_ino);
	CPIN_64(no, str, no_addr);
}

void gfs2_inum_out(struct gfs2_inum *no, char *buf)
{
	struct gfs2_inum *str = (struct gfs2_inum *)buf;

	CPOUT_64(no, str, no_formal_ino);
	CPOUT_64(no, str, no_addr);
}

void gfs2_inum_print(struct gfs2_inum *no)
{
	pv(no, no_formal_ino, "%llu");
	pv(no, no_addr, "%llu");
}

void gfs2_meta_header_in(struct gfs2_meta_header *mh, char *buf)
{
	struct gfs2_meta_header *str = (struct gfs2_meta_header *)buf;

	CPIN_32(mh, str, mh_magic);
	CPIN_16(mh, str, mh_type);
	CPIN_16(mh, str, mh_format);
}

void gfs2_meta_header_out(struct gfs2_meta_header *mh, char *buf)
{
	struct gfs2_meta_header *str = (struct gfs2_meta_header *)buf;

	CPOUT_32(mh, str, mh_magic);
	CPOUT_16(mh, str, mh_type);
	CPOUT_16(mh, str, mh_format);
	str->__pad0 = 0;
	str->__pad1 = 0;
}

void gfs2_meta_header_print(struct gfs2_meta_header *mh)
{
	pv(mh, mh_magic, "0x%.8X");
	pv(mh, mh_type, "%u");
	pv(mh, mh_format, "%u");
}

void gfs2_sb_in(struct gfs2_sb *sb, char *buf)
{
	struct gfs2_sb *str = (struct gfs2_sb *)buf;

	gfs2_meta_header_in(&sb->sb_header, buf);

	CPIN_32(sb, str, sb_fs_format);
	CPIN_32(sb, str, sb_multihost_format);

	CPIN_32(sb, str, sb_bsize);
	CPIN_32(sb, str, sb_bsize_shift);

	gfs2_inum_in(&sb->sb_master_dir, (char *)&str->sb_master_dir);
	gfs2_inum_in(&sb->sb_root_dir, (char *)&str->sb_root_dir);

	CPIN_08(sb, str, sb_lockproto, GFS2_LOCKNAME_LEN);
	CPIN_08(sb, str, sb_locktable, GFS2_LOCKNAME_LEN);
}

void gfs2_sb_out(struct gfs2_sb *sb, char *buf)
{
	struct gfs2_sb *str = (struct gfs2_sb *)buf;

	gfs2_meta_header_out(&sb->sb_header, buf);

	CPOUT_32(sb, str, sb_fs_format);
	CPOUT_32(sb, str, sb_multihost_format);

	CPOUT_32(sb, str, sb_bsize);
	CPOUT_32(sb, str, sb_bsize_shift);

	gfs2_inum_out(&sb->sb_master_dir, (char *)&str->sb_master_dir);
	gfs2_inum_out(&sb->sb_root_dir, (char *)&str->sb_root_dir);

	CPOUT_08(sb, str, sb_lockproto, GFS2_LOCKNAME_LEN);
	CPOUT_08(sb, str, sb_locktable, GFS2_LOCKNAME_LEN);
}

void gfs2_sb_print(struct gfs2_sb *sb)
{
	gfs2_meta_header_print(&sb->sb_header);

	pv(sb, sb_fs_format, "%u");
	pv(sb, sb_multihost_format, "%u");

	pv(sb, sb_bsize, "%u");
	pv(sb, sb_bsize_shift, "%u");

	gfs2_inum_print(&sb->sb_master_dir);
	gfs2_inum_print(&sb->sb_root_dir);

	pv(sb, sb_lockproto, "%s");
	pv(sb, sb_locktable, "%s");
}

void gfs2_rindex_in(struct gfs2_rindex *ri, char *buf)
{
	struct gfs2_rindex *str = (struct gfs2_rindex *)buf;

	CPIN_64(ri, str, ri_addr);
	CPIN_32(ri, str, ri_length);

	CPIN_64(ri, str, ri_data0);
	CPIN_32(ri, str, ri_data);

	CPIN_32(ri, str, ri_bitbytes);

	CPIN_08(ri, str, ri_reserved, 32);
}

void gfs2_rindex_out(struct gfs2_rindex *ri, char *buf)
{
	struct gfs2_rindex *str = (struct gfs2_rindex *)buf;

	CPOUT_64(ri, str, ri_addr);
	CPOUT_32(ri, str, ri_length);
	ri->__pad = 0;

	CPOUT_64(ri, str, ri_data0);
	CPOUT_32(ri, str, ri_data);

	CPOUT_32(ri, str, ri_bitbytes);

	CPOUT_08(ri, str, ri_reserved, 32);
}

void gfs2_rindex_print(struct gfs2_rindex *ri)
{
	pv(ri, ri_addr, "%llu");
	pv(ri, ri_length, "%u");

	pv(ri, ri_data0, "%llu");
	pv(ri, ri_data, "%u");

	pv(ri, ri_bitbytes, "%u");

	pa(ri, ri_reserved, 32);
}

void gfs2_rgrp_in(struct gfs2_rgrp *rg, char *buf)
{
	struct gfs2_rgrp *str = (struct gfs2_rgrp *)buf;

	gfs2_meta_header_in(&rg->rg_header, buf);
	CPIN_32(rg, str, rg_flags);
	CPIN_32(rg, str, rg_free);
	CPIN_32(rg, str, rg_dinodes);

	CPIN_08(rg, str, rg_reserved, 36);
}

void gfs2_rgrp_out(struct gfs2_rgrp *rg, char *buf)
{
	struct gfs2_rgrp *str = (struct gfs2_rgrp *)buf;

	gfs2_meta_header_out(&rg->rg_header, buf);
	CPOUT_32(rg, str, rg_flags);
	CPOUT_32(rg, str, rg_free);
	CPOUT_32(rg, str, rg_dinodes);

	CPOUT_08(rg, str, rg_reserved, 36);
}

void gfs2_rgrp_print(struct gfs2_rgrp *rg)
{
	gfs2_meta_header_print(&rg->rg_header);
	pv(rg, rg_flags, "%u");
	pv(rg, rg_free, "%u");
	pv(rg, rg_dinodes, "%u");
}

void gfs2_quota_in(struct gfs2_quota *qu, char *buf)
{
	struct gfs2_quota *str = (struct gfs2_quota *)buf;

	CPIN_64(qu, str, qu_limit);
	CPIN_64(qu, str, qu_warn);
	CPIN_64(qu, str, qu_value);
}

void gfs2_quota_out(struct gfs2_quota *qu, char *buf)
{
	struct gfs2_quota *str = (struct gfs2_quota *)buf;

	CPOUT_64(qu, str, qu_limit);
	CPOUT_64(qu, str, qu_warn);
	CPOUT_64(qu, str, qu_value);
}

void gfs2_quota_print(struct gfs2_quota *qu)
{
	pv(qu, qu_limit, "%llu");
	pv(qu, qu_warn, "%llu");
	pv(qu, qu_value, "%lld");
}

void gfs2_dinode_in(struct gfs2_dinode *di, char *buf)
{
	struct gfs2_dinode *str = (struct gfs2_dinode *)buf;

	gfs2_meta_header_in(&di->di_header, buf);
	gfs2_inum_in(&di->di_num, (char *)&str->di_num);

	CPIN_32(di, str, di_mode);
	CPIN_32(di, str, di_uid);
	CPIN_32(di, str, di_gid);
	CPIN_32(di, str, di_nlink);
	CPIN_64(di, str, di_size);
	CPIN_64(di, str, di_blocks);
	CPIN_64(di, str, di_atime);
	CPIN_64(di, str, di_mtime);
	CPIN_64(di, str, di_ctime);
	CPIN_32(di, str, di_major);
	CPIN_32(di, str, di_minor);

	CPIN_64(di, str, di_goal_meta);
	CPIN_64(di, str, di_goal_data);

	CPIN_32(di, str, di_flags);
	CPIN_32(di, str, di_payload_format);
	CPIN_16(di, str, di_height);

	CPIN_16(di, str, di_depth);
	CPIN_32(di, str, di_entries);

	CPIN_64(di, str, di_eattr);

	CPIN_08(di, str, di_reserved, 32);
}

void gfs2_dinode_out(struct gfs2_dinode *di, char *buf)
{
	struct gfs2_dinode *str = (struct gfs2_dinode *)buf;

	gfs2_meta_header_out(&di->di_header, buf);
	gfs2_inum_out(&di->di_num, (char *)&str->di_num);

	CPOUT_32(di, str, di_mode);
	CPOUT_32(di, str, di_uid);
	CPOUT_32(di, str, di_gid);
	CPOUT_32(di, str, di_nlink);
	CPOUT_64(di, str, di_size);
	CPOUT_64(di, str, di_blocks);
	CPOUT_64(di, str, di_atime);
	CPOUT_64(di, str, di_mtime);
	CPOUT_64(di, str, di_ctime);
	CPOUT_32(di, str, di_major);
	CPOUT_32(di, str, di_minor);

	CPOUT_64(di, str, di_goal_meta);
	CPOUT_64(di, str, di_goal_data);

	CPOUT_32(di, str, di_flags);
	CPOUT_32(di, str, di_payload_format);
	CPOUT_16(di, str, di_height);

	CPOUT_16(di, str, di_depth);
	CPOUT_32(di, str, di_entries);

	CPOUT_64(di, str, di_eattr);

	CPOUT_08(di, str, di_reserved, 32);
}

void gfs2_dinode_print(struct gfs2_dinode *di)
{
	gfs2_meta_header_print(&di->di_header);
	gfs2_inum_print(&di->di_num);

	pv(di, di_mode, "0%o");
	pv(di, di_uid, "%u");
	pv(di, di_gid, "%u");
	pv(di, di_nlink, "%u");
	pv(di, di_size, "%llu");
	pv(di, di_blocks, "%llu");
	pv(di, di_atime, "%lld");
	pv(di, di_mtime, "%lld");
	pv(di, di_ctime, "%lld");
	pv(di, di_major, "%u");
	pv(di, di_minor, "%u");

	pv(di, di_goal_meta, "%llu");
	pv(di, di_goal_data, "%llu");

	pv(di, di_flags, "0x%.8X");
	pv(di, di_payload_format, "%u");
	pv(di, di_height, "%u");

	pv(di, di_depth, "%u");
	pv(di, di_entries, "%u");

	pv(di, di_eattr, "%llu");
}

void gfs2_dirent_in(struct gfs2_dirent *de, char *buf)
{
	struct gfs2_dirent *str = (struct gfs2_dirent *)buf;

	gfs2_inum_in(&de->de_inum, buf);
	CPIN_32(de, str, de_hash);
	CPIN_32(de, str, de_rec_len);
	de->de_name_len = str->de_name_len;
	de->de_type = str->de_type;
}

void gfs2_dirent_out(struct gfs2_dirent *de, char *buf)
{
	struct gfs2_dirent *str = (struct gfs2_dirent *)buf;

	gfs2_inum_out(&de->de_inum, buf);
	CPOUT_32(de, str, de_hash);
	CPOUT_32(de, str, de_rec_len);
	str->de_name_len = de->de_name_len;
	str->de_type = de->de_type;
	str->__pad1 = 0;
	str->__pad2 = 0;
}

void gfs2_dirent_print(struct gfs2_dirent *de, char *name)
{
	char buf[GFS2_FNAMESIZE + 1];

	gfs2_inum_print(&de->de_inum);
	pv(de, de_hash, "0x%.8X");
	pv(de, de_rec_len, "%u");
	pv(de, de_name_len, "%u");
	pv(de, de_type, "%u");

	memset(buf, 0, GFS2_FNAMESIZE + 1);
	memcpy(buf, name, de->de_name_len);
	printf("  name = %s\n", buf);
}

void gfs2_leaf_in(struct gfs2_leaf *lf, char *buf)
{
	struct gfs2_leaf *str = (struct gfs2_leaf *)buf;

	gfs2_meta_header_in(&lf->lf_header, buf);
	CPIN_16(lf, str, lf_depth);
	CPIN_16(lf, str, lf_entries);
	CPIN_32(lf, str, lf_dirent_format);
	CPIN_64(lf, str, lf_next);

	CPIN_08(lf, str, lf_reserved, 32);
}

void gfs2_leaf_out(struct gfs2_leaf *lf, char *buf)
{
	struct gfs2_leaf *str = (struct gfs2_leaf *)buf;

	gfs2_meta_header_out(&lf->lf_header, buf);
	CPOUT_16(lf, str, lf_depth);
	CPOUT_16(lf, str, lf_entries);
	CPOUT_32(lf, str, lf_dirent_format);
	CPOUT_64(lf, str, lf_next);

	CPOUT_08(lf, str, lf_reserved, 32);
}

void gfs2_leaf_print(struct gfs2_leaf *lf)
{
	gfs2_meta_header_print(&lf->lf_header);
	pv(lf, lf_depth, "%u");
	pv(lf, lf_entries, "%u");
	pv(lf, lf_dirent_format, "%u");
	pv(lf, lf_next, "%llu");
}

void gfs2_ea_header_in(struct gfs2_ea_header *ea, char *buf)
{
	struct gfs2_ea_header *str = (struct gfs2_ea_header *)buf;

	CPIN_32(ea, str, ea_rec_len);
	CPIN_32(ea, str, ea_data_len);
	ea->ea_name_len = str->ea_name_len;
	ea->ea_type = str->ea_type;
	ea->ea_flags = str->ea_flags;
	ea->ea_num_ptrs = str->ea_num_ptrs;
}

void gfs2_ea_header_out(struct gfs2_ea_header *ea, char *buf)
{
	struct gfs2_ea_header *str = (struct gfs2_ea_header *)buf;

	CPOUT_32(ea, str, ea_rec_len);
	CPOUT_32(ea, str, ea_data_len);
	str->ea_name_len = ea->ea_name_len;
	str->ea_type = ea->ea_type;
	str->ea_flags = ea->ea_flags;
	str->ea_num_ptrs = ea->ea_num_ptrs;
	str->__pad = 0;
}

void gfs2_ea_header_print(struct gfs2_ea_header *ea, char *name)
{
	char buf[GFS2_EA_MAX_NAME_LEN + 1];

	pv(ea, ea_rec_len, "%u");
	pv(ea, ea_data_len, "%u");
	pv(ea, ea_name_len, "%u");
	pv(ea, ea_type, "%u");
	pv(ea, ea_flags, "%u");
	pv(ea, ea_num_ptrs, "%u");

	memset(buf, 0, GFS2_EA_MAX_NAME_LEN + 1);
	memcpy(buf, name, ea->ea_name_len);
	printf("  name = %s\n", buf);
}

void gfs2_log_header_in(struct gfs2_log_header *lh, char *buf)
{
	struct gfs2_log_header *str = (struct gfs2_log_header *)buf;

	gfs2_meta_header_in(&lh->lh_header, buf);
	CPIN_64(lh, str, lh_sequence);
	CPIN_32(lh, str, lh_flags);
	CPIN_32(lh, str, lh_tail);
	CPIN_32(lh, str, lh_blkno);
	CPIN_32(lh, str, lh_hash);
}

void gfs2_log_header_out(struct gfs2_log_header *lh, char *buf)
{
	struct gfs2_log_header *str = (struct gfs2_log_header *)buf;

	gfs2_meta_header_out(&lh->lh_header, buf);
	CPOUT_64(lh, str, lh_sequence);
	CPOUT_32(lh, str, lh_flags);
	CPOUT_32(lh, str, lh_tail);
	CPOUT_32(lh, str, lh_blkno);
	CPOUT_32(lh, str, lh_hash);
}

void gfs2_log_header_print(struct gfs2_log_header *lh)
{
	gfs2_meta_header_print(&lh->lh_header);
	pv(lh, lh_sequence, "%llu");
	pv(lh, lh_flags, "0x%.8X");
	pv(lh, lh_tail, "%u");
	pv(lh, lh_blkno, "%u");
	pv(lh, lh_hash, "0x%.8X");
}

void gfs2_log_descriptor_in(struct gfs2_log_descriptor *ld, char *buf)
{
	struct gfs2_log_descriptor *str = (struct gfs2_log_descriptor *)buf;

	gfs2_meta_header_in(&ld->ld_header, buf);
	CPIN_32(ld, str, ld_type);
	CPIN_32(ld, str, ld_length);
	CPIN_32(ld, str, ld_data1);
	CPIN_32(ld, str, ld_data2);

	CPIN_08(ld, str, ld_reserved, 32);
}

void gfs2_log_descriptor_out(struct gfs2_log_descriptor *ld, char *buf)
{
	struct gfs2_log_descriptor *str = (struct gfs2_log_descriptor *)buf;

	gfs2_meta_header_out(&ld->ld_header, buf);
	CPOUT_32(ld, str, ld_type);
	CPOUT_32(ld, str, ld_length);
	CPOUT_32(ld, str, ld_data1);
	CPOUT_32(ld, str, ld_data2);

	CPOUT_08(ld, str, ld_reserved, 32);
}

void gfs2_log_descriptor_print(struct gfs2_log_descriptor *ld)
{
	gfs2_meta_header_print(&ld->ld_header);
	pv(ld, ld_type, "%u");
	pv(ld, ld_length, "%u");
	pv(ld, ld_data1, "%u");
	pv(ld, ld_data2, "%u");

	pa(ld, ld_reserved, 32);
}

void gfs2_inum_range_in(struct gfs2_inum_range *ir, char *buf)
{
	struct gfs2_inum_range *str = (struct gfs2_inum_range *)buf;

	CPIN_64(ir, str, ir_start);
	CPIN_64(ir, str, ir_length);
}

void gfs2_inum_range_out(struct gfs2_inum_range *ir, char *buf)
{
	struct gfs2_inum_range *str = (struct gfs2_inum_range *)buf;

	CPOUT_64(ir, str, ir_start);
	CPOUT_64(ir, str, ir_length);
}

void gfs2_inum_range_print(struct gfs2_inum_range *ir)
{
	pv(ir, ir_start, "%llu");
	pv(ir, ir_length, "%llu");
}

void gfs2_statfs_change_in(struct gfs2_statfs_change *sc, char *buf)
{
	struct gfs2_statfs_change *str = (struct gfs2_statfs_change *)buf;

	CPIN_64(sc, str, sc_total);
	CPIN_64(sc, str, sc_free);
	CPIN_64(sc, str, sc_dinodes);
}

void gfs2_statfs_change_out(struct gfs2_statfs_change *sc, char *buf)
{
	struct gfs2_statfs_change *str = (struct gfs2_statfs_change *)buf;

	CPOUT_64(sc, str, sc_total);
	CPOUT_64(sc, str, sc_free);
	CPOUT_64(sc, str, sc_dinodes);
}

void gfs2_statfs_change_print(struct gfs2_statfs_change *sc)
{
	pv(sc, sc_total, "%lld");
	pv(sc, sc_free, "%lld");
	pv(sc, sc_dinodes, "%lld");
}

void gfs2_unlinked_tag_in(struct gfs2_unlinked_tag *ut, char *buf)
{
	struct gfs2_unlinked_tag *str = (struct gfs2_unlinked_tag *)buf;

	gfs2_inum_in(&ut->ut_inum, buf);
	CPIN_32(ut, str, ut_flags);
}

void gfs2_unlinked_tag_out(struct gfs2_unlinked_tag *ut, char *buf)
{
	struct gfs2_unlinked_tag *str = (struct gfs2_unlinked_tag *)buf;

	gfs2_inum_out(&ut->ut_inum, buf);
	CPOUT_32(ut, str, ut_flags);
	str->__pad = 0;
}

void gfs2_unlinked_tag_print(struct gfs2_unlinked_tag *ut)
{
	gfs2_inum_print(&ut->ut_inum);
	pv(ut, ut_flags, "%u");
}

void gfs2_quota_change_in(struct gfs2_quota_change *qc, char *buf)
{
	struct gfs2_quota_change *str = (struct gfs2_quota_change *)buf;

	CPIN_64(qc, str, qc_change);
	CPIN_32(qc, str, qc_flags);
	CPIN_32(qc, str, qc_id);
}

void gfs2_quota_change_out(struct gfs2_quota_change *qc, char *buf)
{
	struct gfs2_quota_change *str = (struct gfs2_quota_change *)buf;

	CPOUT_64(qc, str, qc_change);
	CPOUT_32(qc, str, qc_flags);
	CPOUT_32(qc, str, qc_id);
}

void gfs2_quota_change_print(struct gfs2_quota_change *qc)
{
	pv(qc, qc_change, "%lld");
	pv(qc, qc_flags, "0x%.8X");
	pv(qc, qc_id, "%u");
}


