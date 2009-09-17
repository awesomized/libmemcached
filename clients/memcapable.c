/* -*- Mode: C; tab-width: 2; c-basic-offset: 2; indent-tabs-mode: nil -*- */
#undef NDEBUG
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include <unistd.h>
#include <poll.h>

#include <libmemcached/memcached/protocol_binary.h>
#include <libmemcached/byteorder.h>

#ifdef linux
/* /usr/include/netinet/in.h defines macros from ntohs() to _bswap_nn to
 * optimize the conversion functions, but the prototypes generate warnings
 * from gcc. The conversion methods isn't the bottleneck for my app, so
 * just remove the warnings by undef'ing the optimization ..
 */
#undef ntohs
#undef ntohl
#endif

/* Should we generate coredumps when we enounter an error (-c) */
static bool do_core= false;
/* connection to the server */
static int sock;
/* Should the output from test failures be verbose or quiet? */
static bool verbose= false;

/* The number of seconds to wait for an IO-operation */
static int timeout= 2;

/*
 * Instead of having to cast between the different datatypes we create
 * a union of all of the different types of pacages we want to send.
 * A lot of the different commands use the same packet layout, so I'll
 * just define the different types I need. The typedefs only contain
 * the header of the message, so we need some space for keys and body
 * To avoid to have to do multiple writes, lets add a chunk of memory
 * to use. 1k should be more than enough for header, key and body.
 */
typedef union
{
  protocol_binary_request_no_extras plain;
  protocol_binary_request_flush flush;
  protocol_binary_request_incr incr;
  protocol_binary_request_set set;
  char bytes[1024];
} command;

typedef union
{
  protocol_binary_response_no_extras plain;
  protocol_binary_response_incr incr;
  protocol_binary_response_decr decr;
  char bytes[1024];
} response;

enum test_return
{
  TEST_SKIP, TEST_PASS, TEST_PASS_RECONNECT, TEST_FAIL
};

/**
 * Try to get an addrinfo struct for a given port on a given host
 */
static struct addrinfo *lookuphost(const char *hostname, const char *port)
{
  struct addrinfo *ai= 0;
  struct addrinfo hints= {.ai_family=AF_UNSPEC,
    .ai_protocol=IPPROTO_TCP,
    .ai_socktype=SOCK_STREAM};
  int error= getaddrinfo(hostname, port, &hints, &ai);

  if (error != 0)
  {
    if (error != EAI_SYSTEM)
      fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(error));
    else
      perror("getaddrinfo()");
  }

  return ai;
}

/**
 * Set the socket in nonblocking mode
 * @return -1 if failure, the socket otherwise
 */
static int set_noblock(void)
{
  int flags= fcntl(sock, F_GETFL, 0);
  if (flags == -1)
  {
    perror("Failed to get socket flags");
    close(sock);
    return -1;
  }

  if ((flags & O_NONBLOCK) != O_NONBLOCK)
  {
    if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
    {
      perror("Failed to set socket to nonblocking mode");
      close(sock);
      return -1;
    }
  }

  return sock;
}

/**
 * Try to open a connection to the server
 * @param hostname the name of the server to connect to
 * @param port the port number (or service) to connect to
 * @return positive integer if success, -1 otherwise
 */
static int connect_server(const char *hostname, const char *port)
{
  struct addrinfo *ai= lookuphost(hostname, port);
  sock= -1;
  if (ai != NULL)
  {
    if ((sock=socket(ai->ai_family, ai->ai_socktype,
                     ai->ai_protocol)) != -1)
    {
      if (connect(sock, ai->ai_addr, ai->ai_addrlen) == -1)
      {
        fprintf(stderr, "Failed to connect socket: %s\n",
                strerror(errno));
        close(sock);
        sock= -1;
      }
      else
      {
        sock= set_noblock();
      }
    } else
      fprintf(stderr, "Failed to create socket: %s\n", strerror(errno));

    freeaddrinfo(ai);
  }

  return sock;
}

static ssize_t timeout_io_op(int fd, short direction, void *buf, size_t len)
{
  ssize_t ret;

  if (direction == POLLOUT)
    ret= write(fd, buf, len);
  else
    ret= read(fd, buf, len);

  if (ret == -1 && errno == EWOULDBLOCK) {
    struct pollfd fds = {
      .events= direction,
      .fd= fd
    };
    int err= poll(&fds, 1, timeout * 1000);

    if (err == 1)
    {
      if (direction == POLLOUT)
        ret= write(fd, buf, len);
      else
        ret= read(fd, buf, len);
    }
    else if (err == 0)
    {
      errno = ETIMEDOUT;
    }
    else
    {
      perror("Failed to poll");
      return -1;
    }
  }

  return ret;
}

/**
 * Ensure that an expression is true. If it isn't print out a message similar
 * to assert() and create a coredump if the user wants that. If not an error
 * message is returned.
 *
 */
