
#include <time.h>
#include <rtt/RTT.hpp>

#include <rtt_rosclock/rtt_rosclock.h>
#include <rtt_rosclock/rtt_rosclock_sim_clock_thread.h>

const ros::Time rtt_rosclock::rtt_now() 
{
  //return ros::Time(((double)RTT::os::TimeService::ticks2nsecs(RTT::os::TimeService::Instance()->getTicks()))*1E-9);
  return ros::Time(RTT::nsecs_to_Seconds(RTT::os::TimeService::Instance()->getNSecs()));
}

const ros::Time rtt_rosclock::ros_now() 
{
  return ros::Time::now();
}

const ros::Time rtt_rosclock::host_rt_now() 
{
  if(SimClockThread::GetInstance() && SimClockThread::GetInstance()->simTimeEnabled()) {
    return rtt_now();   
  } else {
    #ifdef __XENO__
    // Use Xenomai 2.6 feature to get the NTP-synched real-time clock
    timespec ts = {0,0};
    int ret = clock_gettime(CLOCK_HOST_REALTIME, &ts);
    if(ret) {
      RTT::log(RTT::Error) << "Could not query CLOCK_HOST_REALTIME (" << CLOCK_HOST_REALTIME <<"): "<< errno << RTT::endlog();
    }

    return ros::Time(ts.tv_sec, ts.tv_nsec);
    #else 
    return ros::Time::now();
    #endif
  }
}

const RTT::Seconds rtt_rosclock::host_rt_offset_from_rtt() 
{
  return (rtt_rosclock::host_rt_now() - rtt_rosclock::rtt_now()).toSec();
}

void rtt_rosclock::use_ros_clock_topic()
{
  SimClockThread::Instance()->useROSClockTopic();
}

void rtt_rosclock::use_manual_clock()
{
  SimClockThread::Instance()->useManualClock();
}

const bool rtt_rosclock::set_sim_clock_activity(RTT::TaskContext *t)
{
  if (!t) return false;
  return t->setActivity(new SimClockActivity(t->getPeriod()));
}

const bool rtt_rosclock::enable_sim()
{
  return SimClockThread::Instance()->start();
}

const bool rtt_rosclock::disable_sim() 
{
  return SimClockThread::Instance()->stop();
}

void rtt_rosclock::update_sim_clock(const RTT::os::Seconds now)
{
  SimClockThread::Instance()->updateClock(now);
}

