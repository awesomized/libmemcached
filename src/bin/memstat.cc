/*
    +--------------------------------------------------------------------+
    | libmemcached-awesome - C/C++ Client Library for memcached          |
    +--------------------------------------------------------------------+
    | Redistribution and use in source and binary forms, with or without |
    | modification, are permitted under the terms of the BSD license.    |
    | You should have received a copy of the license in a bundled file   |
    | named LICENSE; in case you did not receive a copy you can review   |
    | the terms online at: https://opensource.org/licenses/BSD-3-Clause  |
    +--------------------------------------------------------------------+
    | Copyright (c) 2006-2014 Brian Aker   https://datadifferential.com/ |
    | Copyright (c) 2020-2021 Michael Wallner        https://awesome.co/ |
    +--------------------------------------------------------------------+
*/

#include "mem_config.h"

#define PROGRAM_NAME        "memstat"
#define PROGRAM_DESCRIPTION "Print stats/version of or analyze a memcached cluster."
#define PROGRAM_VERSION     "1.1"

#define DEFAULT_LATENCY_ITERATIONS 100 // update help string, if changed

#include "common/options.hpp"
#include "common/checks.hpp"
#include "common/time.hpp"

#include <cstdio>
#include <iomanip>

static memcached_return_t print_server_version(const memcached_st *,
                                               const memcached_instance_st *instance, void *) {
  std::cerr << memcached_server_name(instance) << ":" << memcached_server_port(instance) << " "
            << int(memcached_server_major_version(instance)) << "."
            << int(memcached_server_minor_version(instance)) << "."
            << int(memcached_server_micro_version(instance)) << std::endl;

  return MEMCACHED_SUCCESS;
}

static void print_report(memcached_st *memc, memcached_analysis_st *report) {
  uint32_t server_count = memcached_server_count(memc);
  auto most_consumed_server = memcached_server_instance_by_position(memc, report->most_consumed_server);
  auto least_free_server = memcached_server_instance_by_position(memc, report->least_free_server);
  auto oldest_server = memcached_server_instance_by_position(memc, report->oldest_server);

  printf("Memcached Cluster Analysis Report\n\n");
  printf("\tNumber of Servers Analyzed         : %u\n", server_count);
  printf("\tAverage Item Size (incl/overhead)  : %u bytes\n", report->average_item_size);

  if (server_count == 1) {
    printf("\nFor a detailed report, you must supply multiple servers.\n");
    return;
  }

  printf("\n");
  printf("\tNode with most memory consumption  : %s:%u (%llu bytes)\n",
      memcached_server_name(most_consumed_server),
      (uint32_t) memcached_server_port(most_consumed_server),
      (unsigned long long) report->most_used_bytes);
  printf("\tNode with least free space         : %s:%u (%llu bytes remaining)\n",
      memcached_server_name(least_free_server),
      (uint32_t) memcached_server_port(least_free_server),
      (unsigned long long) report->least_remaining_bytes);
  printf("\tNode with longest uptime           : %s:%u (%us)\n",
      memcached_server_name(oldest_server), (uint32_t) memcached_server_port(oldest_server),
      report->longest_uptime);
  printf("\tPool-wide Hit Ratio                : %1.f%%\n", report->pool_hit_ratio);
  printf("\n");
}

static bool analyze_stat(const client_options &opt, memcached_st *memc, memcached_stat_st *stat) {
  memcached_return_t rc;
  auto report = memcached_analyze(memc, stat, &rc);

  if (rc != MEMCACHED_SUCCESS || !report) {
    if (!opt.isset("quiet")) {
      std::cerr << "Failure to analyze servers:" << memcached_strerror(memc, rc) << ".\n";
    }
    return false;
  }
  print_report(memc, report);
  free(report);
  return true;
}

static void latency_test(uint32_t iterations, std::vector<memcached_st> &servers) {
  const char *test_key = "libmemcached_test_key";
  size_t test_key_len = strlen(test_key);
  const memcached_instance_st *slowest_server = nullptr;
  time_point::duration slowest_time{};
  std::vector<const memcached_instance_st *> failed_servers{};

  std::cout << "Network Latency Test:\n\n" << std::showpoint << std::fixed << std::setprecision(3);

  for (auto &memc : servers) {
    memcached_return_t rc = memcached_last_error(&memc);

    auto start = time_clock::now();
    for (auto i = 0u; i < iterations; ++i) {
      free(memcached_get(&memc, test_key, test_key_len, nullptr, nullptr, &rc));
      if (memcached_fatal(rc)) {
        break;
      }
    }
    auto elapsed = time_clock::now() - start;

    auto inst = memcached_server_instance_by_position(&memc, 0);
    std::cout << "\t " << memcached_server_name(inst)
              << " (" << memcached_server_port(inst) << ") ";

    if (memcached_fatal(rc)) {
      std::cout << "  => failed to reach the server\n";
      failed_servers.push_back(inst);
    } else {
      std::cout << "  => "
                << time_format(elapsed/iterations).count() << " seconds ("
                << time_format_ms(elapsed/iterations).count() << "ms)\n";
      if (slowest_time == time_point::duration::zero() || slowest_time < elapsed) {
        slowest_time = elapsed;
        slowest_server = inst;
      }
    }
  }

  if (servers.size() > 1 && slowest_server) {
    std::cout << "\n---\n\nSlowest Server: "
              << memcached_server_name(slowest_server) << "("
              << memcached_server_port(slowest_server) << ")"
              << " => "
              << time_format(slowest_time/iterations).count() << " seconds ("
              << time_format_ms(slowest_time/iterations).count() << "ms)\n";
  }
  if (!failed_servers.empty()) {
    for (const auto inst : failed_servers) {
      std::cout << "Failed Server:  " << memcached_server_name(inst)
                << " (" << memcached_server_port(inst)
                << ") => " << memcached_strerror(inst->root, memcached_server_error_return(inst))
                << "\n";
    }
  }
}

