/*
    +--------------------------------------------------------------------+
    | libmemcached - C/C++ Client Library for memcached                  |
    +--------------------------------------------------------------------+
    | Redistribution and use in source and binary forms, with or without |
    | modification, are permitted under the terms of the BSD license.    |
    | You should have received a copy of the license in a bundled file   |
    | named LICENSE; in case you did not receive a copy you can review   |
    | the terms online at: https://opensource.org/licenses/BSD-3-Clause  |
    +--------------------------------------------------------------------+
    | Copyright (c) 2006-2014 Brian Aker   https://datadifferential.com/ |
    | Copyright (c) 2020 Michael Wallner   <mike@php.net>                |
    +--------------------------------------------------------------------+
*/

#pragma once

#include <cstring>
#include <functional>
#include <utility>

using namespace std;

struct c_string {
  const char *c_str;
  size_t size;

  c_string(const char *s, size_t l) noexcept
  : c_str{s}
  , size{l} {}
  bool operator==(const c_string &cs) const noexcept { return !strcmp(c_str, cs.c_str); }
  bool operator==(const char *p) const noexcept { return !strcmp(c_str, p); }
};

static c_string null_c_string{nullptr, 0};

struct test_case {
  using setting_key_t = variant<memcached_flag_t, memcached_behavior_t>;
  using setting_t = pair<setting_key_t, uint64_t>;
  using result_t = variant<c_string, setting_t>;
  c_string option;
  result_t result;

  test_case(c_string o, c_string r) noexcept
  : option{o}
  , result{r} {}
  test_case(const char *os, size_t ol, const char *rs, size_t rl) noexcept
  : option{os, ol}
  , result{c_string{rs, rl}} {}
  test_case(const char *os, size_t ol, memcached_behavior_t b, uint64_t v) noexcept
  : option{os, ol}
  , result{setting_t{b, v}} {}
  test_case(const char *os, size_t ol, memcached_flag_t f, uint64_t v) noexcept
  : option{os, ol}
  , result{setting_t{f, v}} {}
};

#define test_count(tca) (sizeof(tca) / sizeof(tca[0]))

struct test_group {
  using check_func = function<void(memcached_st *, const test_case::result_t &)>;
  const char *name;
  check_func check;
  test_case *tests;
  size_t ntests;

  test_group(const char *name_, check_func check_, test_case *tests_, size_t ntests_) noexcept
  : name{name_}
  , check{move(check_)}
  , tests{tests_}
  , ntests{ntests_} {}
};

static test_case host_string_tests[] = {
    {S("--server=localhost"), S("localhost")},
    {S("--server=10.0.2.1"), S("10.0.2.1")},
    {S("--server=example.com"), S("example.com")},
    {S("--server=localhost:30"), S("localhost")},
    {S("--server=10.0.2.1:20"), S("10.0.2.1")},
    {S("--server=example.com:1024"), S("example.com")},
    {S("--server=10.0.2.1:30/?40"), S("10.0.2.1")},
    {S("--server=example.com:1024/?30"), S("example.com")},
    {S("--server=10.0.2.1/?20"), S("10.0.2.1")},
    {S("--server=example.com/?10"), S("example.com")},
};

static test_group test_host_strings{
    "host strings",
    [](memcached_st *memc, const test_case::result_t &result) {
      auto instance = memcached_server_instance_by_position(memc, 0);
      REQUIRE(instance);
      REQUIRE(get<c_string>(result) == memcached_server_name(instance));
    },
    host_string_tests, test_count(host_string_tests)};

static test_case bad_host_string_tests[] = {
    {{S("-servers=localhost:11221,localhost:11222,localhost:11223,localhost:11224,localhost:"
        "11225")},
     null_c_string},
    {{S("-- servers=a.example.com:81,localhost:82,b.example.com")}, null_c_string},
    {{S("--servers=localhost:+80")}, null_c_string},
    // all of the following should not fail IMO
    {{S("--servers=localhost.com.")}, null_c_string},
    {{S("--server=localhost.com.")}, null_c_string},
    {{S("--server=localhost.com.:80")}, null_c_string},
};

static test_group test_bad_host_strings{
    "bad host strings",
    [](memcached_st *memc, const test_case::result_t &) { REQUIRE_FALSE(memc); },
    bad_host_string_tests, test_count(bad_host_string_tests)};

