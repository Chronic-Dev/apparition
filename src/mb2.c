/*
 * mb2.c
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
	//not sure if we need all of these

#include <glib.h>
#include <glib/gstdio.h>
#include <gcrypt.h>
#include <unistd.h>

#include "afc.h"
#include "mb2.h"
#include "backup.h"
#include "lockdown.h"

#define MOBILEBACKUP2_SERVICE_NAME "com.apple.mobilebackup2"
#define NP_SERVICE_NAME "com.apple.mobile.notification_proxy"

/*
 
 this is a major work in progress, trying to migrate and tweak sections of idevicebackup2 so this code will probably be pretty sloppy until i fully understand wth im doing with it :)
 
  */


#define LOCK_ATTEMPTS 50
#define LOCK_WAIT 200000

#define CODE_SUCCESS 0x00
#define CODE_ERROR_LOCAL 0x06
#define CODE_ERROR_REMOTE 0x0b
#define CODE_FILE_DATA 0x0c

enum plist_format_t {
	PLIST_FORMAT_XML,
	PLIST_FORMAT_BINARY
};

enum cmd_flags {
	CMD_FLAG_RESTORE_SYSTEM_FILES       = 0,
	CMD_FLAG_RESTORE_REBOOT             = (1 << 1),
	CMD_FLAG_RESTORE_COPY_BACKUP        = (1 << 2),
	CMD_FLAG_RESTORE_SETTINGS           = (1 << 3)
};


	// the stuff above is all taken verbatim out of idevicebackup2.c from the top, not sure how much of it we will definitely need yet

	//in here for debug for now
static void print_progress_real(double progress, int flush)
{
	int i = 0;
	printf("\r[");
	for(i = 0; i < 50; i++) {
		if(i < progress / 2) {
			printf("=");
		} else {
			printf(" ");
		}
	}
	printf("] %3.0f%%", progress);
	
	if (flush > 0) {
		fflush(stdout);
		if (progress == 100)
			printf("\n");
	}
}

static void print_progress(uint64_t current, uint64_t total)
{
	gchar *format_size = NULL;
	double progress = ((double)current/(double)total)*100;
	if (progress < 0)
		return;
	
	if (progress > 100)
		progress = 100;
	
	print_progress_real((double)progress, 0);
	
	format_size = g_format_size_for_display(current);
	printf(" (%s", format_size);
	g_free(format_size);
	format_size = g_format_size_for_display(total);
	printf("/%s)     ", format_size);
	g_free(format_size);
	
	fflush(stdout);
	if (progress == 100)
		printf("\n");
}


