#ifndef ACCLOCK_H_INCLUDED
#define ACCLOCK_H_INCLUDED
#include <windows.h>

#ifndef DLL_EXPORT
#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif
#endif // DLL_EXPORT

#ifdef __cplusplus
extern "C"
{
#endif
///这么写是因为Codeblocks识别不了alib::ng
namespace alib{
namespace ng{
    /** \brief time
     *  all : millisec time since launched
     *  offset : millisec time since last recorded "bookmark"
     */
    struct DLL_EXPORT TMST0{
        double all;
        double offset;
    };

    class DLL_EXPORT Clock{
    public:
        static LARGE_INTEGER m_cpuFeq;
        static bool static_lock;
        static bool HTimerSup;

        /** \brief constructor
        * \param start the clock right now?
        * \param use nano second if support
        */
        Clock(bool start = true,bool useHTimerIfSup = true);
        /** \brief start the clock
        * does nothing if already started
        */
        void start();
        /** \brief stop the clock
        * does nothing if already stopped
        * \return recorded time
        */
        TMST0 stop();
        /** \brief pause the clock
        * ...
        * \return recorded time
        */
        TMST0 pause();
        /** \brief resume the clock
        */
        void resume();
        /** \brief reset the clock
        *
        * = stop() + start()
        */

        void reset();

        /** \brief get current stats
        *
        * \return is stopped
        */

        bool isStop();

        /** \brief get milliseconds since started
        *
        * \return milliseconds
        */
        double getAllTime();
        /** \brief get offset*/
        double getOffset();
        /** \brief composed info:alltime + offset*/
        TMST0 now();

        /** \brief clear offset */
        void clearOffset();

        /** \brief select clock freq,
        * toggle timer while the clock is running has undefined behavior
        * 你脑残在clock运行时切换也不是不可以，后果自己承担
        */
        void toggleTimer(bool useHighTimerIfSup);
    private:
        double m_pauseGained;
        double m_StartTime;
        double m_PreTime;
        bool m_start;
        bool m_paused;
        bool m_useHTimer;
    };

    struct DLL_EXPORT Trigger{
    public:
        Trigger(Clock& clock,double duration);
        /** \brief test the trigger,does nothing if no clock*/
        bool test(bool resetIfSucceeds = true);
        /** \brief reset the trigger*/
        void reset();
        /** \brief set duration */
        void setDuration(double duration);
        /** \brief set clock */
        void setClock(Clock & clock);
        /**< you can also set the duration manually :) */
        double duration;
    private:
        Clock * m_clock;
        double rec;
    };
}
}

#ifdef __cplusplus
}
#endif

#endif // ACCLOCK_H_INCLUDED
