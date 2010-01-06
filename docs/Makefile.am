# This file generates all of man/html pages that we use for documentation.
#
# When hacking this file you need to know that we take .pod files and turn
# them into .pop files. .pop files are 1=1 for man pages, but one .pod
# file may generate many .pop files.
#
#    -Brian
#
#
CLEANFILES= *.1 *.3 *.html *.pop

AUTO_PAGES= \
	memcached_increment \
	memcached_increment_with_initial \
	memcached_decrement \
	memcached_decrement_with_initial

SET_PAGES= \
	   memcached_set \
	   memcached_set_by_key \
	   memcached_cas \
	   memcached_cas_by_key \
	   memcached_replace \
	   memcached_replace_by_key \
	   memcached_add memcached_add_by_key \
	   memcached_prepend \
	   memcached_prepend_by_key \
	   memcached_append \
	   memcached_append_by_key

CREATE_PAGES= \
	     memcached_create \
	     memcached_free \
	     memcached_clone

DELETE_PAGES= \
	      memcached_delete \
	      memcached_delete_by_key

GET_PAGES= \
	   memcached_get \
	   memcached_get_by_key \
	   memcached_fetch_result \
	   memcached_fetch_execute \
	   memcached_mget \
	   memcached_mget_by_key \
	   memcached_mget_execute \
	   memcached_mget_execute_by_key \
	   memcached_fetch

RESULT_PAGES= \
	      memcached_result_cas \
	      memcached_result_create \
	      memcached_result_flags \
	      memcached_result_free \
	      memcached_result_key_length \
	      memcached_result_key_value \
	      memcached_result_length \
	      memcached_result_st \
	      memcached_result_value


SERVER_PAGES= \
	      memcached_server_count \
	      memcached_server_list \
	      memcached_server_add \
	      memcached_server_add_unix_socket \
	      memcached_server_push 

SERVER_ST_PAGES= \
		 memcached_server_list_free \
		 memcached_server_list_count \
		 memcached_server_list_append \
		 memcached_servers_parse 

HTML_FILES= \
	    hashkit_create.html \
	    hashkit_functions.html \
	    hashkit_value.html \
	    libmemcached_examples.html \
	    libmemcached.html \
	    libmemcachedutil.html \
	    memcached_analyze.html \
	    memcached_auto.html \
	    memcached_behavior.html \
	    memcached_callback.html \
	    memcached_create.html \
	    memcached_delete.html \
	    memcached_dump.html \
	    memcached_flush_buffers.html \
	    memcached_flush.html \
	    memcached_generate_hash_value.html \
	    memcached_get.html \
	    memcached_memory_allocators.html \
	    memcached_pool.html \
	    memcached_quit.html \
	    memcached_result_st.html \
	    memcached_servers.html \
	    memcached_server_st.html \
	    memcached_set.html \
	    memcached_stats.html \
	    memcached_strerror.html \
	    memcached_user_data.html \
	    memcached_verbosity.html \
	    memcached_version.html \
	    memcapable.html \
	    memcat.html \
	    memcp.html \
	    memdump.html \
	    memerror.html \
	    memflush.html \
	    memrm.html \
	    memslap.html \
	    memstat.html

EXTRA_DIST = \
	  hashkit_create.pod \
	  hashkit_functions.pod \
	  hashkit_value.pod \
	  libmemcached.pod \
	  libmemcached_examples.pod \
	  libmemcachedutil.pod \
	  memcached_analyze.pod \
	  memcached_auto.pod \
	  memcached_behavior.pod \
	  memcached_callback.pod \
	  memcached_create.pod \
	  memcached_delete.pod \
	  memcached_dump.pod \
	  memcached_flush.pod \
	  memcached_flush_buffers.pod \
	  memcached_generate_hash_value.pod \
	  memcached_get.pod \
	  memcached_memory_allocators.pod \
	  memcached_pool.pod \
	  memcached_quit.pod \
	  memcached_server_st.pod \
	  memcached_servers.pod \
	  memcached_set.pod \
	  memcached_stats.pod \
	  memcached_strerror.pod \
	  memcached_user_data.pod \
	  memcached_verbosity.pod \
	  memcached_version.pod \
	  memcapable.pod \
	  memcat.pod \
	  memcp.pod \
	  memdump.pod \
	  memerror.pod \
	  memflush.pod \
	  memrm.pod \
	  memslap.pod \
	  memstat.pod