static plist_t mobilebackup_factory_info_plist_new(mb2_t* mb2s)
{
	/* gather data from lockdown */
	GTimeVal tv = {0, 0};
	plist_t value_node = NULL;
	plist_t root_node = NULL;
	char *uuid = NULL;
	char *uuid_uppercase = NULL;
	
	lockdown_t *lockdown = mb2s->lockdown;
	device_t *device = lockdown->device;
	afc_t *afcs = mb2s->afclient;
	
	plist_t ret = plist_new_dict();
	
	/* get basic device information in one go */
	lockdownd_get_value(lockdown->client, NULL, NULL, &root_node);
	
	/* set fields we understand */
	value_node = plist_dict_get_item(root_node, "BuildVersion");
	plist_dict_insert_item(ret, "Build Version", plist_copy(value_node));
	
	value_node = plist_dict_get_item(root_node, "DeviceName");
	plist_dict_insert_item(ret, "Device Name", plist_copy(value_node));
	plist_dict_insert_item(ret, "Display Name", plist_copy(value_node));
	
	/* FIXME: How is the GUID generated? */
	plist_dict_insert_item(ret, "GUID", plist_new_string("---"));
	
	value_node = plist_dict_get_item(root_node, "IntegratedCircuitCardIdentity");
	if (value_node)
		plist_dict_insert_item(ret, "ICCID", plist_copy(value_node));
	
	value_node = plist_dict_get_item(root_node, "InternationalMobileEquipmentIdentity");
	if (value_node)
		plist_dict_insert_item(ret, "IMEI", plist_copy(value_node));
	
	g_get_current_time(&tv);
	plist_dict_insert_item(ret, "Last Backup Date", plist_new_date(tv.tv_sec, 0));
	
	value_node = plist_dict_get_item(root_node, "PhoneNumber");
	if (value_node && (plist_get_node_type(value_node) == PLIST_STRING)) {
		plist_dict_insert_item(ret, "Phone Number", plist_copy(value_node));
	}
	
	value_node = plist_dict_get_item(root_node, "ProductType");
	plist_dict_insert_item(ret, "Product Type", plist_copy(value_node));
	
	value_node = plist_dict_get_item(root_node, "ProductVersion");
	plist_dict_insert_item(ret, "Product Version", plist_copy(value_node));
	
	value_node = plist_dict_get_item(root_node, "SerialNumber");
	plist_dict_insert_item(ret, "Serial Number", plist_copy(value_node));
	
	/* FIXME Sync Settings? */
	
	value_node = plist_dict_get_item(root_node, "UniqueDeviceID");
	idevice_get_uuid(device->client, &uuid);
	plist_dict_insert_item(ret, "Target Identifier", plist_new_string(uuid));
	
	plist_dict_insert_item(ret, "Target Type", plist_new_string("Device"));
	
	/* uppercase */
	uuid_uppercase = g_ascii_strup(uuid, -1);
	plist_dict_insert_item(ret, "Unique Identifier", plist_new_string(uuid_uppercase));
	free(uuid_uppercase);
	free(uuid);
	
	char *data_buf = NULL;
	uint64_t data_size = 0;
		//mobilebackup_afc_get_file_contents("/Books/iBooksData2.plist", &data_buf, &data_size);
	apparition_afc_get_file_contents(mb2s->afclient, "/Books/iBooksData2.plist", &data_buf, &data_size);
	if (data_buf) {
		plist_dict_insert_item(ret, "iBooks Data 2", plist_new_data(data_buf, data_size));
		free(data_buf);
	}
	
	plist_t files = plist_new_dict();
	const char *itunesfiles[] = {
		"ApertureAlbumPrefs",
		"IC-Info.sidb",
		"IC-Info.sidv",
		"PhotosFolderAlbums",
		"PhotosFolderName",
		"PhotosFolderPrefs",
		"iPhotoAlbumPrefs",
		"iTunesApplicationIDs",
		"iTunesPrefs",
		"iTunesPrefs.plist",
		NULL
	};
	int i = 0;
	for (i = 0; itunesfiles[i]; i++) {
		data_buf = NULL;
		data_size = 0;
		gchar *fname = g_strconcat("/iTunes_Control/iTunes/", itunesfiles[i], NULL);
			//mobilebackup_afc_get_file_contents(fname, &data_buf, &data_size);
		apparition_afc_get_file_contents(mb2s->afclient, "/iTunes_Control/iTunes/", itunesfiles[i], NULL);
		g_free(fname);
		if (data_buf) {
			plist_dict_insert_item(files, itunesfiles[i], plist_new_data(data_buf, data_size));
			free(data_buf);
		}
	}
	plist_dict_insert_item(ret, "iTunes Files", files);
	
	plist_t itunes_settings = plist_new_dict();
	lockdownd_get_value(lockdown->client, "com.apple.iTunes", NULL, &itunes_settings);
	plist_dict_insert_item(ret, "iTunes Settings", itunes_settings);
	
	plist_dict_insert_item(ret, "iTunes Version", plist_new_string("10.0.1"));
	
	plist_free(root_node);
	
	return ret;
}

static void buffer_read_from_filename(const char *filename, char **buffer, uint64_t *length)
{
	FILE *f;
	uint64_t size;
	
	*length = 0;
	
	f = fopen(filename, "rb");
	if (!f) {
		return;
	}
	
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	rewind(f);
	
	if (size == 0) {
		return;
	}
	
	*buffer = (char*)malloc(sizeof(char)*size);
	fread(*buffer, sizeof(char), size, f);
	fclose(f);
	
	*length = size;
}