static enum test_return ensure(bool val, const char *expression, const char *file, int line)
{
  if (!val)
  {
    if (verbose)
      fprintf(stderr, "%s:%u: %s\n", file, line, expression);

    if (do_core)
      abort();

    return TEST_FAIL;
  }

  return TEST_PASS;
}

#define verify(expression) do { if (ensure(expression, #expression, __FILE__, __LINE__) == TEST_FAIL) return TEST_FAIL; } while (0)
#define execute(expression) do { if (ensure(expression == TEST_PASS, #expression, __FILE__, __LINE__) == TEST_FAIL) return TEST_FAIL; } while (0)

/**
 * Send a chunk of memory over the socket (retry if the call is iterrupted
 */
static enum test_return retry_write(const void* buf, size_t len)
{
  size_t offset= 0;
  const char* ptr= buf;

  do
  {
    size_t num_bytes= len - offset;
    ssize_t nw= timeout_io_op(sock, POLLOUT, (void*)(ptr + offset), num_bytes);
    if (nw == -1)
      verify(errno == EINTR || errno == EAGAIN);
    else
      offset+= (size_t)nw;
  } while (offset < len);

  return TEST_PASS;
}

/**
 * Resend a packet to the server (All fields in the command header should
 * be in network byte order)
 */
static enum test_return resend_packet(command *cmd)
{
  size_t length= sizeof (protocol_binary_request_no_extras) +
          ntohl(cmd->plain.message.header.request.bodylen);

  execute(retry_write(cmd, length));
  return TEST_PASS;
}

/**
 * Send a command to the server. The command header needs to be updated
 * to network byte order
 */
static enum test_return send_packet(command *cmd)
{
  /* Fix the byteorder of the header */
  cmd->plain.message.header.request.keylen=
          ntohs(cmd->plain.message.header.request.keylen);
  cmd->plain.message.header.request.bodylen=
          ntohl(cmd->plain.message.header.request.bodylen);
  cmd->plain.message.header.request.cas=
          ntohll(cmd->plain.message.header.request.cas);

  execute(resend_packet(cmd));
  return TEST_PASS;
}

/**
 * Read a fixed length chunk of data from the server
 */
static enum test_return retry_read(void *buf, size_t len)
{
  size_t offset= 0;
  do
  {
    ssize_t nr= timeout_io_op(sock, POLLIN, ((char*) buf) + offset, len - offset);
    switch (nr) {
    case -1 :
      verify(errno == EINTR || errno == EAGAIN);
      break;
    case 0:
      return TEST_FAIL;
    default:
      offset+= (size_t)nr;
    }
  } while (offset < len);

  return TEST_PASS;
}

/**
 * Receive a response from the server and conver the fields in the header
 * to local byte order
 */
static enum test_return recv_packet(response *rsp)
{
  execute(retry_read(rsp, sizeof (protocol_binary_response_no_extras)));

  /* Fix the byte order in the packet header */
  rsp->plain.message.header.response.keylen=
          ntohs(rsp->plain.message.header.response.keylen);
  rsp->plain.message.header.response.status=
          ntohs(rsp->plain.message.header.response.status);
  rsp->plain.message.header.response.bodylen=
          ntohl(rsp->plain.message.header.response.bodylen);
  rsp->plain.message.header.response.cas=
          ntohll(rsp->plain.message.header.response.cas);

  size_t bodysz= rsp->plain.message.header.response.bodylen;
  if (bodysz > 0)
    execute(retry_read(rsp->bytes + sizeof (protocol_binary_response_no_extras), bodysz));

  return TEST_PASS;
}

/**
 * Create a storage command (add, set, replace etc)
 *
 * @param cmd destination buffer
 * @param cc the storage command to create
 * @param key the key to store
 * @param keylen the length of the key
 * @param dta the data to store with the key
 * @param dtalen the length of the data to store with the key
 * @param flags the flags to store along with the key
 * @param exp the expiry time for the key
 */
static void storage_command(command *cmd,
                            uint8_t cc,
                            const void* key,
                            size_t keylen,
                            const void* dta,
                            size_t dtalen,
                            uint32_t flags,
                            uint32_t exp)
{
  /* all of the storage commands use the same command layout */
  protocol_binary_request_set *request= &cmd->set;

  memset(request, 0, sizeof (*request));
  request->message.header.request.magic= PROTOCOL_BINARY_REQ;
  request->message.header.request.opcode= cc;
  request->message.header.request.keylen= (uint16_t)keylen;
  request->message.header.request.extlen= 8;
  request->message.header.request.bodylen= (uint32_t)(keylen + 8 + dtalen);
  request->message.header.request.opaque= 0xdeadbeef;
  request->message.body.flags= flags;
  request->message.body.expiration= exp;

  off_t key_offset= sizeof (protocol_binary_request_no_extras) + 8;
  memcpy(cmd->bytes + key_offset, key, keylen);
  if (dta != NULL)
    memcpy(cmd->bytes + key_offset + keylen, dta, dtalen);
}