static test_case behavior_tests[] = {
    {S("--CONNECT-TIMEOUT=456"), MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT, 456},
    {S("--IO-BYTES-WATERMARK=456"), MEMCACHED_BEHAVIOR_IO_BYTES_WATERMARK, 456},
    {S("--IO-KEY-PREFETCH=456"), MEMCACHED_BEHAVIOR_IO_KEY_PREFETCH, 456},
    {S("--IO-MSG-WATERMARK=456"), MEMCACHED_BEHAVIOR_IO_MSG_WATERMARK, 456},
    {S("--NUMBER-OF-REPLICAS=456"), MEMCACHED_BEHAVIOR_NUMBER_OF_REPLICAS, 456},
    {S("--POLL-TIMEOUT=456"), MEMCACHED_BEHAVIOR_POLL_TIMEOUT, 456},
    {S("--RCV-TIMEOUT=456"), MEMCACHED_BEHAVIOR_RCV_TIMEOUT, 456},
    {S("--REMOVE-FAILED-SERVERS=3"), MEMCACHED_BEHAVIOR_REMOVE_FAILED_SERVERS, 1},
    {S("--RETRY-TIMEOUT=456"), MEMCACHED_BEHAVIOR_RETRY_TIMEOUT, 456},
    {S("--SND-TIMEOUT=456"), MEMCACHED_BEHAVIOR_SND_TIMEOUT, 456},
    {S("--SOCKET-RECV-SIZE=456"), MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE, 456},
    {S("--SOCKET-SEND-SIZE=456"), MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE, 456},
    {S("--BINARY-PROTOCOL"), MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, 1},
    {S("--BUFFER-REQUESTS"), MEMCACHED_BEHAVIOR_BUFFER_REQUESTS, 1},
    {S("--NOREPLY"), MEMCACHED_BEHAVIOR_NOREPLY, 1},
    {S("--RANDOMIZE-REPLICA-READ"), MEMCACHED_BEHAVIOR_RANDOMIZE_REPLICA_READ, 1},
    {S("--SORT-HOSTS"), MEMCACHED_BEHAVIOR_SORT_HOSTS, 1},
    {S("--SUPPORT-CAS"), MEMCACHED_BEHAVIOR_SUPPORT_CAS, 1},
    {S("--TCP-NODELAY"), MEMCACHED_BEHAVIOR_TCP_NODELAY, 1},
    {S("--TCP-KEEPALIVE"), MEMCACHED_BEHAVIOR_TCP_KEEPALIVE, 1},
    {S("--TCP-KEEPIDLE"), MEMCACHED_BEHAVIOR_TCP_KEEPIDLE, 1},
    {S("--USE-UDP"), MEMCACHED_BEHAVIOR_USE_UDP, 1},
    {S("--VERIFY-KEY"), MEMCACHED_BEHAVIOR_VERIFY_KEY, 1},
    {S("--DISTRIBUTION=consistent"), MEMCACHED_BEHAVIOR_DISTRIBUTION,
     MEMCACHED_DISTRIBUTION_CONSISTENT},
    {S("--DISTRIBUTION=consistent,CRC"), MEMCACHED_BEHAVIOR_DISTRIBUTION,
     MEMCACHED_DISTRIBUTION_CONSISTENT},
    {S("--DISTRIBUTION=consistent,MD5"), MEMCACHED_BEHAVIOR_DISTRIBUTION,
     MEMCACHED_DISTRIBUTION_CONSISTENT},
    {S("--DISTRIBUTION=consistent,JENKINS"), MEMCACHED_BEHAVIOR_KETAMA_HASH,
     MEMCACHED_HASH_JENKINS},
    {S("--DISTRIBUTION=random"), MEMCACHED_BEHAVIOR_DISTRIBUTION, MEMCACHED_DISTRIBUTION_RANDOM},
    {S("--DISTRIBUTION=modula"), MEMCACHED_BEHAVIOR_DISTRIBUTION, MEMCACHED_DISTRIBUTION_MODULA},
    {S("--HASH=CRC"), MEMCACHED_BEHAVIOR_HASH, MEMCACHED_HASH_CRC},
    {S("--HASH=FNV1A_32"), MEMCACHED_BEHAVIOR_HASH, MEMCACHED_HASH_FNV1A_32},
    {S("--HASH=FNV1_32"), MEMCACHED_BEHAVIOR_HASH, MEMCACHED_HASH_FNV1_32},
    {S("--HASH=JENKINS"), MEMCACHED_BEHAVIOR_HASH, MEMCACHED_HASH_JENKINS},
    {S("--HASH=MD5"), MEMCACHED_BEHAVIOR_HASH, MEMCACHED_HASH_MD5},

};

static test_group test_behaviors{
    "behaviors",
    [](memcached_st *memc, const test_case::result_t &result) {
      auto setting = get<test_case::setting_t>(result);
      REQUIRE(memc);
      REQUIRE(setting.second
              == memcached_behavior_get(memc, get<memcached_behavior_t>(setting.first)));
    },
    behavior_tests, test_count(behavior_tests)};

static test_case flag_tests[] = {
    {S("--FETCH-VERSION"), MEMCACHED_FLAG_IS_FETCHING_VERSION, 1},
    {S("--HASH-WITH-NAMESPACE"), MEMCACHED_FLAG_HASH_WITH_NAMESPACE, 1},
};

static test_group test_flags{
    "flags",
    [](memcached_st *memc, const test_case::result_t &result) {
      auto setting = get<test_case::setting_t>(result);
      REQUIRE(memc);
      REQUIRE(setting.second == memcached_flag(*memc, get<memcached_flag_t>(setting.first)));
    },
    flag_tests, test_count(flag_tests)};

static test_case namespace_tests[] = {
    {S("--NAMESPACE=foo"), S("foo")},
    {S("--NAMESPACE=\"foo\""), S("foo")},
    {S("--NAMESPACE=\"The quick brown fox jumps over the lazy dog\""),
     S("The quick brown fox jumps over the lazy dog")},
};

static test_group test_namespace{"namespace",
                                 [](memcached_st *memc, const test_case::result_t &result) {
                                   auto ns = get<c_string>(result);
                                   REQUIRE(memc);
                                   REQUIRE(ns == memcached_get_namespace(*memc));
                                 },
                                 namespace_tests, test_count(namespace_tests)};

static test_group tests[] = {
    test_host_strings,
    test_bad_host_strings,
    test_behaviors,
    test_flags,
};
