#ifndef COMMAND_QUEUE_H
#define COMMAND_QUEUE_H

#include <queue>
#include "interfaces.h"


class CommandQueue: public ICommandQueue
{
public:
    CommandQueue() {}

    void push(ICommand_ptr const & cmd) override { m_q.push(cmd); }
    ICommand_ptr take() override { ICommand_ptr out=m_q.front(); m_q.pop(); return out; }
    bool empty() const override { return m_q.empty(); }

protected:
    std::queue<ICommand_ptr> m_q;
};


class EventLoop: public IEventLoop {
public:
    EventLoop(ICommandQueue_ptr const & ptr,IExceptionHandler_ptr const & eh): m_q(ptr), m_eh(eh) {}

    ICommandQueue_ptr queue() const  override { return m_q; }

    void operator()() override {
        while(!m_q->empty()) {
            ICommand_ptr cmd=m_q->take();
            try{
                cmd->execute();
            } catch(std::exception const & ex) {
                m_eh->get(cmd,ex)->execute();
            }
        }
    }
protected:
    ICommandQueue_ptr m_q;
    IExceptionHandler_ptr m_eh;
};

#endif // COMMAND_QUEUE_H
