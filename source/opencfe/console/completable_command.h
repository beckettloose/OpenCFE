#ifndef __COMPLETABLE_COMMAND_H
#define __COMPLETABLE_COMMAND_H

#include <string>
#include <vector>

class CompletableCommand {
public:
    virtual ~CompletableCommand() = default;

    virtual std::vector<std::string> complete(const std::string& prefix) const = 0;

    virtual void printHelp(const std::string& prefix) const = 0;
};

#endif // !__COMPLETABLE_COMMAND_H