/**
 * Create a basic command to send to the server
 * @param cmd destination buffer
 * @param cc the command to create
 * @param key the key to store
 * @param keylen the length of the key
 * @param dta the data to store with the key
 * @param dtalen the length of the data to store with the key
 */
static void raw_command(command *cmd,
                        uint8_t cc,
                        const void* key,
                        size_t keylen,
                        const void* dta,
                        size_t dtalen)
{
  /* all of the storage commands use the same command layout */
  memset(cmd, 0, sizeof (*cmd));
  cmd->plain.message.header.request.magic= PROTOCOL_BINARY_REQ;
  cmd->plain.message.header.request.opcode= cc;
  cmd->plain.message.header.request.keylen= (uint16_t)keylen;
  cmd->plain.message.header.request.bodylen= (uint32_t)(keylen + dtalen);
  cmd->plain.message.header.request.opaque= 0xdeadbeef;

  off_t key_offset= sizeof (protocol_binary_request_no_extras);

  if (key != NULL)
    memcpy(cmd->bytes + key_offset, key, keylen);

  if (dta != NULL)
    memcpy(cmd->bytes + key_offset + keylen, dta, dtalen);
}

/**
 * Create the flush command
 * @param cmd destination buffer
 * @param cc the command to create (FLUSH/FLUSHQ)
 * @param exptime when to flush
 * @param use_extra to force using of the extra field?
 */
static void flush_command(command *cmd,
                          uint8_t cc, uint32_t exptime, bool use_extra)
{
  memset(cmd, 0, sizeof (cmd->flush));
  cmd->flush.message.header.request.magic= PROTOCOL_BINARY_REQ;
  cmd->flush.message.header.request.opcode= cc;
  cmd->flush.message.header.request.opaque= 0xdeadbeef;

  if (exptime != 0 || use_extra)
  {
    cmd->flush.message.header.request.extlen= 4;
    cmd->flush.message.body.expiration= htonl(exptime);
    cmd->flush.message.header.request.bodylen= 4;
  }
}

/**
 * Create a incr/decr command
 * @param cc the cmd to create (FLUSH/FLUSHQ)
 * @param key the key to operate on
 * @param keylen the number of bytes in the key
 * @param delta the number to add/subtract
 * @param initial the initial value if the key doesn't exist
 * @param exp when the key should expire if it isn't set
 */
static void arithmetic_command(command *cmd,
                               uint8_t cc,
                               const void* key,
                               size_t keylen,
                               uint64_t delta,
                               uint64_t initial,
                               uint32_t exp)
{
  memset(cmd, 0, sizeof (cmd->incr));
  cmd->incr.message.header.request.magic= PROTOCOL_BINARY_REQ;
  cmd->incr.message.header.request.opcode= cc;
  cmd->incr.message.header.request.keylen= (uint16_t)keylen;
  cmd->incr.message.header.request.extlen= 20;
  cmd->incr.message.header.request.bodylen= (uint32_t)(keylen + 20);
  cmd->incr.message.header.request.opaque= 0xdeadbeef;
  cmd->incr.message.body.delta= htonll(delta);
  cmd->incr.message.body.initial= htonll(initial);
  cmd->incr.message.body.expiration= htonl(exp);

  off_t key_offset= sizeof (protocol_binary_request_no_extras) + 20;
  memcpy(cmd->bytes + key_offset, key, keylen);
}

/**
 * Validate the response header from the server
 * @param rsp the response to check
 * @param cc the expected command
 * @param status the expected status
 */
