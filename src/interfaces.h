#ifndef INTERFACES_H
#define INTERFACES_H

#include <any>
#include <optional>
#include <string>
#include <memory>
#include <functional>

#include "datatypes.h"

class ICommand {
public:
    virtual ~ICommand(){}
    virtual void execute() =0;
};

using ICommand_ptr=std::shared_ptr<ICommand>;


using  HandlerFunction=std::function<ICommand_ptr(ICommand_ptr const &, std::exception const & ex)>;


class IExceptionHandler {
public:
    virtual ~IExceptionHandler(){}
    virtual ICommand_ptr get(ICommand_ptr const &, std::exception const & ex)=0;
    template<typename Cmd, typename Ex>
    void set(HandlerFunction const & f) {
        size_t iCmd=type_index<Cmd>();
        size_t iEx=type_index<Ex>();
        doSet(iCmd,iEx,f);
    }
protected:
    template<typename T>
    static size_t type_index(T && t) { return typeid(t).hash_code(); }

    template<typename T>
    static size_t type_index(T *t=nullptr) { return typeid(T).hash_code(); }

    virtual void doSet(size_t, size_t, HandlerFunction const & f)=0;
};

using IExceptionHandler_ptr=std::shared_ptr<IExceptionHandler>;


class ICommandQueue
{
public:
    virtual ~ICommandQueue() {}
    virtual void push(ICommand_ptr const & cmd)=0;
    virtual ICommand_ptr take()=0;
    virtual bool empty() const =0;
};

using ICommandQueue_ptr=std::shared_ptr<ICommandQueue>;


class IEventLoop {
public:
    virtual ~IEventLoop() {}
    virtual void operator()()=0;
    virtual ICommandQueue_ptr queue() const=0;
};

using IEventLoop_ptr=std::shared_ptr<IEventLoop>;

#endif // INTERFACES_H