static void buffer_write_to_filename(const char *filename, const char *buffer, uint64_t length)
{
	FILE *f;
	
	f = fopen(filename, "ab");
	if (!f)
		f = fopen(filename, "wb");
	if (f) {
		fwrite(buffer, sizeof(char), length, f);
		fclose(f);
	}
}

static int plist_read_from_filename(plist_t *plist, const char *filename)
{
	char *buffer = NULL;
	uint64_t length;
	
	if (!filename)
		return 0;
	
	buffer_read_from_filename(filename, &buffer, &length);
	
	if (!buffer) {
		return 0;
	}
	
	if ((length > 8) && (memcmp(buffer, "bplist00", 8) == 0)) {
		plist_from_bin(buffer, length, plist);
	} else {
		plist_from_xml(buffer, length, plist);
	}
	
	free(buffer);
	
	return 1;
}

static int plist_write_to_filename(plist_t plist, const char *filename, enum plist_format_t format)
{
	char *buffer = NULL;
	uint32_t length;
	
	if (!plist || !filename)
		return 0;
	
	if (format == PLIST_FORMAT_XML)
		plist_to_xml(plist, &buffer, &length);
	else if (format == PLIST_FORMAT_BINARY)
		plist_to_bin(plist, &buffer, &length);
	else
		return 0;
	
	buffer_write_to_filename(filename, buffer, length);
	
	free(buffer);
	
	return 1;
}

//we may never need to use this

static int mb2_status_check_snapshot_state(const char *path, const char *uuid, const char *matches) 
{
	int ret = -1;
	plist_t status_plist = NULL;
	gchar *file_path = g_build_path(G_DIR_SEPARATOR_S, path, uuid, "Status.plist", NULL);
	
	plist_read_from_filename(&status_plist, file_path);
	g_free(file_path);
	if (!status_plist) {
		printf("Could not read Status.plist!\n");
		return ret;
	}
	plist_t node = plist_dict_get_item(status_plist, "SnapshotState");
	if (node && (plist_get_node_type(node) == PLIST_STRING)) {
		char* sval = NULL;
		plist_get_string_val(node, &sval);
		if (sval) {
			ret = (strcmp(sval, matches) == 0) ? 1 : 0;
		}
	} else {
		printf("%s: ERROR could not get SnapshotState key from Status.plist!\n", __func__);
	}
	plist_free(status_plist);
	return ret;
}

static int mobilebackup_info_is_current_device(mb2_t* mb2s, plist_t info)
{
	plist_t value_node = NULL;
	plist_t node = NULL;
	plist_t root_node = NULL;
	int ret = 0;
	
	lockdown_t *lockdown = mb2s->lockdown;
	
	if (!info)
		return ret;
	
	if (plist_get_node_type(info) != PLIST_DICT)
		return ret;
	
	/* get basic device information in one go */
	lockdownd_get_value(lockdown->client, NULL, NULL, &root_node);
	
	/* verify UUID */
	value_node = plist_dict_get_item(root_node, "UniqueDeviceID");
	node = plist_dict_get_item(info, "Target Identifier");
	
	if(plist_compare_node_value(value_node, node))
		ret = 1;
	else {
		printf("Info.plist: UniqueDeviceID does not match.\n");
	}
	
	/* verify SerialNumber */
	if (ret == 1) {
		value_node = plist_dict_get_item(root_node, "SerialNumber");
		node = plist_dict_get_item(info, "Serial Number");
		
		if(plist_compare_node_value(value_node, node))
			ret = 1;
		else {
			printf("Info.plist: SerialNumber does not match.\n");
			ret = 0;
		}
	}
	
	/* verify ProductVersion to prevent using backup with different OS version */
	if (ret == 1) {
		value_node = plist_dict_get_item(root_node, "ProductVersion");
		node = plist_dict_get_item(info, "Product Version");
		
		if(plist_compare_node_value(value_node, node))
			ret = 1;
		else {
			printf("Info.plist: ProductVersion does not match.\n");
			ret = 0;
		}
	}
	
	plist_free(root_node);
	root_node = NULL;
	
	value_node = NULL;
	node = NULL;
	
	return ret;
}

