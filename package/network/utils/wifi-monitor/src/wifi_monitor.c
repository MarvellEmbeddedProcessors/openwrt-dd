#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <pthread.h>
#include <libubus.h>
#include <libubox/ulog.h>
#include <linux/nl80211.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>

#include "wifi_monitor.h"

static int driver_state;
static struct nl80211_global nl_global;
static int err_exit = 0;

static void local_time_str(char *time_str, int size)
{
	time_t time_utc;
	time_t time_local;
	struct tm tm_local;
	struct tm tm_gmt;
	int time_zone;

	time(&time_utc);
	localtime_r(&time_utc, &tm_local);

	snprintf(time_str, size, "%04d-%02d-%02dT%02d:%02d:%02d: ",
			tm_local.tm_year + 1900,
			tm_local.tm_mon + 1,
			tm_local.tm_mday,
			tm_local.tm_hour,
			tm_local.tm_min,
			tm_local.tm_sec);
}

static void write_file(const char *path, const char *msg) {
	FILE *fp;
	char buf[512];

	fp = fopen(path, "a+");
	if (NULL == fp) {
		ULOG_ERR("open file %s failed\n", path);
		return;
	}
	fseek(fp, 0, SEEK_END);

	memset(buf, 0, sizeof(buf));
	local_time_str(buf, sizeof(buf) - 1);
	if ((strlen(buf)+strlen(msg)) > (sizeof(buf) - 1)) {
		ULOG_ERR("msg too long, cannot write to file");
		return;
	} else
		strcat(buf, msg);

	fwrite(buf, strlen(buf), 1, fp);
	fflush(fp);
	fclose(fp);
}

static int insmod(const char *path, const char *name, const char *options)
{
	void *data = 0;
	struct stat s;
	int fd, ret = -1;
	char full_path[128];

	memset(full_path, 0, sizeof(full_path));
	snprintf(full_path, sizeof(full_path)-1, "%s/%s.ko",
			path, name);

	if (stat(full_path, &s)) {
		ULOG_ERR("missing module %s\n", full_path);
		return ret;
	}

	fd = open(full_path, O_RDONLY);
	if (fd < 0) {
		ULOG_ERR("cannot open %s\n", full_path);
		return ret;
	}

	data = malloc(s.st_size);
	if (!data) {
		ULOG_ERR("out of memory\n");
		goto out;
	}

	if (read(fd, data, s.st_size) == s.st_size) {
		ret = syscall(__NR_init_module, data, (unsigned long) s.st_size, options);
		if (errno == EEXIST)
			ret = 0;
	} else
		ULOG_ERR("failed to read full module %s\n", full_path);

out:
	close(fd);
	free(data);

	return ret;	
}

static int rmmod(const char *modname)
{
	int ret = -1;

	ret = syscall(__NR_delete_module, modname, 0);
	return ret;
}

static int pcie_reset(void)
{
/*
	int fd;
	int delay_time = 200 * 1000; // 200ms

	fd = open(PCIE_RESET_PATH, O_WRONLY);
	if (fd < 0)
		return -1;
		
	write(fd, "1", 1);
	usleep(delay_time);
	write(fd, "0", 1);
	close(fd);
	usleep(delay_time);
*/
	FILE *fp = NULL;
	uint32_t value = 0;
	char value_str[56];
	char cmd_str[56];

	fp = popen("devmem 0xd0018818", "r");
	if (!fp)
		return -1;

	memset(value_str, 0, sizeof(value_str));
	fgets(value_str, sizeof(value_str)-1, fp);
	fclose(fp);
	if (sscanf(value_str, "%x", &value) != 1)
		return -1;

	memset(cmd_str, 0, sizeof(cmd_str));
	snprintf(cmd_str, sizeof(cmd_str)-1, "%s 0x%x",
			"devmem 0xd0018818 32", value & 0xfffffff7);
	system(cmd_str);
	usleep(200000);

	memset(cmd_str, 0, sizeof(cmd_str));
	snprintf(cmd_str, sizeof(cmd_str)-1, "%s 0x%x",
			"devmem 0xd0018818 32", value | 0x8);
	system(cmd_str);
	usleep(200000);

	return 0;
}

static int pcie_rescan(void)
{
	int fd;
	int delay_time =  500 * 1000; // 500ms

	fd = open(PCIE_REMOVE_PATH, O_WRONLY);
	if (fd >= 0) {
		write(fd, "1", 1);
		close(fd);
	} else
		return -1;

	usleep(delay_time);

	fd = open(PCIE_RESCAN_PATH, O_WRONLY);
	if (fd >= 0) {
		write(fd, "1", 1);
		close(fd);
	} else
		return -1;

	usleep(delay_time);
	return 0;
}