static enum test_return validate_response_header(response *rsp,
                                                 uint8_t cc, uint16_t status)
{
  verify(rsp->plain.message.header.response.magic == PROTOCOL_BINARY_RES);
  verify(rsp->plain.message.header.response.opcode == cc);
  verify(rsp->plain.message.header.response.datatype == PROTOCOL_BINARY_RAW_BYTES);
  verify(rsp->plain.message.header.response.status == status);
  verify(rsp->plain.message.header.response.opaque == 0xdeadbeef);

  if (status == PROTOCOL_BINARY_RESPONSE_SUCCESS)
  {
    switch (cc) {
    case PROTOCOL_BINARY_CMD_ADDQ:
    case PROTOCOL_BINARY_CMD_APPENDQ:
    case PROTOCOL_BINARY_CMD_DECREMENTQ:
    case PROTOCOL_BINARY_CMD_DELETEQ:
    case PROTOCOL_BINARY_CMD_FLUSHQ:
    case PROTOCOL_BINARY_CMD_INCREMENTQ:
    case PROTOCOL_BINARY_CMD_PREPENDQ:
    case PROTOCOL_BINARY_CMD_QUITQ:
    case PROTOCOL_BINARY_CMD_REPLACEQ:
    case PROTOCOL_BINARY_CMD_SETQ:
      verify("Quiet command shouldn't return on success" == NULL);
    default:
      break;
    }

    switch (cc) {
    case PROTOCOL_BINARY_CMD_ADD:
    case PROTOCOL_BINARY_CMD_REPLACE:
    case PROTOCOL_BINARY_CMD_SET:
    case PROTOCOL_BINARY_CMD_APPEND:
    case PROTOCOL_BINARY_CMD_PREPEND:
      verify(rsp->plain.message.header.response.keylen == 0);
      verify(rsp->plain.message.header.response.extlen == 0);
      verify(rsp->plain.message.header.response.bodylen == 0);
      verify(rsp->plain.message.header.response.cas != 0);
      break;
    case PROTOCOL_BINARY_CMD_FLUSH:
    case PROTOCOL_BINARY_CMD_NOOP:
    case PROTOCOL_BINARY_CMD_QUIT:
    case PROTOCOL_BINARY_CMD_DELETE:
      verify(rsp->plain.message.header.response.keylen == 0);
      verify(rsp->plain.message.header.response.extlen == 0);
      verify(rsp->plain.message.header.response.bodylen == 0);
      verify(rsp->plain.message.header.response.cas == 0);
      break;

    case PROTOCOL_BINARY_CMD_DECREMENT:
    case PROTOCOL_BINARY_CMD_INCREMENT:
      verify(rsp->plain.message.header.response.keylen == 0);
      verify(rsp->plain.message.header.response.extlen == 0);
      verify(rsp->plain.message.header.response.bodylen == 8);
      verify(rsp->plain.message.header.response.cas != 0);
      break;

    case PROTOCOL_BINARY_CMD_STAT:
      verify(rsp->plain.message.header.response.extlen == 0);
      /* key and value exists in all packets except in the terminating */
      verify(rsp->plain.message.header.response.cas == 0);
      break;

    case PROTOCOL_BINARY_CMD_VERSION:
      verify(rsp->plain.message.header.response.keylen == 0);
      verify(rsp->plain.message.header.response.extlen == 0);
      verify(rsp->plain.message.header.response.bodylen != 0);
      verify(rsp->plain.message.header.response.cas == 0);
      break;

    case PROTOCOL_BINARY_CMD_GET:
    case PROTOCOL_BINARY_CMD_GETQ:
      verify(rsp->plain.message.header.response.keylen == 0);
      verify(rsp->plain.message.header.response.extlen == 4);
      verify(rsp->plain.message.header.response.cas != 0);
      break;

    case PROTOCOL_BINARY_CMD_GETK:
    case PROTOCOL_BINARY_CMD_GETKQ:
      verify(rsp->plain.message.header.response.keylen != 0);
      verify(rsp->plain.message.header.response.extlen == 4);
      verify(rsp->plain.message.header.response.cas != 0);
      break;

    default:
      /* Undefined command code */
      break;
    }
  }
  else
  {
    verify(rsp->plain.message.header.response.cas == 0);
    verify(rsp->plain.message.header.response.extlen == 0);
    if (cc != PROTOCOL_BINARY_CMD_GETK)
    {
      verify(rsp->plain.message.header.response.keylen == 0);
    }
  }

  return TEST_PASS;
}

static enum test_return test_binary_noop(void)
{
  command cmd;
  response rsp;
  raw_command(&cmd, PROTOCOL_BINARY_CMD_NOOP, NULL, 0, NULL, 0);
  execute(send_packet(&cmd));
  execute(recv_packet(&rsp));
  verify(validate_response_header(&rsp, PROTOCOL_BINARY_CMD_NOOP,
                                  PROTOCOL_BINARY_RESPONSE_SUCCESS));
  return TEST_PASS;
}

static enum test_return test_binary_quit_impl(uint8_t cc)
{
  command cmd;
  response rsp;
  raw_command(&cmd, cc, NULL, 0, NULL, 0);

  execute(send_packet(&cmd));
  if (cc == PROTOCOL_BINARY_CMD_QUIT)
  {
    execute(recv_packet(&rsp));
    verify(validate_response_header(&rsp, PROTOCOL_BINARY_CMD_QUIT,
                                    PROTOCOL_BINARY_RESPONSE_SUCCESS));
  }

  /* Socket should be closed now, read should return 0 */
  verify(timeout_io_op(sock, POLLIN, rsp.bytes, sizeof(rsp.bytes)) == 0);

  return TEST_PASS_RECONNECT;
}

static enum test_return test_binary_quit(void)
{
  return test_binary_quit_impl(PROTOCOL_BINARY_CMD_QUIT);
}

static enum test_return test_binary_quitq(void)
{
  return test_binary_quit_impl(PROTOCOL_BINARY_CMD_QUITQ);
}

static enum test_return test_binary_set_impl(const char* key, uint8_t cc)
{
  command cmd;
  response rsp;

  uint64_t value= 0xdeadbeefdeadcafe;
  storage_command(&cmd, cc, key, strlen(key), &value, sizeof (value), 0, 0);

  /* set should always work */
  for (int ii= 0; ii < 10; ii++)
  {
    if (ii == 0)
      execute(send_packet(&cmd));
    else
      execute(resend_packet(&cmd));

    if (cc == PROTOCOL_BINARY_CMD_SET)
    {
      execute(recv_packet(&rsp));
      verify(validate_response_header(&rsp, cc, PROTOCOL_BINARY_RESPONSE_SUCCESS));
    }
    else
      execute(test_binary_noop());
  }