static void apparition_do_post_notification(mb2_t* mb2s, const char *notification)
{
	uint16_t nport = 0;
	np_client_t np;
	
	lockdown_t *lockdown = mb2s->lockdown;
	device_t *device = lockdown->device;
	
	if (!(lockdown->client)) {
		if (lockdownd_client_new_with_handshake(device->client, &(lockdown->client), "apparition") != LOCKDOWN_E_SUCCESS) {
			return;
		}
	}
	
	lockdownd_start_service(lockdown->client, NP_SERVICE_NAME, &nport);
	if (nport) {
		np_client_new(device->client, nport, &np);
		if (np) {
			np_post_notification(np, notification);
			np_client_free(np);
		}
	} else {
		printf("Could not start %s\n", NP_SERVICE_NAME);
	}
}

/* not sure if we even need this */


static void mb2_multi_status_add_file_error(plist_t status_dict, const char *path, int error_code, const char *error_message)
{
	if (!status_dict) return;
	plist_t filedict = plist_new_dict();
	plist_dict_insert_item(filedict, "DLFileErrorString", plist_new_string(error_message));
	plist_dict_insert_item(filedict, "DLFileErrorCode", plist_new_uint(error_code));
	plist_dict_insert_item(status_dict, path, filedict);
}

static int errno_to_device_error(int errno_value)
{
	switch (errno_value) {
		case ENOENT:
			return -6;
		case EEXIST:
			return -7;
		default:
			return -errno_value;
	}
}

static int mb2_handle_send_file(mb2_t* mb2s, const char *backup_dir, const char *path, plist_t *errplist)
{
	uint32_t nlen = 0;
	uint32_t pathlen = strlen(path);
	uint32_t bytes = 0;
	gchar *localfile = g_build_path(G_DIR_SEPARATOR_S, backup_dir, path, NULL);
	char buf[32768];
	struct stat fst;
	
	FILE *f = NULL;
	uint32_t slen = 0;
	int errcode = -1;
	int result = -1;
	uint32_t length;
	off_t total;
	off_t sent;
	
	mobilebackup2_error_t err;
	
	/* send path length */
	nlen = GUINT32_TO_BE(pathlen);
	err = mobilebackup2_send_raw(mb2s->client, (const char*)&nlen, sizeof(nlen), &bytes);
	if (err != MOBILEBACKUP2_E_SUCCESS) {
		goto leave_proto_err;
	}
	if (bytes != (uint32_t)sizeof(nlen)) {
		err = MOBILEBACKUP2_E_MUX_ERROR;
		goto leave_proto_err;
	}
	
	/* send path */
	err = mobilebackup2_send_raw(mb2s->client, path, pathlen, &bytes);
	if (err != MOBILEBACKUP2_E_SUCCESS) {
		goto leave_proto_err;
	}
	if (bytes != pathlen) {
		err = MOBILEBACKUP2_E_MUX_ERROR;
		goto leave_proto_err;
	}
	
	if (stat(localfile, &fst) < 0) {
		if (errno != ENOENT)
			printf("%s: stat failed on '%s': %d\n", __func__, localfile, errno);
		errcode = errno;
		goto leave;
	}
	
	total = fst.st_size;
	
	gchar *format_size = g_format_size_for_display(total);
	printf("Sending '%s' (%s)\n", path, format_size);
	g_free(format_size);
	
	if (total == 0) {
		errcode = 0;
		goto leave;
	}
	
	f = fopen(localfile, "rb");
	if (!f) {
		printf("%s: Error opening local file '%s': %d\n", __func__, localfile, errno);
		errcode = errno;
		goto leave;
	}
	
	sent = 0;
	do {
		length = ((total-sent) < (off_t)sizeof(buf)) ? (uint32_t)total-sent : (uint32_t)sizeof(buf);
		/* send data size (file size + 1) */
		nlen = GUINT32_TO_BE(length+1);
		memcpy(buf, &nlen, sizeof(nlen));
		buf[4] = CODE_FILE_DATA;
		err = mobilebackup2_send_raw(mb2s->client, (const char*)buf, 5, &bytes);
		if (err != MOBILEBACKUP2_E_SUCCESS) {
			goto leave_proto_err;
		}
		if (bytes != 5) {
			goto leave_proto_err;
		}
		
		/* send file contents */
		size_t r = fread(buf, 1, sizeof(buf), f);
		if (r <= 0) {
			printf("%s: read error\n", __func__);
			errcode = errno;
			goto leave;
		}
		err = mobilebackup2_send_raw(mb2s->client, buf, r, &bytes);
		if (err != MOBILEBACKUP2_E_SUCCESS) {
			goto leave_proto_err;
		}
		if (bytes != (uint32_t)r) {
			printf("Error: sent only %d of %d bytes\n", bytes, (int)r);
			goto leave_proto_err;
		}
		sent += r;
	} while (sent < total);
	fclose(f);
	f = NULL;
	errcode = 0;
	
leave:
	if (errcode == 0) {
		result = 0;
		nlen = 1;
		nlen = GUINT32_TO_BE(nlen);
		memcpy(buf, &nlen, 4);
		buf[4] = CODE_SUCCESS;
		mobilebackup2_send_raw(mb2s->client, buf, 5, &bytes);
	} else {
		if (!*errplist) {
			*errplist = plist_new_dict();
		}
		char *errdesc = strerror(errcode);
		mb2_multi_status_add_file_error(*errplist, path, errno_to_device_error(errcode), errdesc);
		
		length = strlen(errdesc);
		nlen = GUINT32_TO_BE(length+1);
		memcpy(buf, &nlen, 4);
		buf[4] = CODE_ERROR_LOCAL;
		slen = 5;
		memcpy(buf+slen, errdesc, length);
		slen += length;
		err = mobilebackup2_send_raw(mb2s->client, (const char*)buf, slen, &bytes);
		if (err != MOBILEBACKUP2_E_SUCCESS) {
			printf("could not send message\n");
		}
		if (bytes != slen) {
			printf("could only send %d from %d\n", bytes, slen);
		}
	}
	
leave_proto_err:
	if (f)
		fclose(f);
	g_free(localfile);
	return result;
}