static int up_service(void)
{
	struct ubus_context *ctx;
	int ret = 0;
	uint32_t ubus_id = 0;

	ctx = ubus_connect(NULL);
	if (!ctx)
		return -1;

	ret = ubus_lookup_id(ctx, "network.wireless", &ubus_id);
	if (ret) {
		ubus_free(ctx);
		return ret;
	}

	ret = ubus_invoke(ctx, ubus_id, "up", NULL, NULL, NULL, 1000);

	ubus_free(ctx);
	return ret;
}

static int down_service(void)
{
	struct ubus_context *ctx;
	int ret = 0;
	uint32_t ubus_id = 0;

	ctx = ubus_connect(NULL);
	if (!ctx)
		return -1;

	ret = ubus_lookup_id(ctx, "network.wireless", &ubus_id);
	if (ret) {
		ubus_free(ctx);
		return ret;
	}

	ret = ubus_invoke(ctx, ubus_id, "down", NULL, NULL, NULL, 1000);

	ubus_free(ctx);
	return ret;
}

static void get_service_state_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	int *st;
	struct blob_attr *pos;
	struct blob_attr *attr = blobmsg_data(msg);
	struct blob_attr *radio0;
	int rem;

	st = req->priv;
	rem = blobmsg_data_len(msg);
	__blob_for_each_attr(pos, blobmsg_data(msg), rem) {
		if (0 == strcmp(blobmsg_name(pos), "radio0")) {
			radio0 = pos;
			break;
		}
	}

	rem = blobmsg_data_len(radio0);
	__blob_for_each_attr(pos, blobmsg_data(radio0), rem) {
		if (0 == strcmp(blobmsg_name(pos), "up")) {
			if (blobmsg_get_bool(pos))
				*st = SERVICE_UP;
			else
				*st = SERVICE_DOWN;
		}
	}
}

static int get_service_state(int *st)
{
	struct ubus_context *ctx;
	int ret = 0;
	uint32_t ubus_id = 0;

	ctx = ubus_connect(NULL);
	if (!ctx)
		return -1;

	ret = ubus_lookup_id(ctx, "network.wireless", &ubus_id);
	if (ret) {
		ubus_free(ctx);
		return ret;
	}

	ret = ubus_invoke(ctx, ubus_id, "status", NULL, get_service_state_cb, st, 1000);

	ubus_free(ctx);
	return ret;
}

static int wait_for_driver_state(int st)
{
	int times = 100; // wait for 10s
	int timeout = 1;
	int i;

	for (i = 0; i < times; i++) {
		if (driver_state == st) {
			timeout = 0;
			break;
		}
		usleep(100*1000);
	}

	return timeout;
}

static int wait_for_service_state(int st)
{
	int times = 100; // wait for 10s
	int timeout = 1;
	int service_state = SERVICE_UNKNOWN;
	int i;

	for (i = 0; i < times; i++) {
		get_service_state(&service_state);
		if (service_state == st) {
			timeout = 0;
			break;
		}
		usleep(100*1000);
	}

	return timeout;
}

static int error_handler(struct sockaddr_nl *nla, struct nlmsgerr *err,
			 void *arg)
{
	int *ret = arg;
	*ret = err->error;
	return NL_STOP;
}

static int ack_handler(struct nl_msg *msg, void *arg)
{
	int *ret = arg;
	*ret = 0;
	return NL_STOP;
}

static int no_seq_check(struct nl_msg *msg, void *arg)
{
	return NL_OK;
}

struct handler_args {
	const char *group;
	int id;
};

