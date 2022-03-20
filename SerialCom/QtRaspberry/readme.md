QtRaspberry
===

Hi everyone,
I made a tutorial about Serial Communication between Rapberry and devices (ESP32, Arduino) implemented with Qt and multithreading.
It is shown als how to implement a simple communication protocol for applications where large amount of information are needed.

You can see it on

[youtube](https://youtu.be/AFJxrrIvZrU)

Source code is available on

[github](https://github.com/vigasan/SerialCom.git)

# Singleton
  完全支持单例的版本是[v5.14+](https://doc.qt.io/qt-5/qqmlengine.html#qmlRegisterSingletonType-2)。    
  The version that fully supports singletons is v5.14+    
  This function was introduced in Qt 5.14.
```c++
template <typename T> int qmlRegisterSingletonType(const char *uri, int versionMajor, int versionMinor, const char *typeName, 
std::function<QObject *(QQmlEngine *, QJSEngine *)> callback)
```
  主要的改进是第5个参数！ 之前，是实例不了带参的类的。    
  The main improvement is the fifth parameter! Previously, only classes with no parameters could be instantiated.    
  Example for v5.12: [qml-cpp-integration-example](https://github.com/RaymiiOrg/qml-cpp-integration-example/pull/1/commits/07d82aeea2ed9971fb6a4c61d0ce8a691824ca6e)
```c++
qmlRegisterSingletonType<TrafficLightClass>("org.raymii.RoadObjects", 1, 0, "TrafficLightSingleton",
                                     [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        TrafficLightClass *trafficLightSingleton = new TrafficLightClass();
        return trafficLightSingleton;
    });
```
  Example for v5.14+: [qml-cpp-integration-example](https://raymii.org/s/articles/Qt_QML_Integrate_Cpp_with_QML_and_why_ContextProperties_are_bad.html)
```c++
 TrafficLightClass trafficLightSingleton;
    qmlRegisterSingletonType<TrafficLightClass>("org.raymii.RoadObjects", 1, 0, "TrafficLightSingleton",
                                     [&](QQmlEngine *, QJSEngine *) -> QObject * {
        return &trafficLightSingleton;
    });
```

# Development environment


## qt install

### online
[Qt在线安装程序](https://download.qt.io/official_releases/online_installers/): Qt5.15.2 -> MSVC 2019 32-bit    
![qt-online-msvc2019](https://raw.githubusercontent.com/tdtc-hrb/cnblogs/master/images/qt-online-msvc2019.png)    
以下几项为安装程序默认勾选:    
![qt-online-creator](https://raw.githubusercontent.com/tdtc-hrb/cnblogs/master/images/qt-online-creator.png)
![qt-online-cmake](https://raw.githubusercontent.com/tdtc-hrb/cnblogs/master/images/qt-online-cmake.png)

CMake: 程序设置：    
```bash
-GNinja
-DCMAKE_BUILD_TYPE:STRING=Release
-DCMAKE_PROJECT_INCLUDE_BEFORE:PATH=%{IDE:ResourcePath}/package-manager/auto-setup.cmake
-DQT_QMAKE_EXECUTABLE:STRING=%{Qt:qmakeExecutable}
-DCMAKE_PREFIX_PATH:STRING=%{Qt:QT_INSTALL_PREFIX}
-DCMAKE_C_COMPILER:STRING=%{Compiler:Executable:C}
-DCMAKE_CXX_COMPILER:STRING=%{Compiler:Executable:Cxx}
```

### offline
> qt5.14.2+qtcreator5.03

Installation of QtCreator:     
After installing Qt, delete the QtCreator folder.    

Specify the new QtCreator installation path as the QtCreator folder.

CMake: 程序设置：    
```
-GNMake Makefiles JOM
-DCMAKE_BUILD_TYPE:STRING=Debug
-DCMAKE_PROJECT_INCLUDE_BEFORE:PATH=%{IDE:ResourcePath}/package-manager/auto-setup.cmake
-DQT_QMAKE_EXECUTABLE:STRING=%{Qt:qmakeExecutable}
-DCMAKE_PREFIX_PATH:STRING=C:\Qt5.14.2\5.14.2\msvc2017\lib\cmake\Qt5
-DCMAKE_C_COMPILER:STRING=%{Compiler:Executable:C}
-DCMAKE_CXX_COMPILER:STRING=%{Compiler:Executable:Cxx}
```

## Visual Studio
|qt|vc|
|-|-|
|v5.6.3|vc2013|
|v5.9.9|vc2015|
|v5.12.11|vc2017|
|v5.14.2|vc2017|
|v5.15.2|vc2019|
### vs2019(v16.11)
```bash
.\vs_Community.exe --layout D:\win10soft\vs2019 `
--add Microsoft.VisualStudio.Component.CoreEditor `
--add Microsoft.VisualStudio.Workload.CoreEditor `
--add Microsoft.VisualStudio.Component.Roslyn.Compiler `
--add Microsoft.Component.MSBuild `
--add Microsoft.VisualStudio.Component.TextTemplating `
--add Microsoft.VisualStudio.Component.VC.CoreIde `
--add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
--add Microsoft.VisualStudio.Component.Windows10SDK.19041 `
--add Microsoft.VisualStudio.Component.VC.Redist.14.Latest `
--add Microsoft.VisualStudio.ComponentGroup.NativeDesktop.Core `
--add Microsoft.VisualStudio.Component.VC.Llvm.ClangToolset `
--add Microsoft.VisualStudio.Component.VC.Llvm.Clang `
--add Microsoft.VisualStudio.ComponentGroup.NativeDesktop.Llvm.Clang `
--add Microsoft.VisualStudio.Workload.NativeDesktop --lang en-US
```

## Deploy
```bash
C:\Qt\5.15.2\msvc2019\bin\windeployqt --qmldir C:\Users\tdtc\Documents\SerialCom\SerialCom\QtRaspberry C:\Users\tdtc\Documents\SerialCom_qml\serialCom.exe
```


# Ref
- [Qt and Serial Communication Tutorial](https://forums.raspberrypi.com/viewtopic.php?t=304199)