static void mb2_handle_send_files(mb2_t* mb2s, plist_t message, const char *backup_dir)
{
	uint32_t cnt; 
	uint32_t i = 0;
	uint32_t sent;
	plist_t errplist = NULL;
	
	if (!message || (plist_get_node_type(message) != PLIST_ARRAY) || (plist_array_get_size(message) < 2) || !backup_dir) return;
	
	plist_t files = plist_array_get_item(message, 1);
	cnt = plist_array_get_size(files);
	if (cnt == 0) return;
	
	for (i = 0; i < cnt; i++) {
		plist_t val = plist_array_get_item(files, i);
		if (plist_get_node_type(val) != PLIST_STRING) {
			continue;
		}
		char *str = NULL;
		plist_get_string_val(val, &str);
		if (!str)
			continue;
		
		if (mb2_handle_send_file(mb2s, backup_dir, str, &errplist) < 0) {
				//printf("Error when sending file '%s' to device\n", str);
				// TODO: perhaps we can continue, we've got a multi status response?!
			break;
		}
	}
	
	/* send terminating 0 dword */
	uint32_t zero = 0;
	mobilebackup2_send_raw(mb2s->client, (char*)&zero, 4, &sent);
	
	if (!errplist) {
		mobilebackup2_send_status_response(mb2s->client, 0, NULL, plist_new_dict());
	} else {
		mobilebackup2_send_status_response(mb2s->client, -13, "Multi status", errplist);
		plist_free(errplist);
	}
}