static int family_handler(struct nl_msg *msg, void *arg)
{
	struct handler_args *grp = arg;
	struct nlattr *tb[CTRL_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *mcgrp;
	int rem_mcgrp;

	nla_parse(tb, CTRL_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		  genlmsg_attrlen(gnlh, 0), NULL);

	if (!tb[CTRL_ATTR_MCAST_GROUPS])
		return NL_SKIP;

	nla_for_each_nested(mcgrp, tb[CTRL_ATTR_MCAST_GROUPS], rem_mcgrp) {
		struct nlattr *tb_mcgrp[CTRL_ATTR_MCAST_GRP_MAX + 1];

		nla_parse(tb_mcgrp, CTRL_ATTR_MCAST_GRP_MAX,
			  nla_data(mcgrp), nla_len(mcgrp), NULL);

		if (!tb_mcgrp[CTRL_ATTR_MCAST_GRP_NAME] ||
		    !tb_mcgrp[CTRL_ATTR_MCAST_GRP_ID])
			continue;
		if (strncmp(nla_data(tb_mcgrp[CTRL_ATTR_MCAST_GRP_NAME]),
			    grp->group, nla_len(tb_mcgrp[CTRL_ATTR_MCAST_GRP_NAME])))
			continue;
		grp->id = nla_get_u32(tb_mcgrp[CTRL_ATTR_MCAST_GRP_ID]);
		break;
	}

	return NL_SKIP;
}

static int nl_get_multicast_id(struct nl_sock *sock, const char *family, const char *group)
{
	struct nl_msg *msg;
	struct nl_cb *cb;
	int ret, ctrlid;
	struct handler_args grp = {
		.group = group,
		.id = -ENOENT,
	};

	msg = nlmsg_alloc();
	if (!msg)
		return -ENOMEM;

	cb = nl_cb_alloc(NL_CB_DEFAULT);
	if (!cb) {
		ret = -ENOMEM;
		goto out_fail_cb;
	}

	ctrlid = genl_ctrl_resolve(sock, "nlctrl");

	genlmsg_put(msg, 0, 0, ctrlid, 0, 0, CTRL_CMD_GETFAMILY, 0);

	ret = -ENOBUFS;
	NLA_PUT_STRING(msg, CTRL_ATTR_FAMILY_NAME, family);

	ret = nl_send_auto_complete(sock, msg);
	if (ret < 0)
		goto out;

	ret = 1;

	nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &ret);
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &ret);
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, family_handler, &grp);

	while (ret > 0)
		nl_recvmsgs(sock, cb);

	if (ret == 0)
		ret = grp.id;
nla_put_failure:
out:
	nl_cb_put(cb);
out_fail_cb:
	nlmsg_free(msg);
	return ret;
}

static void nl80211_destroy_sock(struct nl_sock **sock)
{
	if (*sock == NULL)
		return;
	nl_socket_free(*sock);
	*sock = NULL;
}

static int nl80211_init_nl_global(struct nl80211_global *global)
{
	int ret = 0;

	memset(global, 0, sizeof(struct nl80211_global));

	global->nl_cb = nl_cb_alloc(NL_CB_DEFAULT);
	if (!global->nl_cb) {
		ULOG_ERR("Failed to allocate netlink callback\n");
		return -ENOMEM;
	}

	global->nl_sock = nl_socket_alloc_cb(global->nl_cb);
	if (!global->nl_sock) {
		ULOG_ERR("Failed to allocate netlink command socket\n");
		ret = -ENOMEM;
		goto err;
	}
	nl_socket_set_buffer_size(global->nl_sock, 8192, 8192);
	if (genl_connect(global->nl_sock)) {
		ULOG_ERR("Failed to connect to generic netlink\n");
		ret = -ENOLINK;
		goto err;
	}

	global->nl80211_id = genl_ctrl_resolve(global->nl_sock, "nl80211");
	if (global->nl80211_id < 0) {
		ULOG_ERR("nl80211 not find\n");
		ret = -ENOENT;
		goto err;
	}

	global->nl_event = nl_socket_alloc_cb(global->nl_cb);
	if (!global->nl_event) {
		ULOG_ERR("Failed to allocate netlink event socket\n");
		ret = -ENOMEM;
		goto err;
	}
	nl_socket_set_buffer_size(global->nl_event, 8192, 8192);
	if (genl_connect(global->nl_event)) {
		ULOG_ERR("Failed to connect to generic netlink\n");
		ret = -ENOLINK;
		goto err;
	}

	return 0;

err:
	nl80211_destroy_sock(&global->nl_sock);
	nl80211_destroy_sock(&global->nl_event);
	nl_cb_put(global->nl_cb);
	global->nl_cb = NULL;
	return ret;
}

static void nl80211_destroy_nl_global(struct nl80211_global *global)
{
	global->ready = 0;
	nl80211_destroy_sock(&global->nl_sock);
	nl80211_destroy_sock(&global->nl_event);
	nl_cb_put(global->nl_cb);
	global->nl_cb = NULL;
}

