#ifndef COMMANDS_H
#define COMMANDS_H

#include <iomanip>
#include <stdexcept>

#include "interfaces.h"


class LoggingCommand: public ICommand {
public:
    LoggingCommand(std::ostream & oss):m_oss(oss) {}
    void log(std::string const & s) { m_oss<<s<<std::endl; }
protected:
    std::ostream & m_oss;
};


// class LogSomethingCommand:public LoggingCommand {
// public:
//     LogSomethingCommand(std::ostream & oss): m_oss(oss) {}
//     void execute() override {  this->log("Something"); }
// };

class LogUnknownExceptionCaughtCommand: public LoggingCommand {
public:
    LogUnknownExceptionCaughtCommand(std::ostream & oss): LoggingCommand(oss) {}
    void execute() override { log("Unknown exception caught"); }
};


class LogUnknownCommandHasThrownExceptionCommand: public LoggingCommand {
public:
    LogUnknownCommandHasThrownExceptionCommand(std::ostream & oss): LoggingCommand(oss) {}
    void execute() override { log("Unknown command has thrown an exception"); }
};


class LogEmptyHandlerMetCommand: public LoggingCommand {
public:
    LogEmptyHandlerMetCommand(std::ostream & oss): LoggingCommand(oss) {}
    void execute() override { log("Empty handler function met"); }
};


class LogExceptionCommand: public LoggingCommand {
public:
    LogExceptionCommand(std::ostream & oss, std::exception const & ex):LoggingCommand(oss), m_s(ex.what()) {}
    void execute() override { log(m_s); }
public:
    std::string m_s;
};


class ThrowingCommand: public ICommand {
public:
    ThrowingCommand() {}
    void execute() override { throw std::runtime_error("Error occured during command execution."); }
};


class ThrowingNTimesCommand: public ThrowingCommand {
    using Super_type=ThrowingCommand;
public:
    ThrowingNTimesCommand(size_t n): m_n(n) {}

    void execute() override {
        if(m_n>0) {
            --m_n;
            throw std::runtime_error("Error occured during command execution.");
        }
    }
    size_t left() const { return m_n; }
protected:
    size_t m_n;
};

class RepeatCommand: public ICommand {
public:
    RepeatCommand(ICommand_ptr const & cmd):  m_cmd(cmd) {}
    void execute() override { m_cmd->execute(); }
protected:
    ICommand_ptr m_cmd;
};

class CountedRepeatsCommand: public ICommand {
public:
    CountedRepeatsCommand(ICommand_ptr const & cmd):  m_cmd(cmd), m_cnt(0) {}
    void execute() override {++m_cnt; m_cmd->execute(); }
    size_t count() const { return m_cnt; }
protected:

    ICommand_ptr m_cmd;
    size_t m_cnt;
};

class RepeatSecondTimeCommand: public RepeatCommand {
public:
    RepeatSecondTimeCommand(ICommand_ptr const & cmd):RepeatCommand(cmd) {}
};


class EnqueueCommand: public ICommand {
public:
    EnqueueCommand(ICommandQueue_ptr const & q, ICommand_ptr const & cmd): m_q(q), m_cmd(cmd) {}
    void execute() override { m_q->push(m_cmd); }
protected:
    ICommand_ptr m_cmd;
    ICommandQueue_ptr m_q;
};

#endif // COMMANDS_H