static int mb2_handle_receive_files(mb2_t* mb2s, plist_t message, const char *backup_dir)
{
	uint64_t backup_real_size = 0;
	uint64_t backup_total_size = 0;
	uint32_t blocksize;
	uint32_t bdone;
	uint32_t rlen;
	uint32_t nlen = 0;
	uint32_t r;
	char buf[32768];
	char *fname = NULL;
	char *dname = NULL;
	gchar *bname = NULL;
	char code = 0;
	char last_code = 0;
	plist_t node = NULL;
	FILE *f = NULL;
	unsigned int file_count = 0;
	
	if (!message || (plist_get_node_type(message) != PLIST_ARRAY) || plist_array_get_size(message) < 4 || !backup_dir) return 0;
	
	node = plist_array_get_item(message, 3);
	if (plist_get_node_type(node) == PLIST_UINT) {
		plist_get_uint_val(node, &backup_total_size);
	}
	if (backup_total_size > 0) {
		printf("Receiving files\n");
	}
	
	do {
	
		r = 0;
		mobilebackup2_receive_raw(mb2s->client, (char*)&nlen, 4, &r);
		nlen = GUINT32_FROM_BE(nlen);
		if (nlen == 0) {
				// we're done here
			break;
		} else if (nlen > 4096) {
				// too very long path
			printf("ERROR: %s: too long device filename (%d)!\n", __func__, nlen);
			break;
		}
		if (dname != NULL)
			free(dname);
		dname = (char*)malloc(nlen+1);
		r = 0;
		mobilebackup2_receive_raw(mb2s->client, dname, nlen, &r);
		if (r != nlen) {
			printf("ERROR: %s: could not read device filename\n", __func__);
			break;
		}
		dname[r] = 0;
		nlen = 0;
		mobilebackup2_receive_raw(mb2s->client, (char*)&nlen, 4, &r);
		nlen = GUINT32_FROM_BE(nlen);
		if (nlen == 0) {
			printf("ERROR: %s: zero-length backup filename!\n", __func__);
			break;
		} else if (nlen > 4096) {
			printf("ERROR: %s: too long backup filename (%d)!\n", __func__, nlen);
			break;
		}
		fname = (char*)malloc(nlen+1);
		mobilebackup2_receive_raw(mb2s->client, fname, nlen, &r);
		if (r != nlen) {
			printf("ERROR: %s: could not receive backup filename!\n", __func__);
			break;
		}
		fname[r] = 0;
		if (bname != NULL)
			g_free(bname);
		bname = g_build_path(G_DIR_SEPARATOR_S, backup_dir, fname, NULL);
		free(fname);
		nlen = 0;
		mobilebackup2_receive_raw(mb2s->client, (char*)&nlen, 4, &r);
		if (r != 4) {
			printf("ERROR: %s: could not receive code length!\n", __func__);
			break;
		}
		nlen = GUINT32_FROM_BE(nlen);
		last_code = code;
		code = 0;
		mobilebackup2_receive_raw(mb2s->client, &code, 1, &r);
		if (r != 1) {
			printf("ERROR: %s: could not receive code!\n", __func__);
			break;
		}
		
		/* TODO remove this */
		if ((code != CODE_SUCCESS) && (code != CODE_FILE_DATA) && (code != CODE_ERROR_REMOTE)) {
			printf("Found new flag %02x\n", code);
		}
		
		remove(bname);
		f = fopen(bname, "wb");
		while (f && (code == CODE_FILE_DATA)) {
			blocksize = nlen-1;
			bdone = 0;
			rlen = 0;
			while (bdone < blocksize) {
				if ((blocksize - bdone) < sizeof(buf)) {
					rlen = blocksize - bdone;
				} else {
					rlen = sizeof(buf);
				}
				mobilebackup2_receive_raw(mb2s->client, buf, rlen, &r);
				if ((int)r <= 0) {
					break;
				}
				fwrite(buf, 1, r, f);
				bdone += r;
			}
			if (bdone == blocksize) {
				backup_real_size += blocksize;
			}
			if (backup_total_size > 0) {
					//print_progress(backup_real_size, backup_total_size);
			}
			
			nlen = 0;
			mobilebackup2_receive_raw(mb2s->client, (char*)&nlen, 4, &r);
			nlen = GUINT32_FROM_BE(nlen);
			if (nlen > 0) {
				last_code = code;
				mobilebackup2_receive_raw(mb2s->client, &code, 1, &r);
			} else {
				break;
			}
		}
		if (f) {
			fclose(f);
			file_count++;
		} else {
			printf("Error opening '%s' for writing: %s\n", bname, strerror(errno));
		}
		if (nlen == 0) {
			break;
		}
		
		/* check if an error message was received */
		if (code == CODE_ERROR_REMOTE) {
			/* error message */
			char *msg = (char*)malloc(nlen);
			mobilebackup2_receive_raw(mb2s->client, msg, nlen-1, &r);
			msg[r] = 0;
			/* If sent using CODE_FILE_DATA, end marker will be CODE_ERROR_REMOTE which is not an error! */
			if (last_code != CODE_FILE_DATA) {
				fprintf(stdout, "\nReceived an error message from device: %s\n", msg);
			}
			free(msg);
		}
	} while (1);
	
	/* if there are leftovers to read, finish up cleanly */
	if ((int)nlen-1 > 0) {
		printf("\nDiscarding current data hunk.\n");
		fname = (char*)malloc(nlen-1);
		mobilebackup2_receive_raw(mb2s->client, fname, nlen-1, &r);
		free(fname);
		remove(bname);
	}
	
	/* clean up */
	if (bname != NULL)
		g_free(bname);
	
	if (dname != NULL)
		free(dname);
	
		// TODO error handling?!
	mobilebackup2_send_status_response(mb2s->client, 0, NULL, plist_new_dict());
	return file_count;
}