man_MANS = ${set_} \
	  libmemcached.3 \
	  libmemcached_examples.3 \
	  memcached_add.3 \
	  memcached_add_by_key.3 \
	  memcached_analyze.3 \
	  memcached_append.3 \
	  memcached_append_by_key.3 \
	  memcached_behavior_get.3 \
	  memcached_behavior_set.3 \
	  memcached_callback_get.3 \
	  memcached_callback_set.3 \
	  memcached_cas.3 \
	  memcached_cas_by_key.3 \
	  memcached_clone.3 \
	  memcached_create.3 \
	  memcached_decrement.3 \
	  memcached_decrement_with_initial.3 \
	  memcached_delete.3 \
	  memcached_delete_by_key.3 \
	  memcached_dump.3 \
	  memcached_fetch.3 \
	  memcached_fetch_execute.3 \
	  memcached_fetch_result.3 \
	  memcached_flush_buffers.3 \
	  memcached_free.3 \
	  memcached_generate_hash_value.3 \
	  memcached_get.3 \
	  memcached_get_by_key.3 \
	  memcached_get_memory_allocators.3 \
	  memcached_get_user_data.3 \
	  memcached_increment.3 \
	  memcached_increment_with_initial.3 \
	  memcached_lib_version.3 \
	  memcached_mget.3 \
	  memcached_mget_by_key.3 \
	  memcached_mget_execute.3  \
	  memcached_mget_execute_by_key.3  \
	  memcached_prepend.3 \
	  memcached_prepend_by_key.3 \
	  memcached_quit.3 \
	  memcached_replace.3 \
	  memcached_replace_by_key.3 \
	  memcached_server_add.3 \
	  memcached_server_count.3 \
	  memcached_server_list.3 \
	  memcached_server_list_append.3 \
	  memcached_server_list_count.3 \
	  memcached_server_list_free.3 \
	  memcached_server_push.3 \
	  memcached_servers_parse.3 \
	  memcached_set.3 \
	  memcached_set_by_key.3 \
	  memcached_set_memory_allocators.3 \
	  memcached_set_user_data.3 \
	  memcached_stat.3 \
	  memcached_stat_get_keys.3 \
	  memcached_stat_get_value.3 \
	  memcached_stat_servername.3 \
	  memcached_strerror.3 \
	  memcached_verbosity.3 \
	  memcached_version.3 \
	  memcapable.1 \
	  memcat.1 \
	  memcp.1 \
	  memdump.1 \
	  memerror.1 \
	  memflush.1 \
	  memrm.1 \
	  memslap.1 \
	  memstat.1

if BUILD_LIBMEMCACHEDUTIL
man_MANS+= \
	  libmemcachedutil.3 \
	  memcached_pool_behavior_set.3 \
	  memcached_pool_behavior_get.3 \
	  memcached_pool_create.3 \
	  memcached_pool_destroy.3 \
	  memcached_pool_push.3 \
	  memcached_pool_pop.3
endif


${CREATE_PAGES}: 
	@rm -f $@.pop
	ln -s ${top_srcdir}/docs/memcached_create.pod ${top_builddir}/docs/$@.pop

${SET_PAGES}: 
	@rm -f $@.pop
	ln -s ${top_srcdir}/docs/memcached_set.pod ${top_builddir}/docs/$@.pop

${DELETE_PAGES}:
	@rm -f $@.pop
	ln -s ${top_srcdir}/docs/memcached_delete.pod ${top_builddir}/docs/$@.pop