  /* try to set with the correct CAS value */
  cmd.plain.message.header.request.cas=
          htonll(rsp.plain.message.header.response.cas);
  execute(resend_packet(&cmd));
  if (cc == PROTOCOL_BINARY_CMD_SET)
  {
    execute(recv_packet(&rsp));
    verify(validate_response_header(&rsp, cc, PROTOCOL_BINARY_RESPONSE_SUCCESS));
  }
  else
    execute(test_binary_noop());

  /* try to set with an incorrect CAS value */
  cmd.plain.message.header.request.cas=
          htonll(rsp.plain.message.header.response.cas - 1);
  execute(resend_packet(&cmd));
  execute(recv_packet(&rsp));
  verify(validate_response_header(&rsp, cc, PROTOCOL_BINARY_RESPONSE_KEY_EEXISTS));

  return test_binary_noop();
}

static enum test_return test_binary_set(void)
{
  return test_binary_set_impl("test_binary_set", PROTOCOL_BINARY_CMD_SET);
}

static enum test_return test_binary_setq(void)
{
  return test_binary_set_impl("test_binary_setq", PROTOCOL_BINARY_CMD_SETQ);
}

static enum test_return test_binary_add_impl(const char* key, uint8_t cc)
{
  command cmd;
  response rsp;
  uint64_t value= 0xdeadbeefdeadcafe;
  storage_command(&cmd, cc, key, strlen(key), &value, sizeof (value), 0, 0);

  /* first add should work, rest of them should fail (even with cas
     as wildcard */
  for (int ii=0; ii < 10; ii++)
  {
    if (ii == 0)
      execute(send_packet(&cmd));
    else
      execute(resend_packet(&cmd));

    if (cc == PROTOCOL_BINARY_CMD_ADD || ii > 0)
    {
      uint16_t expected_result;
      if (ii == 0)
        expected_result= PROTOCOL_BINARY_RESPONSE_SUCCESS;
      else
        expected_result= PROTOCOL_BINARY_RESPONSE_KEY_EEXISTS;

      execute(recv_packet(&rsp));
      verify(validate_response_header(&rsp, cc, expected_result));
    }
    else
      execute(test_binary_noop());
  }

  return TEST_PASS;
}

static enum test_return test_binary_add(void)
{
  return test_binary_add_impl("test_binary_add", PROTOCOL_BINARY_CMD_ADD);
}

static enum test_return test_binary_addq(void)
{
  return test_binary_add_impl("test_binary_addq", PROTOCOL_BINARY_CMD_ADDQ);
}

static enum test_return set_item(const char *key, const char *value)
{
  command cmd;
  response rsp;
  storage_command(&cmd, PROTOCOL_BINARY_CMD_SET, key, strlen(key),
                  value, strlen(value), 0, 0);
  execute(send_packet(&cmd));
  execute(recv_packet(&rsp));
  verify(validate_response_header(&rsp, PROTOCOL_BINARY_CMD_SET,
                                  PROTOCOL_BINARY_RESPONSE_SUCCESS));
  return TEST_PASS;
}

static enum test_return test_binary_replace_impl(const char* key, uint8_t cc)
{
  command cmd;
  response rsp;
  uint64_t value= 0xdeadbeefdeadcafe;
  storage_command(&cmd, cc, key, strlen(key), &value, sizeof (value), 0, 0);

  /* first replace should fail, successive should succeed (when the
     item is added! */
  for (int ii= 0; ii < 10; ii++)
  {
    if (ii == 0)
      execute(send_packet(&cmd));
    else
      execute(resend_packet(&cmd));

    if (cc == PROTOCOL_BINARY_CMD_REPLACE || ii == 0)
    {
      uint16_t expected_result;
      if (ii == 0)
        expected_result=PROTOCOL_BINARY_RESPONSE_KEY_ENOENT;
      else
        expected_result=PROTOCOL_BINARY_RESPONSE_SUCCESS;

      execute(recv_packet(&rsp));
      verify(validate_response_header(&rsp, cc, expected_result));

      if (ii == 0)
        execute(set_item(key, key));
    }
    else
      execute(test_binary_noop());
  }

  /* verify that replace with CAS value works! */
  cmd.plain.message.header.request.cas=
          htonll(rsp.plain.message.header.response.cas);
  execute(resend_packet(&cmd));

  if (cc == PROTOCOL_BINARY_CMD_REPLACE)
  {
    execute(recv_packet(&rsp));
    verify(validate_response_header(&rsp, cc, PROTOCOL_BINARY_RESPONSE_SUCCESS));
  }
  else
    execute(test_binary_noop());

  /* try to set with an incorrect CAS value */
  cmd.plain.message.header.request.cas=
          htonll(rsp.plain.message.header.response.cas - 1);
  execute(resend_packet(&cmd));
  execute(recv_packet(&rsp));
  verify(validate_response_header(&rsp, cc, PROTOCOL_BINARY_RESPONSE_KEY_EEXISTS));