static bool analyze_latency(client_options &opt, memcached_st *root) {
  uint32_t num_of_tests = DEFAULT_LATENCY_ITERATIONS;

  if (auto iter_str = opt.argof("iterations")) {
    num_of_tests = std::stoul(iter_str);
  }

  std::vector<memcached_st> servers{memcached_server_count(root)};

  uint32_t i = 0;
  for (auto &memc : servers) {
    memcached_clone(&memc, root);
    memcached_servers_reset(&memc);
    auto instance = memcached_server_instance_by_position(root, i++);
    memcached_server_add(&memc, memcached_server_name(instance), memcached_server_port(instance));
    //pre-connect
    memcached_version(&memc);
  }

  latency_test(num_of_tests, servers);

  for (auto &memc : servers) {
    memcached_free(&memc);
  }

  return true;
}

static memcached_return_t print_stat(const memcached_instance_st *server,
                                     const char *key, size_t key_length,
                                     const char *value, size_t value_length, void *context) {
  auto instance = static_cast<const memcached_instance_st **>(context);

  if (*instance != server) {
    *instance = server;

    std::cout << "Server: " << memcached_server_name(server)
              << " (" << memcached_server_port(server) << ")\n";
  }

  std::cout << "\t";
  std::cout.write(key, key_length) << ": ";
  std::cout.write(value, value_length) << "\n";

  return MEMCACHED_SUCCESS;
}

static bool memstat(const client_options &opt, memcached_st &memc, const char *arg) {
  memcached_instance_st *context = nullptr;
  auto rc = memcached_stat_execute(&memc, arg, print_stat, &context);
  if (memcached_success(rc)) {
    return true;
  }
  if (!opt.isset("quiet")) {
    std::cerr << "Failed to 'STAT " << (arg ? arg : "") << "': ";
    if (memcached_last_error(&memc)) {
      std::cerr << memcached_last_error_message(&memc) << "\n";
    } else {
      std::cerr << memcached_strerror(&memc, rc) << "\n";
    }
  }
  return false;
}

int main(int argc, char *argv[]) {
  client_options opt{PROGRAM_NAME, PROGRAM_VERSION, PROGRAM_DESCRIPTION, "[stat ...]"};

  for (const auto &def : opt.defaults) {
    if (def.opt.val != 'H') {
      // no need for --hash
      opt.add(def);
    }
  }

  opt.add("args", 'A', required_argument, "Stat args. DEPRECATED: use positional arguments.");
  opt.add("server-version", 'S', no_argument, "Print server version.");
  opt.add("analyze", 'a', optional_argument, "Analyze server characteristics (options: default, latency).");
  opt.add("iterations", 0, required_argument, "Iteration count of GETs sent by the latency test (default: 1000).");

  char **argp = nullptr;
  if (!opt.parse(argc, argv, &argp)) {
    exit(EXIT_FAILURE);
  }

  memcached_st memc;
  if (!check_memcached(opt, memc)) {
    exit(EXIT_FAILURE);
  }

  if (!opt.apply(&memc)) {
    memcached_free(&memc);
    exit(EXIT_FAILURE);
  }

  auto exit_code = EXIT_SUCCESS;
  if (opt.isset('S')) {
    if (opt.isset("verbose")) {
      std::cout << "Server versions:\n";
    }
    if (MEMCACHED_SUCCESS != memcached_version(&memc)) {
      exit_code = EXIT_FAILURE;
    }
    memcached_server_fn cb[] = {&print_server_version};
    memcached_server_cursor(&memc, cb, nullptr, 1);
    goto done;
  }

  if (opt.isset("analyze")) {
    const char *analyze = opt.argof("analyze");
    if (analyze && strcmp(analyze, "default")) {
      if (!strcmp(analyze, "latency")) {
        if (!analyze_latency(opt, &memc)) {
          exit_code = EXIT_FAILURE;
        }
        goto done;
      }

      if (!opt.isset("quiet")) {
        std::cerr << "Unknown --analyze mode: '" << analyze << "'.\n";
      }
    }

    memcached_return_t rc;
    auto stat = memcached_stat(&memc, nullptr, &rc);
    if (!memcached_success(rc)) {
      exit_code = EXIT_FAILURE;
      if (!opt.isset("quiet")) {
        std::cerr << memcached_last_error_message(&memc) << "\n";
      }
    } else if (!analyze_stat(opt, &memc, stat)) {
      exit_code = EXIT_FAILURE;
    }
    memcached_stat_free(&memc, stat);
    goto done;
  }

  if (!*argp || opt.isset('A')) {
    if (!memstat(opt, memc, opt.argof('A'))) {
      exit_code = EXIT_FAILURE;
    }
  }
  for (auto arg = argp; *arg; ++arg) {
    if (!memstat(opt, memc, *arg)) {
      exit_code = EXIT_FAILURE;
    }
  }

done:
  memcached_free(&memc);
  exit(exit_code);
}
