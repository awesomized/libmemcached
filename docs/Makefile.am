INCLUDES = include

man_MANS = libmemcached.3\
  	libmemcached_examples.3\
	memcat.1\
	memcp.1\
	memflush.1\
	memrm.1\
	memslap.1\
	memstat.1\
	memcached_add.3\
	memcached_behavior_get.3\
	memcached_behavior_set.3\
	memcached_create.3\
	memcached_decrement.3\
	memcached_delete.3\
	memcached_fetch.3\
	memcached_free.3\
	memcached_get.3\
	memcached_increment.3\
	memcached_mget.3\
	memcached_replace.3\
	memcached_server_add.3\
	memcached_server_count.3\
	memcached_server_list.3\
	memcached_server_list_append.3\
	memcached_server_list_count.3\
	memcached_server_list_free.3\
	memcached_server_push.3\
	memcached_servers_parse.3\
	memcached_set.3\
	memcached_stat.3\
	memcached_stat_get_keys.3\
	memcached_stat_get_value.3\
	memcached_stat_servername.3\
	memcached_strerror.3\
	memcached_quit.3\
	memcached_verbosity.3

libmemcached.3: libmemcached.pod 
	pod2man libmemcached.pod > libmemcached.3

libmemcached_examples.3: libmemcached_examples.pod
	pod2man libmemcached_examples.pod > libmemcached_examples.3

memcached_create.3: memcached_create.pod 
	pod2man memcached_create.pod > memcached_create.3

memcached_free.3: memcached_create.pod
	pod2man memcached_create.pod > memcached_free.3

memcached_set.3: memcached_set.pod
	pod2man memcached_set.pod > memcached_set.3

memcached_replace.3: memcached_set.pod
	pod2man memcached_set.pod > memcached_replace.3

memcached_add.3: memcached_set.pod
	pod2man memcached_set.pod > memcached_add.3

memcached_delete.3: memcached_delete.pod
	pod2man memcached_delete.pod > memcached_delete.3

memcached_increment.3:
	pod2man memcached_auto.pod > memcached_increment.3

memcached_decrement.3:
	pod2man memcached_auto.pod > memcached_decrement.3

memcached_flush.3: memcached_flush.pod
	pod2man memcached_flush.pod > memcached_flush.3

memcached_get.3: memcached_get.pod
	pod2man memcached_get.pod > memcached_get.3

memcached_mget.3: memcached_get.pod
	pod2man memcached_get.pod > memcached_mget.3

memcached_fetch.3: memcached_get.pod
	pod2man memcached_get.pod > memcached_fetch.3

memcached_quit.3: memcached_quit.pod
	pod2man memcached_quit.pod > memcached_quit.3

memcached_strerror.3: memcached_strerror.pod
	pod2man memcached_strerror.pod > memcached_strerror.3

memcached_server_count.3: memcached_servers.pod
	pod2man memcached_servers.pod > memcached_server_count.3

memcached_server_list.3: memcached_servers.pod
	pod2man memcached_servers.pod > memcached_server_list.3

memcached_server_add.3: memcached_servers.pod
	pod2man memcached_servers.pod > memcached_server_add.3

memcached_server_push.3: memcached_servers.pod
	pod2man memcached_servers.pod > memcached_server_push.3

memcached_server_list_free.3: memcached_server_st.pod
	pod2man memcached_server_st.pod > memcached_server_list_free.3

memcached_server_list_count.3: memcached_server_st.pod
	pod2man memcached_server_st.pod > memcached_server_list_count.3

memcached_server_list_append.3: memcached_server_st.pod
	pod2man memcached_server_st.pod > memcached_server_list_append.3

memcached_servers_parse.3: memcached_server_st.pod
	pod2man memcached_server_st.pod > memcached_servers_parse.3

memcached_verbosity.3: memcached_verbosity.pod
	pod2man memcached_verbosity.pod > memcached_verbosity.3

memcached_behavior_get.3: memcached_behavior.pod
	pod2man memcached_behavior.pod > memcached_behavior_get.3

memcached_behavior_set.3: memcached_behavior.pod
	pod2man memcached_behavior.pod > memcached_behavior_set.3

memcached_stat.3: memcached_stats.pod
	pod2man memcached_stats.pod > memcached_stat.3

memcached_stat_servername.3: memcached_stats.pod
	pod2man memcached_stats.pod > memcached_stat_servername.3

memcached_stat_get_value.3: memcached_stats.pod
	pod2man memcached_stats.pod > memcached_stat_get_value.3

memcached_stat_get_keys.3: memcached_stats.pod
	pod2man memcached_stats.pod > memcached_stat_get_keys.3

memcp.1: memcp.pod
	pod2man memcp.pod > memcp.1

memslap.1: memslap.pod
	pod2man memslap.pod > memslap.1

memcat.1: memcat.pod
	pod2man memcat.pod > memcat.1

memstat.1: memstat.pod
	pod2man memstat.pod > memstat.1

memrm.1: memrm.pod
	pod2man memrm.pod > memrm.1

memflush.1: memflush.pod
	pod2man memflush.pod > memflush.1