  return TEST_PASS;
}

static enum test_return test_binary_replace(void)
{
  return test_binary_replace_impl("test_binary_replace", PROTOCOL_BINARY_CMD_REPLACE);
}

static enum test_return test_binary_replaceq(void)
{
  return test_binary_replace_impl("test_binary_replaceq", PROTOCOL_BINARY_CMD_REPLACEQ);
}

static enum test_return test_binary_delete_impl(const char *key, uint8_t cc)
{
  command cmd;
  response rsp;
  raw_command(&cmd, cc, key, strlen(key), NULL, 0);

  /* The delete shouldn't work the first time, because the item isn't there */
  execute(send_packet(&cmd));
  execute(recv_packet(&rsp));
  verify(validate_response_header(&rsp, cc, PROTOCOL_BINARY_RESPONSE_KEY_ENOENT));
  execute(set_item(key, key));

  /* The item should be present now, resend*/
  execute(resend_packet(&cmd));
  if (cc == PROTOCOL_BINARY_CMD_DELETE)
  {
    execute(recv_packet(&rsp));
    verify(validate_response_header(&rsp, cc, PROTOCOL_BINARY_RESPONSE_SUCCESS));
  }

  execute(test_binary_noop());

  return TEST_PASS;
}

static enum test_return test_binary_delete(void)
{
  return test_binary_delete_impl("test_binary_delete", PROTOCOL_BINARY_CMD_DELETE);
}

static enum test_return test_binary_deleteq(void)
{
  return test_binary_delete_impl("test_binary_deleteq", PROTOCOL_BINARY_CMD_DELETEQ);
}

static enum test_return test_binary_get_impl(const char *key, uint8_t cc)
{
  command cmd;
  response rsp;

  raw_command(&cmd, cc, key, strlen(key), NULL, 0);
  execute(send_packet(&cmd));

  if (cc == PROTOCOL_BINARY_CMD_GET || cc == PROTOCOL_BINARY_CMD_GETK)
  {
    execute(recv_packet(&rsp));
    verify(validate_response_header(&rsp, cc, PROTOCOL_BINARY_RESPONSE_KEY_ENOENT));
  }
  else
    execute(test_binary_noop());

  execute(set_item(key, key));
  execute(resend_packet(&cmd));
  execute(recv_packet(&rsp));
  verify(validate_response_header(&rsp, cc, PROTOCOL_BINARY_RESPONSE_SUCCESS));

  return TEST_PASS;
}

static enum test_return test_binary_get(void)
{
  return test_binary_get_impl("test_binary_get", PROTOCOL_BINARY_CMD_GET);
}

static enum test_return test_binary_getk(void)
{
  return test_binary_get_impl("test_binary_getk", PROTOCOL_BINARY_CMD_GETK);
}

static enum test_return test_binary_getq(void)
{
  return test_binary_get_impl("test_binary_getq", PROTOCOL_BINARY_CMD_GETQ);
}

static enum test_return test_binary_getkq(void)
{
  return test_binary_get_impl("test_binary_getkq", PROTOCOL_BINARY_CMD_GETKQ);
}

static enum test_return test_binary_incr_impl(const char* key, uint8_t cc)
{
  command cmd;
  response rsp;
  arithmetic_command(&cmd, cc, key, strlen(key), 1, 0, 0);

  uint64_t ii;
  for (ii= 0; ii < 10; ++ii)
  {
    if (ii == 0)
      execute(send_packet(&cmd));
    else
      execute(resend_packet(&cmd));

    if (cc == PROTOCOL_BINARY_CMD_INCREMENT)
    {
      execute(recv_packet(&rsp));
      verify(validate_response_header(&rsp, cc, PROTOCOL_BINARY_RESPONSE_SUCCESS));
      verify(ntohll(rsp.incr.message.body.value) == ii);
    }
    else
      execute(test_binary_noop());
  }

  /* @todo add incorrect CAS */
  return TEST_PASS;
}

static enum test_return test_binary_incr(void)
{
  return test_binary_incr_impl("test_binary_incr", PROTOCOL_BINARY_CMD_INCREMENT);
}

static enum test_return test_binary_incrq(void)
{
  return test_binary_incr_impl("test_binary_incrq", PROTOCOL_BINARY_CMD_INCREMENTQ);
}

static enum test_return test_binary_decr_impl(const char* key, uint8_t cc)
{
  command cmd;
  response rsp;
  arithmetic_command(&cmd, cc, key, strlen(key), 1, 9, 0);

  int ii;
  for (ii= 9; ii > -1; --ii)
  {
    if (ii == 9)
      execute(send_packet(&cmd));
    else
      execute(resend_packet(&cmd));

    if (cc == PROTOCOL_BINARY_CMD_DECREMENT)
    {
      execute(recv_packet(&rsp));
      verify(validate_response_header(&rsp, cc, PROTOCOL_BINARY_RESPONSE_SUCCESS));
      verify(ntohll(rsp.decr.message.body.value) == (uint64_t)ii);
    }
    else
      execute(test_binary_noop());
  }

