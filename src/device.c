/*
 * device.c
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

/* This is the application Controller.
 *  This file should contain functions to control the device
 *  through lockdownd services. This file acts as a proxy to
 *  the afc, mb2, and nos services and should handle messages.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "device.h"
#include "nos.h"
#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>

static mobilebackup2_client_t mobilebackup2 = NULL;
static lockdownd_client_t client = NULL;
static idevice_t phone = NULL;
static nos_t nos = NULL;

device_t* device_create(const char* uuid) {
	int err = 0;
	device_t* device = NULL;
	int i;
	
	device = (device_t*) malloc(sizeof(device_t));
	if(device == NULL) {
		return NULL;
	}
	
	if (uuid == NULL)
	{
	
		char **dev_list = NULL;
		if (idevice_get_device_list(&dev_list, &i) < 0) {
			fprintf(stderr, "ERROR: Unable to retrieve device list!\n");
			return NULL;
		}
		
		uuid = dev_list[0];
	}
	
	err = idevice_new(&phone, uuid);
	if (ret != IDEVICE_E_SUCCESS) {
		printf("No device found with uuid %s, is it plugged in?\n", uuid);
		return NULL;
	}
	
	if (LOCKDOWN_E_SUCCESS != lockdownd_client_new_with_handshake(phone, &client, "apparition")) {
		idevice_free(phone);
		return NULL;
	}
	
	nos = nos_open(client);
	
	if (nos == NULL)
	{
		printf("Failed to open NOS notification center!!");
		idevice_free(phone);
		return NULL;
	}
	int nosStatus = 0;
	nosStatus = nos_register(nos, notify_cb, phone);
	
	if (nosStatus != 0)
	{
		printf("Failed to register NOS notification callback!!");
		idevice_free(phone);
		return NULL;
	}
	
	
	
	memset(device, '\0', sizeof(device_t));
	
	return device;
}

void device_free(device_t* device) {
	if(device) {
		free(device);
	}
}


static void notify_cb(const char *notification, void *userdata)
{
	if (!strcmp(notification, NP_SYNC_CANCEL_REQUEST)) {
		printf("User has cancelled the backup process on the device.\n");
		
	} else {
		printf("Unhandled notification '%s' (TODO: implement)\n", notification);
	}
}
 
 
 


/*

 FROM idevicebackup2.c, just here to provide a framework of what needs to be done

 int main(int argc, char *argv[])
 {
 idevice_error_t ret = IDEVICE_E_UNKNOWN_ERROR;
 int i;
 char uuid[41];
 uint16_t port = 0;
 uuid[0] = 0;
 int cmd = -1;
 int cmd_flags = 0;
 int is_full_backup = 0;
 char *backup_directory = NULL;
 struct stat st;
 plist_t node_tmp = NULL;
 plist_t info_plist = NULL;
 plist_t opts = NULL;
 mobilebackup2_error_t err;
 
  we need to exit cleanly on running backups and restores or we cause havok 
signal(SIGINT, clean_exit);
signal(SIGQUIT, clean_exit);
signal(SIGTERM, clean_exit);
signal(SIGPIPE, SIG_IGN);



afc = NULL;
if (cmd == CMD_BACKUP) {
	 start AFC, we need this for the lock file 
	port = 0;
	ret = lockdownd_start_service(client, "com.apple.afc", &port);
	if ((ret == LOCKDOWN_E_SUCCESS) && port) {
		afc_client_new(phone, port, &afc);
	}
}

 start mobilebackup service and retrieve port 
port = 0;
ret = lockdownd_start_service(client, MOBILEBACKUP2_SERVICE_NAME, &port);
if ((ret == LOCKDOWN_E_SUCCESS) && port) {
	PRINT_VERBOSE(1, "Started \"%s\" service on port %d.\n", MOBILEBACKUP2_SERVICE_NAME, port);
	mobilebackup2_client_new(phone, port, &mobilebackup2);
	
	 send Hello message 
	double local_versions[2] = {2.0, 2.1};
	double remote_version = 0.0;
	err = mobilebackup2_version_exchange(mobilebackup2, local_versions, 2, &remote_version);
	if (err != MOBILEBACKUP2_E_SUCCESS) {
		printf("Could not perform backup protocol version exchange, error code %d\n", err);
		cmd = CMD_LEAVE;
		goto checkpoint;
	}
	
	PRINT_VERBOSE(1, "Negotiated Protocol Version %.1f\n", remote_version);
	
	 check abort conditions 
	if (quit_flag > 0) {
		PRINT_VERBOSE(1, "Aborting as requested by user...\n");
		cmd = CMD_LEAVE;
		goto checkpoint;
	}
	
	 verify existing Info.plist 
	if (stat(info_path, &st) == 0) {
		PRINT_VERBOSE(1, "Reading Info.plist from backup.\n");
		plist_read_from_filename(&info_plist, info_path);
		
		if (!info_plist) {
			printf("Could not read Info.plist\n");
			is_full_backup = 1;
		}
		if (info_plist && ((cmd == CMD_BACKUP) || (cmd == CMD_RESTORE))) {
			if (!mobilebackup_info_is_current_device(info_plist)) {
				printf("Aborting. Backup data is not compatible with the current device.\n");
				cmd = CMD_LEAVE;
			}
		}
	} else {
		if (cmd == CMD_RESTORE) {
			printf("Aborting restore. Info.plist is missing.\n");
			cmd = CMD_LEAVE;
		} else {
			is_full_backup = 1;
		}
	}
	
	uint64_t lockfile = 0;
	if (cmd == CMD_BACKUP) {
		do_post_notification(NP_SYNC_WILL_START);
		afc_file_open(afc, "/com.apple.itunes.lock_sync", AFC_FOPEN_RW, &lockfile);
	}
	if (lockfile) {
		afc_error_t aerr;
		do_post_notification(NP_SYNC_LOCK_REQUEST);
		for (i = 0; i < LOCK_ATTEMPTS; i++) {
			aerr = afc_file_lock(afc, lockfile, AFC_LOCK_EX);
			if (aerr == AFC_E_SUCCESS) {
				do_post_notification(NP_SYNC_DID_START);
				break;
			} else if (aerr == AFC_E_OP_WOULD_BLOCK) {
				usleep(LOCK_WAIT);
				continue;
			} else {
				fprintf(stderr, "ERROR: could not lock file! error code: %d\n", aerr);
				afc_file_close(afc, lockfile);
				lockfile = 0;
				cmd = CMD_LEAVE;
			}
		}
		if (i == LOCK_ATTEMPTS) {
			fprintf(stderr, "ERROR: timeout while locking for sync\n");
			afc_file_close(afc, lockfile);
			lockfile = 0;
			cmd = CMD_LEAVE;
		}
	}
	
checkpoint:
	
	switch(cmd) {
		case CMD_BACKUP:
			PRINT_VERBOSE(1, "Starting backup...\n");
			
			 make sure backup device sub-directory exists 
			gchar *devbackupdir = g_build_path(G_DIR_SEPARATOR_S, backup_directory, uuid, NULL);
			g_mkdir(devbackupdir, 0755);
			g_free(devbackupdir);
			
			 TODO: check domain com.apple.mobile.backup key RequiresEncrypt and WillEncrypt with lockdown 
			 TODO: verify battery on AC enough battery remaining 	
			
			 re-create Info.plist (Device infos, IC-Info.sidb, photos, app_ids, iTunesPrefs) 
			if (info_plist) {
				plist_free(info_plist);
				info_plist = NULL;
			}
			info_plist = mobilebackup_factory_info_plist_new();
			remove(info_path);
			plist_write_to_filename(info_plist, info_path, PLIST_FORMAT_XML);
			g_free(info_path);
			
			plist_free(info_plist);
			info_plist = NULL;
			
			 request backup from device with manifest from last backup 
			PRINT_VERBOSE(1, "Requesting backup from device...\n");
			
			err = mobilebackup2_send_request(mobilebackup2, "Backup", uuid, NULL, NULL);
			if (err == MOBILEBACKUP2_E_SUCCESS) {
				if (is_full_backup) {
					PRINT_VERBOSE(1, "Full backup mode.\n");
				}	else {
					PRINT_VERBOSE(1, "Incremental backup mode.\n");
				}
			} else {
				if (err == MOBILEBACKUP2_E_BAD_VERSION) {
					printf("ERROR: Could not start backup process: backup protocol version mismatch!\n");
				} else if (err == MOBILEBACKUP2_E_REPLY_NOT_OK) {
					printf("ERROR: Could not start backup process: device refused to start the backup process.\n");
				} else {
					printf("ERROR: Could not start backup process: unspecified error occured\n");
				}
				cmd = CMD_LEAVE;
			}
			break;
		case CMD_RESTORE:
			 TODO: verify battery on AC enough battery remaining 
			
			 verify if Status.plist says we read from an successful backup 
			if (!mb2_status_check_snapshot_state(backup_directory, uuid, "finished")) {
				printf("ERROR: Cannot ensure we restore from a successful backup. Aborting.\n");
				cmd = CMD_LEAVE;
				break;
			}
			
			PRINT_VERBOSE(1, "Starting Restore...\n");
			
			opts = plist_new_dict();
			plist_dict_insert_item(opts, "RestoreSystemFiles", plist_new_bool(cmd_flags & CMD_FLAG_RESTORE_SYSTEM_FILES));
			PRINT_VERBOSE(1, "Restoring system files: %s\n", (cmd_flags & CMD_FLAG_RESTORE_SYSTEM_FILES ? "Yes":"No"));
			if ((cmd_flags & CMD_FLAG_RESTORE_REBOOT) == 0)
				plist_dict_insert_item(opts, "RestoreShouldReboot", plist_new_bool(0));
			PRINT_VERBOSE(1, "Rebooting after restore: %s\n", (cmd_flags & CMD_FLAG_RESTORE_REBOOT ? "Yes":"No"));
			if ((cmd_flags & CMD_FLAG_RESTORE_COPY_BACKUP) == 0)
				plist_dict_insert_item(opts, "RestoreDontCopyBackup", plist_new_bool(1));
			PRINT_VERBOSE(1, "Don't copy backup: %s\n", ((cmd_flags & CMD_FLAG_RESTORE_COPY_BACKUP) == 0 ? "Yes":"No"));
			plist_dict_insert_item(opts, "RestorePreserveSettings", plist_new_bool((cmd_flags & CMD_FLAG_RESTORE_SETTINGS) == 0));
			PRINT_VERBOSE(1, "Preserve settings of device: %s\n", ((cmd_flags & CMD_FLAG_RESTORE_SETTINGS) == 0  ? "Yes":"No"));
			
			err = mobilebackup2_send_request(mobilebackup2, "Restore", uuid, uuid, opts);
			plist_free(opts);
			if (err != MOBILEBACKUP2_E_SUCCESS) {
				if (err == MOBILEBACKUP2_E_BAD_VERSION) {
					printf("ERROR: Could not start restore process: backup protocol version mismatch!\n");
				} else if (err == MOBILEBACKUP2_E_REPLY_NOT_OK) {
					printf("ERROR: Could not start restore process: device refused to start the restore process.\n");
				} else {
					printf("ERROR: Could not start restore process: unspecified error occured\n");
				}
				cmd = CMD_LEAVE;
			}
			break;
		case CMD_INFO:
			PRINT_VERBOSE(1, "Requesting backup info from device...\n");
			err = mobilebackup2_send_request(mobilebackup2, "Info", uuid, NULL, NULL);
			if (err != MOBILEBACKUP2_E_SUCCESS) {
				printf("Error requesting backup info from device, error code %d\n", err);
				cmd = CMD_LEAVE;
			}
			break;
		case CMD_LIST:
			PRINT_VERBOSE(1, "Requesting backup list from device...\n");
			err = mobilebackup2_send_request(mobilebackup2, "List", uuid, NULL, NULL);
			if (err != MOBILEBACKUP2_E_SUCCESS) {
				printf("Error requesting backup list from device, error code %d\n", err);
				cmd = CMD_LEAVE;
			}
			break;
		case CMD_UNBACK:
			PRINT_VERBOSE(1, "Starting to unpack backup...\n");
			err = mobilebackup2_send_request(mobilebackup2, "Unback", uuid, NULL, NULL);
			if (err != MOBILEBACKUP2_E_SUCCESS) {
				printf("Error requesting unback operation from device, error code %d\n", err);
				cmd = CMD_LEAVE;
			}
			break;
		default:
			break;
	}
	
	 close down the lockdown connection as it is no longer needed 
	if (client) {
		lockdownd_client_free(client);
		client = NULL;
	}
	
	if (cmd != CMD_LEAVE) {
		 reset operation success status 
		int operation_ok = 0;
		plist_t message = NULL;
		
		char *dlmsg = NULL;
		int file_count = 0;
		int errcode = 0;
		const char *errdesc = NULL;
		
		 process series of DLMessage* operations 
		do {
			if (dlmsg) {
				free(dlmsg);
				dlmsg = NULL;
			}
			mobilebackup2_receive_message(mobilebackup2, &message, &dlmsg);
			if (!message || !dlmsg) {
				PRINT_VERBOSE(1, "Device is not ready yet. Going to try again in 2 seconds...\n");
				sleep(2);
				goto files_out;
			}
			
			if (!strcmp(dlmsg, "DLMessageDownloadFiles")) {
				 device wants to download files from the computer 
				mb2_handle_send_files(message, backup_directory);
			} else if (!strcmp(dlmsg, "DLMessageUploadFiles")) {
				 device wants to send files to the computer 
				file_count += mb2_handle_receive_files(message, backup_directory);
			} else if (!strcmp(dlmsg, "DLContentsOfDirectory")) {
				 list directory contents 
				mb2_handle_list_directory(message, backup_directory);
			} else if (!strcmp(dlmsg, "DLMessageCreateDirectory")) {
				 make a directory 
				mb2_handle_make_directory(message, backup_directory);
			} else if (!strcmp(dlmsg, "DLMessageMoveFiles")) {
				 perform a series of rename operations 
				plist_t moves = plist_array_get_item(message, 1);
				uint32_t cnt = plist_dict_get_size(moves);
				PRINT_VERBOSE(1, "Moving %d file%s\n", cnt, (cnt == 1) ? "" : "s");
				plist_dict_iter iter = NULL;
				plist_dict_new_iter(moves, &iter);
				errcode = 0;
				errdesc = NULL;
				if (iter) {
					char *key = NULL;
					plist_t val = NULL;
					do {
						plist_dict_next_item(moves, iter, &key, &val);
						if (key && (plist_get_node_type(val) == PLIST_STRING)) {
							char *str = NULL;
							plist_get_string_val(val, &str);
							if (str) {
								gchar *newpath = g_build_path(G_DIR_SEPARATOR_S, backup_directory, str, NULL);
								g_free(str);
								gchar *oldpath = g_build_path(G_DIR_SEPARATOR_S, backup_directory, key, NULL);
								
								remove(newpath);
								if (rename(oldpath, newpath) < 0) {
									printf("Renameing '%s' to '%s' failed: %s (%d)\n", oldpath, newpath, strerror(errno), errno);
									errcode = errno_to_device_error(errno);
									errdesc = strerror(errno);
									break;
								}
								g_free(oldpath);
								g_free(newpath);
							}
							free(key);
							key = NULL;
						}
					} while (val);
					free(iter);
				} else {
					errcode = -1;
					errdesc = "Could not create dict iterator";
					printf("Could not create dict iterator\n");
				}
				err = mobilebackup2_send_status_response(mobilebackup2, errcode, errdesc, plist_new_dict());
				if (err != MOBILEBACKUP2_E_SUCCESS) {
					printf("Could not send status response, error %d\n", err);
				}
			} else if (!strcmp(dlmsg, "DLMessageRemoveFiles")) {
				plist_t removes = plist_array_get_item(message, 1);
				uint32_t cnt = plist_array_get_size(removes);
				PRINT_VERBOSE(1, "Removing %d file%s\n", cnt, (cnt == 1) ? "" : "s");
				uint32_t ii = 0;
				errcode = 0;
				errdesc = NULL;
				for (ii = 0; ii < cnt; ii++) {
					plist_t val = plist_array_get_item(removes, ii);
					if (plist_get_node_type(val) == PLIST_STRING) {
						char *str = NULL;
						plist_get_string_val(val, &str);
						if (str) {
							gchar *newpath = g_build_path(G_DIR_SEPARATOR_S, backup_directory, str, NULL);
							g_free(str);
							if (remove(newpath) < 0) {
								printf("Could not remove '%s': %s (%d)\n", newpath, strerror(errno), errno);
								errcode = errno_to_device_error(errno);
								errdesc = strerror(errno);
							}
							g_free(newpath);
						}
					}
				}
				err = mobilebackup2_send_status_response(mobilebackup2, errcode, errdesc, plist_new_dict());
				if (err != MOBILEBACKUP2_E_SUCCESS) {
					printf("Could not send status response, error %d\n", err);
				}
			} else if (!strcmp(dlmsg, "DLMessageCopyItem")) {
				plist_t srcpath = plist_array_get_item(message, 1);
				plist_t dstpath = plist_array_get_item(message, 2);
				errcode = 0;
				errdesc = NULL;
				if ((plist_get_node_type(srcpath) == PLIST_STRING) && (plist_get_node_type(dstpath) == PLIST_STRING)) {
					char *src = NULL;
					char *dst = NULL;
					plist_get_string_val(srcpath, &src);
					plist_get_string_val(dstpath, &dst);
					if (src && dst) {
						gchar *oldpath = g_build_path(G_DIR_SEPARATOR_S, backup_directory, src, NULL);
						gchar *newpath = g_build_path(G_DIR_SEPARATOR_S, backup_directory, dst, NULL);
						
						PRINT_VERBOSE(1, "Copying '%s' to '%s'\n", src, dst);
						
						 check that src exists 
						if (g_file_test(oldpath, G_FILE_TEST_IS_DIR)) {
							mb2_copy_directory_by_path(oldpath, newpath);
						} else if (g_file_test(oldpath, G_FILE_TEST_IS_REGULAR)) {
							mb2_copy_file_by_path(oldpath, newpath);
						}
						
						g_free(newpath);
						g_free(oldpath);
					}
					g_free(src);
					g_free(dst);
				}
				
				err = mobilebackup2_send_status_response(mobilebackup2, errcode, errdesc, plist_new_dict());
				if (err != MOBILEBACKUP2_E_SUCCESS) {
					printf("Could not send status response, error %d\n", err);
				}
			} else if (!strcmp(dlmsg, "DLMessageDisconnect")) {
				break;
			} else if (!strcmp(dlmsg, "DLMessageProcessMessage")) {
				node_tmp = plist_array_get_item(message, 1);
				if (plist_get_node_type(node_tmp) != PLIST_DICT) {
					printf("Unknown message received!\n");
				}
				plist_t nn;
				int error_code = -1;
				nn = plist_dict_get_item(node_tmp, "ErrorCode");
				if (nn && (plist_get_node_type(nn) == PLIST_UINT)) {
					uint64_t ec = 0;
					plist_get_uint_val(nn, &ec);
					error_code = (uint32_t)ec;
					if (error_code == 0) {
						operation_ok = 1;
					}
				}
				nn = plist_dict_get_item(node_tmp, "ErrorDescription");
				char *str = NULL;
				if (nn && (plist_get_node_type(nn) == PLIST_STRING)) {
					plist_get_string_val(nn, &str);
				}
				if (error_code != 0) {
					if (str) {
						printf("ErrorCode %d: %s\n", error_code, str);
					} else {
						printf("ErrorCode %d: (Unknown)\n", error_code);
					}
				}
				if (str) {
					free(str);
				}
				nn = plist_dict_get_item(node_tmp, "Content");
				if (nn && (plist_get_node_type(nn) == PLIST_STRING)) {
					str = NULL;
					plist_get_string_val(nn, &str);
					PRINT_VERBOSE(1, "Content:\n");
					printf("%s", str);
					free(str);
				}
				
				break;
			}
			
			 print status 
			if (plist_array_get_size(message) >= 3) {
				plist_t pnode = plist_array_get_item(message, 3);
				if (pnode && (plist_get_node_type(pnode) == PLIST_REAL)) {
					double progress = 0.0;
					plist_get_real_val(pnode, &progress);
					if (progress > 0) {
						print_progress_real(progress, 0);
						PRINT_VERBOSE(1, " Finished\n");
					}
				}
			}
			
			if (message)
				plist_free(message);
			message = NULL;
			
		files_out:
			if (quit_flag > 0) {
				 need to cancel the backup here 
					//mobilebackup_send_error(mobilebackup, "Cancelling DLSendFile");
				
				 remove any atomic Manifest.plist.tmp 
				
				manifest_path = mobilebackup_build_path(backup_directory, "Manifest", ".plist.tmp");
				 if (stat(manifest_path, &st) == 0)
				 remove(manifest_path);
				break;
			}
		} while (1);
		
		 report operation status to user 
		switch (cmd) {
			case CMD_BACKUP:
				PRINT_VERBOSE(1, "Received %d files from device.\n", file_count);
				if (mb2_status_check_snapshot_state(backup_directory, uuid, "finished")) {
					PRINT_VERBOSE(1, "Backup Successful.\n");
				} else {
					if (quit_flag) {
						PRINT_VERBOSE(1, "Backup Aborted.\n");
					} else {
						PRINT_VERBOSE(1, "Backup Failed.\n");
					}
				}
				break;
			case CMD_UNBACK:
				if (quit_flag) {
					PRINT_VERBOSE(1, "Unback Aborted.\n");
				} else {
					PRINT_VERBOSE(1, "The files can now be found in the \"_unback_\" directory.\n");
					PRINT_VERBOSE(1, "Unback Successful.\n");
				}
				break;
			case CMD_RESTORE:
				if (cmd_flags & CMD_FLAG_RESTORE_REBOOT)
					PRINT_VERBOSE(1, "The device should reboot now.\n");
				if (operation_ok) {
					PRINT_VERBOSE(1, "Restore Successful.\n");
				} else {
					PRINT_VERBOSE(1, "Restore Failed.\n");
				}
				
				break;
			case CMD_INFO:
			case CMD_LIST:
			case CMD_LEAVE:
			default:
				if (quit_flag) {
					PRINT_VERBOSE(1, "Operation Aborted.\n");
				} else if (cmd == CMD_LEAVE) {
					PRINT_VERBOSE(1, "Operation Failed.\n");
				} else {
					PRINT_VERBOSE(1, "Operation Successful.\n");
				}
				break;
		}
	}
	if (lockfile) {
		afc_file_lock(afc, lockfile, AFC_LOCK_UN);
		afc_file_close(afc, lockfile);
		lockfile = 0;
		if (cmd == CMD_BACKUP)
			do_post_notification(NP_SYNC_DID_FINISH);
	}
} else {
	printf("ERROR: Could not start service %s.\n", MOBILEBACKUP2_SERVICE_NAME);
	lockdownd_client_free(client);
	client = NULL;
}

if (client) {
	lockdownd_client_free(client);
	client = NULL;
}

if (mobilebackup2)
mobilebackup2_client_free(mobilebackup2);

if (afc)
afc_client_free(afc);

if (np)
np_client_free(np);

idevice_free(phone);

return 0;
}
 
 

