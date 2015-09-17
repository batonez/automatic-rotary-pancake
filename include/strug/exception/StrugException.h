#pragma once

#include <glade/exception/GladeException.h>

class StrugException: public GladeException
{
  public:
    StrugException(): GladeException() {}
    StrugException(std::string &message): GladeException(message) {}
    StrugException(const char *message): GladeException(message) {}
};
