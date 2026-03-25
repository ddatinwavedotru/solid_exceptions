#include "gtest/gtest.h"
#include "library.h"

bool hasText(std::ostream & os) {
    std::streampos pos = os.tellp();  // store current location
    os.seekp(0, std::ios_base::end);  // go to end
    bool empty = (os.tellp() == 0);   // check size == 0 ?
    os.seekp(pos);
    return !empty;
}

// Реализовать Команду, которая записывает информацию о выброшенном исключении в лог.
TEST(solid_exceptions, log_thrown_exception) {
    ICommandQueue_ptr cq=std::make_shared<CommandQueue>();
    std::ostringstream handlerLog;
    IExceptionHandler_ptr eh=std::make_shared<ExceptionHandler>(handlerLog);
    IEventLoop_ptr el=std::make_shared<EventLoop>(cq,eh);

    std::ostringstream log;
    eh->set<ThrowingCommand,std::runtime_error>([&log](const ICommand_ptr & cmd, const std::exception & ex) ->ICommand_ptr {
        return std::make_shared<LogExceptionCommand>(log,ex);
    });

    cq->push(std::make_shared<ThrowingCommand>());
    (*el)();
    EXPECT_TRUE(hasText(log));
    EXPECT_FALSE(hasText(handlerLog));
}



// Реализовать обработчик исключения, который ставит Команду, пишущую в лог в очередь Команд.
TEST(solid_exceptions, put_logging_command_in_queue) {
    ICommandQueue_ptr cq=std::make_shared<CommandQueue>();
    std::ostringstream handlerLog;
    IExceptionHandler_ptr eh=std::make_shared<ExceptionHandler>(handlerLog);
    IEventLoop_ptr el=std::make_shared<EventLoop>(cq,eh);

    std::ostringstream log;

    eh->set<ThrowingCommand,std::runtime_error>([&log,&cq](const ICommand_ptr & cmd, const std::exception & ex) ->ICommand_ptr {
        std::shared_ptr<ICommand> logCmd=std::make_shared<LogExceptionCommand>(log,ex);
        return std::make_shared<EnqueueCommand>(cq,logCmd);
    });

    cq->push(std::make_shared<ThrowingCommand>());
    (*el)();
    EXPECT_TRUE(hasText(log));
    EXPECT_FALSE(hasText(handlerLog));
}

// Реализовать Команду, которая повторяет Команду, выбросившую исключение.
TEST(solid_exceptions, repeat_thrown_command_once) {
    ICommandQueue_ptr cq=std::make_shared<CommandQueue>();
    std::ostringstream handlerLog;
    IExceptionHandler_ptr eh=std::make_shared<ExceptionHandler>(handlerLog);
    IEventLoop_ptr el=std::make_shared<EventLoop>(cq,eh);

    eh->set<ThrowingNTimesCommand,std::runtime_error>([&cq](const ICommand_ptr & cmd, const std::exception & ex) ->ICommand_ptr {
        return std::make_shared<RepeatCommand>(cmd);
    });

    std::shared_ptr<ThrowingNTimesCommand> nT=std::make_shared<ThrowingNTimesCommand>(1);
    cq->push(nT);
    (*el)();
    EXPECT_EQ(nT->left(),0);
    EXPECT_FALSE(hasText(handlerLog));
}

// Реализовать обработчик исключения, который ставит в очередь Команду - повторитель команды, выбросившей исключение.
TEST(solid_exceptions, equeue_repeater_of_thrown_command_once) {
    ICommandQueue_ptr cq=std::make_shared<CommandQueue>();
    std::ostringstream handlerLog;
    IExceptionHandler_ptr eh=std::make_shared<ExceptionHandler>(handlerLog);
    IEventLoop_ptr el=std::make_shared<EventLoop>(cq,eh);

    eh->set<ThrowingNTimesCommand,std::runtime_error>([&cq](const ICommand_ptr & cmd, const std::exception & ex) ->ICommand_ptr {
        return std::make_shared<EnqueueCommand>(cq, std::make_shared<RepeatCommand>(cmd));
    });

    std::shared_ptr<ThrowingNTimesCommand> nT=std::make_shared<ThrowingNTimesCommand>(1);
    cq->push(nT);
    (*el)();
    EXPECT_EQ(nT->left(),0);
    EXPECT_FALSE(hasText(handlerLog));
}

