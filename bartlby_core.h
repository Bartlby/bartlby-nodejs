#include <time.h>
#include <sys/times.h> 
#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* calloc, exit, free */
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/ipc.h>
#include <sys/shm.h>


void * bartlby_get_shm(char * cfgfile);
char * getConfigValue(char * key, char * fname);
void * bartlby_get_sohandle(char * cfgfile);
	



#define LOAD_SYMBOL(x,y,z) 	x=dlsym(y, z); \
    	if((dlmsg=dlerror()) != NULL) { \
					ThrowException(Exception::TypeError(String::New(dlmsg))); \
    				return scope.Close(Undefined()); \
    	}
    	

#define MAX_GROUP_MEMBERS 512
#define GROUP_MEMBER_STR_LENGTH 2048

 
struct shm_counter {
	long worker;
	long services;
	long downtimes;
	long servers;	
	long servergroups;
	long servicegroups;
};

struct perf_statistic {
	long sum;
	long counter;	
};


struct snmpi {
	char community[512];
	int version;
	char objid[1024];
	long warn;
	long crit;
	long type;
	char textmatch[1024];
};
struct sprocess {
	int start_time;
	int pid;
		
};

struct sched_worker {
	int pid;
	struct service * svc;
	int start_time;
	int  idle;
	int shutdown;
	struct tms timing;
	int idx;


} astt;

struct notification_log_entry {
	int notification_valid; //-1 invalid == end of list
	long worker_id; //Worker id
	long service_id; //Service_id
	int state; //State
	int aggregated; //Default -1 > 0 - this notification has already been aggregated
	char trigger_name[512];
	int type; // 0 if it was a normal notification, 1 = it was a escalation notification to the standby's
	time_t time;
	int aggregation_interval;
	int received_via;
};
#define NOTIFICATION_LOG_MAX 512
#define NOTIFICATION_VIA_LOCAL 1
#define NOTIFICATION_VIA_UPSTREAM 2


struct shm_header {
	long size_of_structs;
	long thrdcount;
	long svccount;
	long wrkcount;
	long srvcount;
	long srvgroupcount;
	long svcgroupcount;
	long current_running;
	char  version[50];
	int do_reload;
	int last_replication;
	int startup_time;
	long dtcount;
	int sirene_mode;
	struct perf_statistic pstat;
	int cur_event_index;
	long checks_performed;
	int checks_performed_time;
	struct  sched_worker worker_threads[50];
	struct notification_log_entry notification_log[NOTIFICATION_LOG_MAX];
	long notification_log_current_top;	
	time_t notification_log_aggregate_last_run;
	
};

struct server {
	long server_id;
	char  client_ip[2048];
	char  server_name[2048];
	char server_icon[1024];
	int server_enabled;
	int client_port;
	int server_dead;
	int server_notify;
	long server_flap_seconds;
	int flap_count;
	int last_notify_send;
	
	struct service * dead_marker;
	int is_gone;
	
	struct servergroup * servergroups[MAX_GROUP_MEMBERS];
	long servergroup_counter;
	long servergroup_place[MAX_GROUP_MEMBERS];
	
	char server_ssh_keyfile[512];
	char server_ssh_passphrase[512];
	char server_ssh_username[512];
	
	char enabled_triggers[512];
	int default_service_type;
	int orch_id;
} xxyz;



struct service {
	long service_id;
	long server_id;
	int last_state;
	int current_state;
	
	char  new_server_text[2048];
	char  service_name[2048];
	
	char  plugin[2048];
	char  plugin_arguments[2048];
	long check_interval;
	long check_interval_original;
	int last_check;
	struct timeval lcheck;
	
	/*Time stuff*/
	
	char service_exec_plan[2048];
	
	/*Notify things*/
	int notify_enabled;
	int last_notify_send;
	int last_state_change;
	long flap_count;
	
	int service_active;
	
	char  service_var[2048];
	int service_type;
	long service_passive_timeout;
	
	int notify_last_state;
	
	long service_check_timeout;
	
	
	
	int service_ack_enabled;
	int service_ack_current;
	
	
	long service_retain;
	long service_retain_current;
	
	
	
	struct perf_statistic pstat;
	struct perf_statistic delay_time;
	
	int do_force;
	
	struct snmpi snmp_info;
	
	int recovery_outstanding; //Flag to see if recover is waiting
	
	struct sprocess process;
	
	long flap_seconds;
	
	struct server * srv;
	long srv_place;
	
	int is_server_dead;
	
	/**/
	long renotify_interval; // interval to renotify
	long escalate_divisor; //

	int is_gone;
	
	
	struct servicegroup * servicegroups[MAX_GROUP_MEMBERS];
	long servicegroup_counter;
	long servicegroup_place[MAX_GROUP_MEMBERS];
	
	long fires_events;
	
	char enabled_triggers[512];

	int handled;
	int orch_id;
	int last_orch_sync;
};

struct servicegroup {
	long servicegroup_id;
	char servicegroup_name[1024];
	int servicegroup_notify;
	int servicegroup_active;
	char servicegroup_members[GROUP_MEMBER_STR_LENGTH];
	
	int servicegroup_dead;
	struct service * dead_marker;
	
	char enabled_triggers[512];
	int orch_id;
};

struct servergroup {
	long servergroup_id;
	char servergroup_name[1024];
	int servergroup_notify;
	int servergroup_active;
	char servergroup_members[GROUP_MEMBER_STR_LENGTH];
	int servergroup_dead;
	struct service * dead_marker;
	char enabled_triggers[512];
	int orch_id;
};


struct service_sort {
	struct service * svc;	
};
struct sched_worker_sort {
	struct sched_worker * th;
};


struct worker {
	char name[2048];
	char  mail[2048];
	char  icq[2048];
	
	
	long worker_id;
	int active;
	char notify_plan[2048];
	char password[2048];
	
	int escalation_count;
	int escalation_time;
	char notify_levels[20];
	char enabled_triggers[2048];
	char t[500];
	long escalation_limit;
	long escalation_minutes;
	int is_gone;

	char  visible_services[2048];
	char  visible_servers[2048];
	char  selected_services[2048];
	char  selected_servers[2048];
	int  notification_aggregation_interval;
	int is_super_user;

	int orch_id;
}sa;


struct downtime {
	long downtime_id;
	int downtime_type;
	int downtime_from;
	int downtime_to;
	char downtime_notice[2048];
	int service_id;
	
	int is_gone;

	int orch_id;
	
}sb;

struct btl_event {
	long evnt_id;
	char evnt_message[1024];
	int evnt_time;
		
}eb;
struct ext_notify {
	struct service * svc;	
	struct worker * wrk;
	char * trigger;
} ty;



typedef struct v2_packet_struct{

	u_int32_t crc32_value;
	int16_t   exit_code;
	int16_t   packet_type;
	char      output[2048];
	char      cmdline[2048];
	char      plugin[2048];
	char 	   perf_handler[1024];
	
	 
} agent_v2_packet;

typedef struct port_packet_struct{

	u_int32_t crc32_value;
	int16_t   exit_code;
	int16_t   packet_type;
	char      output[2048];
	char      cmdline[2048];
	char      plugin[2048];
	char 	   perf_handler[1024];
	int32_t	   service_id;
	
	 
} portier_packet;