static void mb2_handle_list_directory(mb2_t* mb2s, plist_t message, const char *backup_dir)
{
	if (!message || (plist_get_node_type(message) != PLIST_ARRAY) || plist_array_get_size(message) < 2 || !backup_dir) return;
	
	plist_t node = plist_array_get_item(message, 1);
	char *str = NULL;
	if (plist_get_node_type(node) == PLIST_STRING) {
		plist_get_string_val(node, &str);
	}
	if (!str) {
		printf("ERROR: Malformed DLContentsOfDirectory message\n");
			// TODO error handling
		return;
	}
	
	gchar *path = g_build_path(G_DIR_SEPARATOR_S, backup_dir, str, NULL);
	free(str);
	
	plist_t dirlist = plist_new_dict();
	
	GDir *cur_dir = g_dir_open(path, 0, NULL);
	if (cur_dir) {
		gchar *dir_file;
		while ((dir_file = (gchar *)g_dir_read_name(cur_dir))) {
			gchar *fpath = g_build_filename(path, dir_file, NULL);
			if (fpath) {
				plist_t fdict = plist_new_dict();
				GStatBuf st;
				g_stat(fpath, &st);
				const char *ftype = "DLFileTypeUnknown";
				if (g_file_test(fpath, G_FILE_TEST_IS_DIR)) {
					ftype = "DLFileTypeDirectory";
				} else if (g_file_test(fpath, G_FILE_TEST_IS_REGULAR)) {
					ftype = "DLFileTypeRegular";
				}
				plist_dict_insert_item(fdict, "DLFileType", plist_new_string(ftype));
				plist_dict_insert_item(fdict, "DLFileSize", plist_new_uint(st.st_size));
				plist_dict_insert_item(fdict, "DLFileModificationDate", plist_new_date(st.st_mtime, 0));
				
				plist_dict_insert_item(dirlist, dir_file, fdict);
				g_free(fpath);
			}
		}
		g_dir_close(cur_dir);
	}
	g_free(path);
	
	/* TODO error handling */
	mobilebackup2_error_t err = mobilebackup2_send_status_response(mb2s->client, 0, NULL, dirlist);
	plist_free(dirlist);
	if (err != MOBILEBACKUP2_E_SUCCESS) {
		printf("Could not send status response, error %d\n", err);
	}
}

