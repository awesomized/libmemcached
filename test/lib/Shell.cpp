#include "Shell.hpp"

#include <cstdlib>

bool Shell::run(const string &command_, string &output) {
  auto command = prepareCommand(command_);
  auto *file = popen(command.c_str(), "r");

  if (!file) {
    perror("Shell::run popen()");
    return false;
  }

  do {
    char data[1U<<12U];
    auto read = fread(data, 1, sizeof(data), file);

    if (read) {
      output.append(data, read);
    }
    if (ferror(file)) {
      cerr << "Shell::run read(): " << strerror(ferror(file));
      break;
    }
  } while (!feof(file));

  auto error = ferror(file);
  auto status = pclose(file);
  return !error && !status;
}

bool Shell::run(const string &command) {
  auto error = system(prepareCommand(command).c_str());
  if (error == -1) {
    perror("Shell::run system()");
    return false;
  }
  return !error;
}

Shell::Shell(bool redirect_stderr)
: redirect{redirect_stderr}
{
  if (!system(nullptr)) {
    throw runtime_error("no shell available");
  }
}

Shell::Shell(string prefix_, bool redirect_stderr)
: prefix{move(prefix_)}
, redirect{redirect_stderr}
{
  if (!system(nullptr)) {
    throw runtime_error("no shell available");
  }
}

string Shell::prepareCommand(const string &command_) {
  string command;
  if (prefix.length()) {
    command.append(prefix);
    command.append("/");
  }
  command.append(command_);
  if (redirect) {
    command.append(" 2>&1");
  }
  INFO("Prepared command: " << command);
  return command;
}
