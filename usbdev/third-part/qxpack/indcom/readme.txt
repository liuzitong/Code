QxPack.indcom 是一个基于 Qt5 的常用工具包

【目录】

[ common ] -- std c++11, qxiccore package
  这个目录中不依赖于QT，作为最基础的类，含有可作为内存对象使用追踪的宏。

[ algor ]  -- std c++11
   这个目录为独立的模板函数/类部分，仅依赖C++11 的标准模板，可单独复制出来使用（不用编译）。
   

[ afw ] -- QT5, qxicqt5 package
   UI 应用程序主框架，用于构建基于QT5 的GUI程序，提供消息总线以及
从业务层可阻塞式询问UI 界面的确认总线。拥有全局单例对象管理，以及
基于MVVM 的一套模式

[ net ] -- QT5, qxicqt5 package
   基于QT5 的网络工具类

[ sys ] -- QT5, qxicqt5 package
   基于QT5 的通用工具类

[ ui_qml_base ] -- QT5, qxicqt5ui package, QML base
   QML 应用程序的基本类

[ ui_qml_control ] -- QT5, qxicqt5ui package, QML control
   QML 应用程序用的一些基本控件等

[ ui_qml_charts  ] -- QT5, qxicqt5ui package, QML charts
   QML 应用程序用的自定义图表控件


【 链接库 】

qxpackic_all -- 动态链接库，根据QT5 的版本链接对应的 qxpackic_all.dll/so

qxpackic_all_static -- 静态链接库，根据QT5 的版本链接对应的 qxpackic_all_static.lib/so

qxpackic_all_static_noqt -- 不带QT库的版本静态库