  /* decr 0 should not wrap */
  execute(resend_packet(&cmd));
  if (cc == PROTOCOL_BINARY_CMD_DECREMENT)
  {
    execute(recv_packet(&rsp));
    verify(validate_response_header(&rsp, cc, PROTOCOL_BINARY_RESPONSE_SUCCESS));
    verify(ntohll(rsp.decr.message.body.value) == 0);
  }
  else
  {
    /* @todo get the value and verify! */

  }

  /* @todo add incorrect cas */
  execute(test_binary_noop());
  return TEST_PASS;
}

static enum test_return test_binary_decr(void)
{
  return test_binary_decr_impl("test_binary_decr",
                               PROTOCOL_BINARY_CMD_DECREMENT);
}

static enum test_return test_binary_decrq(void)
{
  return test_binary_decr_impl("test_binary_decrq",
                               PROTOCOL_BINARY_CMD_DECREMENTQ);
}

static enum test_return test_binary_version(void)
{
  command cmd;
  response rsp;
  raw_command(&cmd, PROTOCOL_BINARY_CMD_VERSION, NULL, 0, NULL, 0);

  execute(send_packet(&cmd));
  execute(recv_packet(&rsp));
  verify(validate_response_header(&rsp, PROTOCOL_BINARY_CMD_VERSION,
                                  PROTOCOL_BINARY_RESPONSE_SUCCESS));

  return TEST_PASS;
}

static enum test_return test_binary_flush_impl(const char *key, uint8_t cc)
{
  command cmd;
  response rsp;

  for (int ii= 0; ii < 2; ++ii)
  {
    execute(set_item(key, key));
    flush_command(&cmd, cc, 0, ii == 0);
    execute(send_packet(&cmd));

    if (cc == PROTOCOL_BINARY_CMD_FLUSH)
    {
      execute(recv_packet(&rsp));
      verify(validate_response_header(&rsp, cc, PROTOCOL_BINARY_RESPONSE_SUCCESS));
    }
    else
      execute(test_binary_noop());

    raw_command(&cmd, PROTOCOL_BINARY_CMD_GET, key, strlen(key), NULL, 0);
    execute(send_packet(&cmd));
    execute(recv_packet(&rsp));
    verify(validate_response_header(&rsp, PROTOCOL_BINARY_CMD_GET,
                                    PROTOCOL_BINARY_RESPONSE_KEY_ENOENT));
  }

  return TEST_PASS;
}

static enum test_return test_binary_flush(void)
{
  return test_binary_flush_impl("test_binary_flush", PROTOCOL_BINARY_CMD_FLUSH);
}

static enum test_return test_binary_flushq(void)
{
  return test_binary_flush_impl("test_binary_flushq", PROTOCOL_BINARY_CMD_FLUSHQ);
}

static enum test_return test_binary_concat_impl(const char *key, uint8_t cc)
{
  command cmd;
  response rsp;
  const char *value;

  if (cc == PROTOCOL_BINARY_CMD_APPEND || cc == PROTOCOL_BINARY_CMD_APPENDQ)
    value="hello";
  else
    value=" world";

  execute(set_item(key, value));

  if (cc == PROTOCOL_BINARY_CMD_APPEND || cc == PROTOCOL_BINARY_CMD_APPENDQ)
    value=" world";
  else
    value="hello";

  raw_command(&cmd, cc, key, strlen(key), value, strlen(value));
  execute(send_packet(&cmd));
  if (cc == PROTOCOL_BINARY_CMD_APPEND || cc == PROTOCOL_BINARY_CMD_PREPEND)
  {
    execute(recv_packet(&rsp));
    verify(validate_response_header(&rsp, cc, PROTOCOL_BINARY_RESPONSE_SUCCESS));
  }
  else
    execute(test_binary_noop());

  raw_command(&cmd, PROTOCOL_BINARY_CMD_GET, key, strlen(key), NULL, 0);
  execute(send_packet(&cmd));
  execute(recv_packet(&rsp));
  verify(validate_response_header(&rsp, PROTOCOL_BINARY_CMD_GET,
                                  PROTOCOL_BINARY_RESPONSE_SUCCESS));
  verify(rsp.plain.message.header.response.bodylen - 4 == 11);
  verify(memcmp(rsp.bytes + 28, "hello world", 11) == 0);

  return TEST_PASS;
}

static enum test_return test_binary_append(void)
{
  return test_binary_concat_impl("test_binary_append", PROTOCOL_BINARY_CMD_APPEND);
}

static enum test_return test_binary_prepend(void)
{
  return test_binary_concat_impl("test_binary_prepend", PROTOCOL_BINARY_CMD_PREPEND);
}

static enum test_return test_binary_appendq(void)
{
  return test_binary_concat_impl("test_binary_appendq", PROTOCOL_BINARY_CMD_APPENDQ);
}