static void mb2_handle_make_directory(mb2_t* mb2s, plist_t message, const char *backup_dir)
{
	if (!message || (plist_get_node_type(message) != PLIST_ARRAY) || plist_array_get_size(message) < 2 || !backup_dir) return;
	
	plist_t dir = plist_array_get_item(message, 1);
	char *str = NULL;
	int errcode = 0;
	char *errdesc = NULL;
	plist_get_string_val(dir, &str);
	
	gchar *newpath = g_build_path(G_DIR_SEPARATOR_S, backup_dir, str, NULL);
	g_free(str);
	
	if (g_mkdir_with_parents(newpath, 0755) < 0) {
		errdesc = strerror(errno);
		if (errno != EEXIST) {
			printf("mkdir: %s (%d)\n", errdesc, errno);
		}
		errcode = errno_to_device_error(errno);
	}
	g_free(newpath);
	mobilebackup2_error_t err = mobilebackup2_send_status_response(mb2s->client, errcode, errdesc, NULL);
	if (err != MOBILEBACKUP2_E_SUCCESS) {
		printf("Could not send status response, error %d\n", err);
	}
}

static void mb2_copy_file_by_path(const gchar *src, const gchar *dst)
{
	FILE *from, *to;
	char buf[BUFSIZ];
	size_t length;
	
	/* open source file */
	if ((from = fopen(src, "rb")) == NULL) {
		printf("Cannot open source path '%s'.\n", src);
		return;
	}
	
	/* open destination file */
	if ((to = fopen(dst, "wb")) == NULL) {
		printf("Cannot open destination file '%s'.\n", dst);
		return;
	}
	
	/* copy the file */
	while ((length = fread(buf, 1, BUFSIZ, from)) != 0) {
		fwrite(buf, 1, length, to);
	}
	
	if(fclose(from) == EOF) {
		printf("Error closing source file.\n");
	}
	
	if(fclose(to) == EOF) {
		printf("Error closing destination file.\n");
	}
}

static void mb2_copy_directory_by_path(const gchar *src, const gchar *dst)
{
	if (!src || !dst) {
		return;
	}
	
	/* if src does not exist */
	if (!g_file_test(src, G_FILE_TEST_EXISTS)) {
		printf("ERROR: Source directory does not exist '%s': %s (%d)\n", src, strerror(errno), errno);
		return;
	}
	
	/* if dst directory does not exist */
	if (!g_file_test(dst, G_FILE_TEST_IS_DIR)) {
		/* create it */
		if (g_mkdir_with_parents(dst, 0755) < 0) {
			printf("ERROR: Unable to create destination directory '%s': %s (%d)\n", dst, strerror(errno), errno);
			return;
		}
	}
	
	/* loop over src directory contents */
	GDir *cur_dir = g_dir_open(src, 0, NULL);
	if (cur_dir) {
		gchar *dir_file;
		while ((dir_file = (gchar *)g_dir_read_name(cur_dir))) {
			gchar *srcpath = g_build_filename(src, dir_file, NULL);
			gchar *dstpath = g_build_filename(dst, dir_file, NULL);
			if (srcpath && dstpath) {
				/* copy file */
				mb2_copy_file_by_path(srcpath, dstpath);
				
				g_free(srcpath);
				g_free(dstpath);
			}
		}
		g_dir_close(cur_dir);
	}
}

/**
 * signal handler function for cleaning up properly
 */
static void clean_exit(int sig)
{
	fprintf(stderr, "Exiting...\n");
	
}


	//^^ this code is also taken verbatim from idevicebackup2.c, will definitely require tweaking



mb2_t* mb2_open(lockdown_t* lockdown, afc_t* afcs)
{
	int err = 0;
	mb2_t* mb2 = NULL;
	
	mb2 = (mb2_t*) malloc(sizeof(mb2_t));
	if(mb2 == NULL) {
		return NULL;
	}
	memset(mb2, '\0', sizeof(mb2_t));
	
	mb2->lockdown = lockdown;
	mb2->afclient = afcs;

	return mb2;
}

int mb2_restore(mb2_t* mb2, backup_t* backup) {
	//TODO: Implement Me
	return -1;
}

int mb2_close(mb2_t* mb2) {
	//TODO: Implement Me
	return -1;
}

void mb2_free(mb2_t* mb2) {
	if(mb2) {
		free(mb2);
	}
}