${AUTO_PAGES}:
	@rm -f $@.pop
	ln -s ${top_srcdir}/docs/memcached_auto.pod ${top_builddir}/docs/$@.pop

${GET_PAGES}:
	@rm -f $@.pop
	ln -s ${top_srcdir}/docs/memcached_get.pod ${top_builddir}/docs/$@.pop

${SERVER_PAGES}:
	@rm -f $@.pop
	ln -s ${top_srcdir}/docs/memcached_servers.pod ${top_builddir}/docs/$@.pop

${SERVER_ST_PAGES}:
	@rm -f $@.pop
	ln -s ${top_srcdir}/docs/memcached_server_st.pod ${top_builddir}/docs/$@.pop

${GENERIC_PAGES}:
	@rm -f $@.pop
	ln -s ${top_srcdir}/docs/$@.pod ${top_builddir}/docs/$@.pop

GENERIC_PAGES= \
	libmemcached \
	libmemcached_examples \
	libmemcachedutil \
	memcached_analyze \
	memcached_dump \
	memcached_flush \
	memcached_flush_buffers \
	memcached_generate_hash_value \
	memcached_quit \
	memcached_strerror \
	memcached_verbosity \
	memcapable \
	memcat \
	memcp \
	memdump \
	memerror \
	memflush \
	memrm \
	memslap \
	memstat

BEHAVIOR_PAGES= \
	memcached_behavior_get \
	memcached_behavior_set

${BEHAVIOR_PAGES}:
	@rm -f $@.pop
	ln -s ${top_srcdir}/docs/memcached_behavior.pod ${top_builddir}/docs/$@.pop

CALLBACK_PAGES= \
	memcached_callback_get \
	memcached_callback_set

${CALLBACK_PAGES}:
	@rm -f $@.pop
	ln -s ${top_srcdir}/docs/memcached_callback.pod ${top_builddir}/docs/$@.pop

STATS_PAGES= \
	memcached_stat \
	memcached_stat_get_keys \
	memcached_stat_get_value \
	memcached_stat_servername

${STATS_PAGES}:
	@rm -f $@.pop
	ln -s ${top_srcdir}/docs/memcached_stats.pod ${top_builddir}/docs/$@.pop

${RESULT_PAGES}:
	@rm -f $@.pop
	ln -s ${top_srcdir}/docs/memcached_result_st.pod ${top_builddir}/docs/$@.pop

VERSION_PAGES= \
	       memcached_version \
	       memcached_lib_version

${VERSION_PAGES}:
	@rm -f $@.pop
	ln -s ${top_srcdir}/docs/memcached_version.pod ${top_builddir}/docs/$@.pop

MEMORY_ALLOCATORS_PAGES= \
			 memcached_get_memory_allocators \
			 memcached_set_memory_allocators

${MEMORY_ALLOCATORS_PAGES}:
	@rm -f $@.pop
	ln -s ${top_srcdir}/docs/memcached_memory_allocators.pod ${top_builddir}/docs/$@.pop

USER_DATA_PAGES= \
		 memcached_get_user_data \
		 memcached_set_user_data

${USER_DATA_PAGES}:
	@rm -f $@.pop
	ln -s ${top_srcdir}/docs/memcached_user_data.pod ${top_builddir}/docs/$@.pop


POOL_PAGES= \
	memcached_pool_behavior_get \
	memcached_pool_behavior_set \
	memcached_pool_create \
	memcached_pool_destroy \
	memcached_pool_pop \
	memcached_pool_push 

${POOL_PAGES}:
	@rm -f $@.pop
	ln -s ${top_srcdir}/docs/memcached_pool.pod ${top_builddir}/docs/$@.pop

HASH_CREATE_PAGES= \
	     hashkit_is_allocated \
	     hashkit_create \
	     hashkit_clone \
	     hashkit_free

${HASH_CREATE_PAGES}:
	@rm -f $@.pop
	ln -s ${top_srcdir}/docs/hashkit_create.pod ${top_builddir}/docs/$@.pop