static int  nl80211_setup_event_monitor(struct nl80211_global *global);
static void * handle_cmd_timeout(void *arg)
{
	int ret = -1;
	int service_state = SERVICE_UNKNOWN;
	char logmsg[128];

	memset(logmsg, 0, sizeof(logmsg));
	write_file(LOG_FILE, "recv cmd timeout, triggle wifi restart\n");

	/* wifi down */
	get_service_state(&service_state);
	down_service();
	if (SERVICE_UP == service_state)
		wait_for_service_state(SERVICE_DOWN);

	/* rmmod mwlwifi mac80211 cfg80211 */
	ret = rmmod(MODULE_NAME);
	if (ret) {
		strncpy(logmsg, "rmmod wifi driver failed\n", sizeof(logmsg)-1);
		goto err;
	}
	nl80211_destroy_nl_global(&nl_global);
	rmmod("mac80211");
	rmmod("cfg80211");
	driver_state = DRIVER_UNLOADED;

	/* pcie reset gpio */
	ret = pcie_reset();
	if (ret) {
		strncpy(logmsg, "pcie reset failed\n", sizeof(logmsg)-1);
		goto err;
	}

	/* pcie rescan */
	ret = pcie_rescan();
	if (ret) {
		strncpy(logmsg, "pcie rescan failed\n", sizeof(logmsg)-1);
		goto err;
	}

	/* insmod cfg80211 mac80211 mwlwifi */
	driver_state = DRIVER_LOADING;
	insmod(MODULE_PATH, "cfg80211", "");
	insmod(MODULE_PATH, "mac80211", "");
	nl80211_setup_event_monitor(&nl_global);
	ret = insmod(MODULE_PATH, MODULE_NAME, "");
	if (ret) {
		strncpy(logmsg, "insmod wifi driver failed\n", sizeof(logmsg)-1);
		goto err;
	}
	ret = wait_for_driver_state(DRIVER_LOADED);
	if (ret) {
		strncpy(logmsg, "wifi driver initialize timeout\n", sizeof(logmsg)-1);
		goto err;
	}

	/* wifi up */
	up_service();
	if (SERVICE_UP == service_state) {
		ret = wait_for_service_state(SERVICE_UP);
		if (ret) {
			strncpy(logmsg, "wifi up timeout\n", sizeof(logmsg)-1);
			goto err;
		}
	}

	write_file(LOG_FILE, "wifi restart finished\n");
	return NULL;

err:
	err_exit = 1;
	write_file(LOG_FILE, logmsg);
	return NULL;
}

static int nl80211_event_handler(struct nl_msg *msg, void *arg)
{
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	uint32_t vendor_id;
	uint32_t subcmd;
	pthread_t restart_pthread;

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		  genlmsg_attrlen(gnlh, 0), NULL);

	if (gnlh->cmd == NL80211_CMD_VENDOR) {
		vendor_id = nla_get_u32(tb[NL80211_ATTR_VENDOR_ID]);
		subcmd = nla_get_u32(tb[NL80211_ATTR_VENDOR_SUBCMD]);

		if (vendor_id != MRVL_OUI)
			return 0;

		ULOG_INFO("recv netlink event %d from driver\n", subcmd);

		switch(driver_state) {
			case DRIVER_LOADED:
				if (MWL_VENDOR_EVENT_CMD_TIMEOUT == subcmd) {
					driver_state = DRIVER_CMD_TIMEOUT;
					pthread_create(&restart_pthread, NULL, handle_cmd_timeout, NULL);
				}
				break;
			case DRIVER_LOADING:
				if (MWL_VENDOR_EVENT_DRIVER_READY == subcmd)
					driver_state = DRIVER_LOADED;
				break;
			default:
				break;
		}
	}

	return 0;
}

static int  nl80211_setup_event_monitor(struct nl80211_global *global)
{
	int mcid, ret;

	ret = nl80211_init_nl_global(global);
	if (ret) {
		ULOG_ERR("initialize nl80211 global struct failed\n");
		return ret;
	}

	mcid = nl_get_multicast_id(global->nl_sock, "nl80211", "vendor");
	if (mcid >= 0) {
		ret = nl_socket_add_membership(global->nl_event, mcid);
		if (ret)
			return ret;
	}

	/* no sequence checking for multicast messages */
	nl_cb_set(global->nl_cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_check, NULL);
	nl_cb_set(global->nl_cb, NL_CB_VALID, NL_CB_CUSTOM, nl80211_event_handler, NULL);
	global->ready = 1;

	return 0;
}

int main (int argc, char *argv[])
{
	ulog_open(ULOG_SYSLOG, LOG_DAEMON, "wifi_monitor");
	daemon(1, 1);

	driver_state = DRIVER_LOADED;

	memset(&nl_global, 0, sizeof(nl_global));
	nl80211_setup_event_monitor(&nl_global);

	while (!err_exit) {
		if (driver_state != DRIVER_CMD_TIMEOUT && nl_global.ready) {
			nl_recvmsgs(nl_global.nl_event, nl_global.nl_cb);
		}
	}

	return 0;
}
