#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/event.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/deep_history.hpp>
#include <boost/statechart/history.hpp>
#include <boost/statechart/state.hpp>
#include <iostream>

#ifdef DEEP_HISTORY
namespace sc = boost::statechart;

// 事件
struct EvShutdown : sc::event<EvShutdown> {};  // 关机
struct EvRestart : sc::event<EvRestart> {};    // 重新启动
struct EvCharge : sc::event<EvCharge> {};      // 进入充电状态
struct EvMove : sc::event<EvMove> {};          // 进入移动状态
struct EvFastCharge : sc::event<EvFastCharge> {};
struct EvSlowCharge : sc::event<EvSlowCharge> {};
struct EVchargePause : sc::event<EVchargePause> {};

struct EvStart : sc::event<EvStart> {};      //开始


// 预声明状态
struct Operational;
struct Shutdown;
struct Moving;
struct Charging;
struct SlowCharge;
struct FastCharge;
struct chargePause;


// 机器人状态机
struct Robot : sc::state_machine<Robot, Operational> {};


// 运行状态（默认进入 Moving）
struct Operational : sc::simple_state<Operational, Robot, Moving, sc::has_deep_history> {
    using reactions = sc::transition<EvShutdown, Shutdown>;
};

// 关机状态
struct Shutdown : sc::simple_state<Shutdown, Robot> {
    Shutdown() { std::cout << "Robot is Shutdown\n"; }
     typedef sc::transition<EvRestart, sc::deep_history<Moving>> reactions; // 必须指定为组下的子状态。并不是回到Moving
};
// 移动状态
struct Moving : sc::simple_state<Moving, Operational> {
    Moving() { std::cout << "Robot is Moving\n"; }
    using reactions = sc::transition<EvCharge, Charging>;
};

// 充电状态（有深度历史）一个组里只能运行一个状态
struct Charging : sc::simple_state<Charging, Operational, SlowCharge, sc::has_deep_history> {
    typedef boost::mpl::list<sc::transition<EvMove, Moving>,
                             boost::statechart::custom_reaction<EVchargePause>
                             >reactions;
    boost::statechart::result react(const EVchargePause& ev) {
        // 还没return transit<chargePause>()，所有上一个状态还没释放
        if(state_downcast<const SlowCharge*>()) {
            std::cout << "state_downcast SlowCharge\n";
        }else if(state_downcast<const FastCharge*>()) {
            std::cout << "state_downcast_FastCharge\n";
        }
        return transit<chargePause>();
    }
    boost::statechart::result react(const EvSlowCharge& ev) {
        return transit<SlowCharge>();
    }
};

// 充电子状态：慢充
struct SlowCharge : sc::state<SlowCharge, Charging> {
    SlowCharge(my_context c):my_base(c) { std::cout << "Charging Slowly\n";

    }
    using reactions = sc::transition<EvFastCharge, FastCharge>;
};

// 充电子状态：快充
struct FastCharge : sc::simple_state<FastCharge, Charging> {
    FastCharge() { std::cout << "Charging Fast\n"; }
    using reactions = sc::transition<EvSlowCharge, SlowCharge>;
    ~FastCharge(){
        std::cout << "~FastCharge() \n";
    }
};

// 充电子状态：
struct chargePause : sc::simple_state<chargePause, Operational> {
    typedef sc::transition<EvStart, sc::deep_history<SlowCharge>> reactions; // 必须指定为组下的子状态。必须不在同一组内
    chargePause() { std::cout << "Charging chargePause\n"; }
    ~chargePause(){
        std::cout << "~chargePause() \n";
    }
};


int main() {
    Robot robot;
    robot.initiate(); // 默认进入 Moving
    robot.process_event(EvCharge()); // 进入 Charging（默认 SlowCharge）
    robot.process_event(EvFastCharge()); // 切换到 FastCharge
    robot.process_event(EVchargePause()); // 切换到

    robot.process_event(EvStart());

    // robot.process_event(EvSlowCharge()); // 切换到

    // robot.process_event(EvMove()); // 切换到 moving

    // robot.process_event(EvShutdown()); // 进入 Shutdown
    // robot.process_event(EvRestart()); // 恢复到 FastCharge，而不是 SlowCharge

    return 0;
}
#endif
/////////////////////////正交


#include <iostream>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/event.hpp>
#include <boost/mpl/list.hpp>

namespace sc = boost::statechart;
namespace mpl = boost::mpl;

// 事件定义
struct EvPowerOn : sc::event<EvPowerOn> {};
struct EvPowerOff : sc::event<EvPowerOff> {};
struct EvPlay : sc::event<EvPlay> {};
struct EvPause : sc::event<EvPause> {};
struct EvStop : sc::event<EvStop> {};
struct EvMute : sc::event<EvMute> {};
struct EvUnmute : sc::event<EvUnmute> {};

struct EvPowertest : sc::event<EvPowertest> {};