KIT_FUNCTIONS_PAGES= \
		     hashkit_crc32 \
		     hashkit_fnv1_32 \
		     hashkit_fnv1_64 \
		     hashkit_fnv1a_32 \
		     hashkit_fnv1a_64 \
		     hashkit_functions \
		     hashkit_hsieh \
		     hashkit_jenkins \
		     hashkit_md5 \
		     hashkit_murmur

${KIT_FUNCTIONS_PAGES}:
	@rm -f $@.pop
	ln -s ${top_srcdir}/docs/hashkit_functions.pod ${top_builddir}/docs/$@.pop

HASHKIT_ST_PAGES= \
	hashkit_value

${HASH_KIT_ST_PAGES}:
	@rm -f $@.pop
	ln -s ${top_srcdir}/docs/hashkit_value.pod ${top_builddir}/docs/$@.pop

test-docs:
	${PODCHECKER} $(top_srcdir)/docs/hashkit_create.pod
	${PODCHECKER} $(top_srcdir)/docs/hashkit_functions.pod
	${PODCHECKER} $(top_srcdir)/docs/hashkit_value.pod
	${PODCHECKER} $(top_srcdir)/docs/libmemcached.pod
	${PODCHECKER} $(top_srcdir)/docs/libmemcached_examples.pod
	${PODCHECKER} $(top_srcdir)/docs/memcached_auto.pod
	${PODCHECKER} $(top_srcdir)/docs/memcached_behavior.pod
	${PODCHECKER} $(top_srcdir)/docs/memcached_callback.pod
	${PODCHECKER} $(top_srcdir)/docs/memcached_create.pod
	${PODCHECKER} $(top_srcdir)/docs/memcached_delete.pod
	${PODCHECKER} $(top_srcdir)/docs/memcached_flush.pod
	${PODCHECKER} $(top_srcdir)/docs/memcached_flush_buffers.pod
	${PODCHECKER} $(top_srcdir)/docs/memcached_get.pod
	${PODCHECKER} $(top_srcdir)/docs/memcached_memory_allocators.pod
	${PODCHECKER} $(top_srcdir)/docs/memcached_pool.pod
	${PODCHECKER} $(top_srcdir)/docs/memcached_quit.pod
	${PODCHECKER} $(top_srcdir)/docs/memcached_server_st.pod
	${PODCHECKER} $(top_srcdir)/docs/memcached_servers.pod
	${PODCHECKER} $(top_srcdir)/docs/memcached_set.pod
	${PODCHECKER} $(top_srcdir)/docs/memcached_stats.pod
	${PODCHECKER} $(top_srcdir)/docs/memcached_strerror.pod
	${PODCHECKER} $(top_srcdir)/docs/memcached_user_data.pod
	${PODCHECKER} $(top_srcdir)/docs/memcached_verbosity.pod
	${PODCHECKER} $(top_srcdir)/docs/memcached_version.pod
	${PODCHECKER} $(top_srcdir)/docs/memcapable.pod
	${PODCHECKER} $(top_srcdir)/docs/memcat.pod
	${PODCHECKER} $(top_srcdir)/docs/memcp.pod
	${PODCHECKER} $(top_srcdir)/docs/memerror.pod
	${PODCHECKER} $(top_srcdir)/docs/memflush.pod
	${PODCHECKER} $(top_srcdir)/docs/memrm.pod
	${PODCHECKER} $(top_srcdir)/docs/memslap.pod
	${PODCHECKER} $(top_srcdir)/docs/memstat.pod

html-pages: $(HTML_FILES)

SUFFIXES: .pop .pod .html .1 .3

.pop: ${_set}

.pop.html:
	pod2html --infile=$< >  $@

.pop.1:
	${POD2MAN} -c "$*" -r "" -s 3 $< >  $@

.pop.3: ${_set}
	${POD2MAN} -c "$*" -r "" -s 3 $< >  $@