// С помощью Команд из пункта 4 и пункта 6 реализовать следующую обработку исключений:
// при первом выбросе исключения повторить команду, при повторном выбросе исключения записать информацию в лог.
TEST(solid_exceptions, repeat_once_then_log_thrown_command) {
    ICommandQueue_ptr cq=std::make_shared<CommandQueue>();
    std::ostringstream handlerLog;
    IExceptionHandler_ptr eh=std::make_shared<ExceptionHandler>(handlerLog);
    IEventLoop_ptr el=std::make_shared<EventLoop>(cq,eh);

    std::ostringstream log;

    eh->set<ThrowingNTimesCommand,std::runtime_error>([&cq](const ICommand_ptr & cmd, const std::exception & ex) ->ICommand_ptr {
        return std::make_shared<EnqueueCommand>(cq, std::make_shared<RepeatCommand>(cmd));
    });

    eh->set<RepeatCommand,std::runtime_error>([&log](const ICommand_ptr & cmd, const std::exception & ex) ->ICommand_ptr {
        EXPECT_FALSE(hasText(log));
        return std::make_shared<LogExceptionCommand>(log,ex);
    });

    std::shared_ptr<ThrowingNTimesCommand> nT=std::make_shared<ThrowingNTimesCommand>(2);
    cq->push(nT);
    (*el)();
    EXPECT_EQ(nT->left(),0);
    EXPECT_TRUE(hasText(log));
    EXPECT_FALSE(hasText(handlerLog));
}

// Реализовать стратегию обработки исключения - повторить два раза, потом записать в лог.
// Указание: создать новую команду, точно такую же как в пункте 6. Тип этой команды будет показывать, что Команду не удалось выполнить два раза.
TEST(solid_exceptions, repeat_twice_then_log_thrown_command) {
    ICommandQueue_ptr cq=std::make_shared<CommandQueue>();
    std::ostringstream handlerLog;
    IExceptionHandler_ptr eh=std::make_shared<ExceptionHandler>(handlerLog);
    IEventLoop_ptr el=std::make_shared<EventLoop>(cq,eh);

    std::ostringstream log;

    eh->set<ThrowingNTimesCommand,std::runtime_error>([&cq](const ICommand_ptr & cmd, const std::exception & ex) ->ICommand_ptr {
        return std::make_shared<EnqueueCommand>(cq, std::make_shared<RepeatCommand>(cmd));
    });

    eh->set<RepeatCommand,std::runtime_error>([&cq](const ICommand_ptr & cmd, const std::exception & ex) ->ICommand_ptr {
        return std::make_shared<EnqueueCommand>(cq, std::make_shared<RepeatSecondTimeCommand>(cmd));
    });

    eh->set<RepeatSecondTimeCommand,std::runtime_error>([&log,&cq](const ICommand_ptr & cmd, const std::exception & ex) ->ICommand_ptr {
        EXPECT_FALSE(hasText(log));
        return std::make_shared<LogExceptionCommand>(log,ex);
    });

    std::shared_ptr<ThrowingNTimesCommand> nT=std::make_shared<ThrowingNTimesCommand>(3);
    cq->push(nT);
    (*el)();
    EXPECT_EQ(nT->left(),0);
    EXPECT_TRUE(hasText(log));
    EXPECT_FALSE(hasText(handlerLog));
}


// Реализовать стратегию обработки исключения - повторить N раз, потом записать в лог.
TEST(solid_exceptions, repeat_N_times_then_log_thrown_command) {
    ICommandQueue_ptr cq=std::make_shared<CommandQueue>();
    std::ostringstream handlerLog;
    IExceptionHandler_ptr eh=std::make_shared<ExceptionHandler>(handlerLog);
    IEventLoop_ptr el=std::make_shared<EventLoop>(cq,eh);

    std::ostringstream log;
    size_t N=5;

    std::shared_ptr<CountedRepeatsCommand> repCmd;
    eh->set<ThrowingNTimesCommand,std::runtime_error>([&cq,&repCmd](const ICommand_ptr & cmd, const std::exception & ex) ->ICommand_ptr {
        return std::make_shared<EnqueueCommand>(cq, repCmd=std::make_shared<CountedRepeatsCommand>(cmd));
    });

    eh->set<CountedRepeatsCommand,std::runtime_error>([&log,&cq,&N](const ICommand_ptr & cmd, const std::exception & ex) ->ICommand_ptr {
        EXPECT_FALSE(hasText(log));
        CountedRepeatsCommand* cr=dynamic_cast<CountedRepeatsCommand*>(cmd.operator->());
        EXPECT_TRUE(cr);
        if(cr->count()<N) {
            return std::make_shared<EnqueueCommand>(cq, cmd);
        }
        return std::make_shared<LogExceptionCommand>(log,ex);
    });

    std::shared_ptr<ThrowingNTimesCommand> nT=std::make_shared<ThrowingNTimesCommand>(N+1);
    cq->push(nT);
    (*el)();
    EXPECT_EQ(nT->left(),0);
    EXPECT_TRUE(repCmd);
    EXPECT_EQ(repCmd->count(),N);
    EXPECT_TRUE(hasText(log));
    EXPECT_FALSE(hasText(handlerLog));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