static enum test_return test_binary_prependq(void)
{
  return test_binary_concat_impl("test_binary_prependq", PROTOCOL_BINARY_CMD_PREPENDQ);
}

static enum test_return test_binary_stat(void)
{
  command cmd;
  response rsp;

  raw_command(&cmd, PROTOCOL_BINARY_CMD_STAT, NULL, 0, NULL, 0);
  execute(send_packet(&cmd));

  do
  {
    execute(recv_packet(&rsp));
    verify(validate_response_header(&rsp, PROTOCOL_BINARY_CMD_STAT,
                                    PROTOCOL_BINARY_RESPONSE_SUCCESS));
  } while (rsp.plain.message.header.response.keylen != 0);

  return TEST_PASS;
}

static enum test_return test_binary_illegal(void)
{
  command cmd;
  response rsp;
  uint8_t cc= 0x1b;

  while (cc != 0x00)
  {
    raw_command(&cmd, cc, NULL, 0, NULL, 0);
    execute(send_packet(&cmd));
    execute(recv_packet(&rsp));
    verify(validate_response_header(&rsp, cc, PROTOCOL_BINARY_RESPONSE_UNKNOWN_COMMAND));
    ++cc;
  }

  return TEST_PASS;
}

typedef enum test_return(*TEST_FUNC)(void);

struct testcase
{
  const char *description;
  TEST_FUNC function;
};

struct testcase testcases[]= {
  { "noop", test_binary_noop},
  { "quit", test_binary_quit},
  { "quitq", test_binary_quitq},
  { "set", test_binary_set},
  { "setq", test_binary_setq},
  { "flush", test_binary_flush},
  { "flushq", test_binary_flushq},
  { "add", test_binary_add},
  { "addq", test_binary_addq},
  { "replace", test_binary_replace},
  { "replaceq", test_binary_replaceq},
  { "delete", test_binary_delete},
  { "deleteq", test_binary_deleteq},
  { "get", test_binary_get},
  { "getq", test_binary_getq},
  { "getk", test_binary_getk},
  { "getkq", test_binary_getkq},
  { "incr", test_binary_incr},
  { "incrq", test_binary_incrq},
  { "decr", test_binary_decr},
  { "decrq", test_binary_decrq},
  { "version", test_binary_version},
  { "append", test_binary_append},
  { "appendq", test_binary_appendq},
  { "prepend", test_binary_prepend},
  { "prependq", test_binary_prependq},
  { "stat", test_binary_stat},
  { "illegal", test_binary_illegal},
  { NULL, NULL}
};

int main(int argc, char **argv)
{
  static const char * const status_msg[]= {"[skip]", "[pass]", "[pass]", "[FAIL]"};
  int total= 0;
  int failed= 0;
  const char *hostname= "localhost";
  const char *port= "11211";
  int cmd;

  while ((cmd= getopt(argc, argv, "t:vch:p:?")) != EOF)
  {
    switch (cmd) {
    case 't':
      timeout= atoi(optarg);
      if (timeout == 0)
      {
        fprintf(stderr, "Invalid timeout. Please specify a number for -t\n");
        return 1;
      }
      break;
    case 'v': verbose= true;
      break;
    case 'c': do_core= true;
      break;
    case 'h': hostname= optarg;
      break;
    case 'p': port= optarg;
      break;
    default:
      fprintf(stderr, "Usage: %s [-h hostname] [-p port] [-c] [-v] [-t n]\n"
              "\t-c\tGenerate coredump if a test fails\n"
              "\t-v\tVerbose test output (print out the assertion)\n"
              "\t-c n\tSet the timeout for io-operations to n seconds\n",
              argv[0]);
      return 1;
    }
  }

  sock= connect_server(hostname, port);
  if (sock == -1)
  {
    fprintf(stderr, "Failed to connect to <%s:%s>: %s\n",
            hostname, port, strerror(errno));
    return 1;
  }

  for (int ii= 0; testcases[ii].description != NULL; ++ii)
  {
    ++total;
    fprintf(stdout, "%s\t\t", testcases[ii].description);
    fflush(stdout);

    bool reconnect= false;
    enum test_return ret= testcases[ii].function();
    fprintf(stderr, "%s\n", status_msg[ret]);
    if (ret == TEST_FAIL)
    {
      reconnect= true;
      ++failed;
    }
    else if (ret == TEST_PASS_RECONNECT)
      reconnect= true;

    if (reconnect)
    {
      (void) close(sock);
      if ((sock=connect_server(hostname, port)) == -1)
      {
        fprintf(stderr, "Failed to connect to <%s:%s>: %s\n",
                hostname, port, strerror(errno));
        fprintf(stderr, "%d of %d tests failed\n", failed, total);
        return 1;
      }
    }
  }

  (void) close(sock);
  if (failed == 0)
    fprintf(stdout, "All tests passed\n");
  else
    fprintf(stderr, "%d of %d tests failed\n", failed, total);

  return (failed == 0) ? 0 : 1;
}
