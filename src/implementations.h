#ifndef IMPLEMENTATIONS_H
#define IMPLEMENTATIONS_H

#include <any>
#include <optional>
#include <map>

#include "interfaces.h"
#include "commands.h"



class ExceptionHandler: public IExceptionHandler {
public:

    ExceptionHandler(std::ostream & log): m_log(log) {}

    ICommand_ptr get(const ICommand_ptr &cmd, const std::exception &ex) override {
        size_t typeCmd=type_index(*cmd);
        auto it1=m_m.find(typeCmd);
        if(it1==m_m.end()) {
            return std::make_shared<LogUnknownCommandHasThrownExceptionCommand>(m_log);
        }
        size_t typeException=type_index(ex);
        auto it2=it1->second.find(typeException);
        if(it2==it1->second.end()) {
            return std::make_shared<LogUnknownExceptionCaughtCommand>(m_log);
        }
        HandlerFunction hdl=it2->second;
        if(!hdl) {
            return std::make_shared<LogEmptyHandlerMetCommand>(m_log);
        }
        return hdl(cmd,ex);
    }
protected:
    void doSet(size_t cmdType, size_t exType, HandlerFunction const & f) override {
        m_m[cmdType][exType]=f;
    }

    std::map<size_t,std::map<size_t, HandlerFunction> > m_m;
    std::ostream & m_log;
};


#endif // IMPLEMENTATIONS_H