// 前向声明
struct MediaPlayer;
struct Powered;
struct PoweredOn;
struct PoweredOff;
struct PlaybackRegion;
struct Playing;
struct Paused;
struct Stopped;
struct AudioRegion;
struct Muted;
struct Unmuted;

struct PowereTest;

// 媒体播放器状态机
struct MediaPlayer : sc::state_machine<MediaPlayer, Powered> {};

// 顶层状态：电源状态
struct Powered : sc::simple_state<Powered, MediaPlayer, PoweredOff> {
    typedef mpl::list<sc::transition<EvPowerOn, PoweredOn>
        // sc::transition<EvPowerOff, PoweredOff>
            > reactions;
    // typedef sc::transition<EvPowerOn, PoweredOn> reactions;
    Powered() {
        std::cout << "Powered()" << std::endl;
    }
};

// 电源关闭状态
struct PoweredOff : sc::simple_state<PoweredOff, Powered> {
    PoweredOff() {
        std::cout << "PoweredOff()" << std::endl;
    }
    void entry() {
        std::cout << "进入电源关闭状态" << std::endl;
    }
    void exit() {
        std::cout << "离开电源关闭状态" << std::endl;
    }
};

// 电源开启状态 - 包含两个正交区域
struct PoweredOn : sc::simple_state<PoweredOn, Powered, mpl::list<PlaybackRegion, AudioRegion>> {
    typedef mpl::list< sc::transition<EvPowerOff, PoweredOff>
                      /*,sc::transition<EvPowertest, PowereTest>*/ // 会离开所有正交区域，所以只能在正交区域状态转换
                      > reactions;

    void entry() { std::cout << "进入电源开启状态" << std::endl; }
    void exit() { std::cout << "离开电源开启状态" << std::endl; }
};

struct PowereTest: sc::simple_state<PowereTest , PoweredOn> {
    void entry() { std::cout << "进入PowereTest状态" << std::endl; }
    void exit() { std::cout << "离开PowereTest状态" << std::endl; }
};

// 播放控制区域
struct PlaybackRegion : sc::simple_state<PlaybackRegion, PoweredOn::orthogonal<0>, Stopped> {
    void entry() { std::cout << "初始化播放控制区域" << std::endl; }
};

// 播放状态
struct Playing : sc::simple_state<Playing, PlaybackRegion> {
    typedef mpl::list<
        sc::transition<EvPause, Paused>,
        sc::transition<EvStop, Stopped>
        > reactions;

    void entry() { std::cout << "进入播放状态" << std::endl; }
    void exit() { std::cout << "离开播放状态" << std::endl; }
};

// 暂停状态
struct Paused : sc::simple_state<Paused, PlaybackRegion> {
    typedef mpl::list<
        sc::transition<EvPlay, Playing>,
        sc::transition<EvStop, Stopped>
        > reactions;

    void entry() { std::cout << "进入暂停状态" << std::endl; }
    void exit() { std::cout << "离开暂停状态" << std::endl; }
};

// 停止状态
struct Stopped : sc::simple_state<Stopped, PlaybackRegion> {
    typedef sc::transition<EvPlay, Playing> reactions;
    Stopped() { std::cout << "Stopped()" << std::endl; }
    void entry() { std::cout << "进入停止状态" << std::endl; }
    void exit() { std::cout << "离开停止状态" << std::endl; }
};

// 音频控制区域
struct AudioRegion : sc::simple_state<AudioRegion, PoweredOn::orthogonal<1>, Unmuted> {
    void entry() { std::cout << "初始化音频控制区域" << std::endl; }
};

// 静音状态
struct Muted : sc::simple_state<Muted, AudioRegion> {
    typedef sc::transition<EvUnmute, Unmuted> reactions;

    void entry() { std::cout << "进入静音状态" << std::endl; }
    void exit() { std::cout << "离开静音状态" << std::endl; }
};

// 有声状态
struct Unmuted : sc::simple_state<Unmuted, AudioRegion> {
    typedef sc::transition<EvMute, Muted> reactions;

    void entry() { std::cout << "进入有声状态" << std::endl; }
    void exit() { std::cout << "离开有声状态" << std::endl; }
};

// 主函数
int main() {
    MediaPlayer player;

    std::cout << "初始化媒体播放器状态机...\n" << std::endl;
    player.initiate();

    std::cout << "\n打开电源..." << std::endl;
    player.process_event(EvPowerOn());

    std::cout << "\n开始播放..." << std::endl;
    player.process_event(EvPlay());

    std::cout << "\n静音..." << std::endl;
    player.process_event(EvMute());

    // std::cout << "\nEvpowerTest..." << std::endl;
    // player.process_event(EvPowertest());

    std::cout << "\n暂停播放..." << std::endl;
    player.process_event(EvPause());

    std::cout << "\n取消静音..." << std::endl;
    player.process_event(EvUnmute());

    std::cout << "\n停止播放..." << std::endl;
    player.process_event(EvStop());

    std::cout << "\n关闭电源..." << std::endl;
    player.process_event(EvPowerOff());

    while (1) {

    }
    return 0;
}
