TEMPLATE = app
TARGET = screenpic
CONFIG += c++11

QT += core gui network sql
DEFINES += QT_STATICPLUGIN

macx {
   TARGET = Screenpic
   ICON = ../os/osx/app.icns
   QMAKE_INFO_PLIST = ../os/osx/Info.plist
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
lessThan(QT_MAJOR_VERSION, 5) {
    !win32-msvc*:QMAKE_CXXFLAGS += -std=c++0x
}

GLOBAL_SHORTCUTS = 1

include(config.pri)

HEADERS += \
    App.h \
    AppCore.h \
    AutoUpdate.h \
    base32/base32.h \
    capture/QtCapture.h \
    data/ImageCrop.h \
    data/ImageItem.h \
    data/Thumbnail.h \
    data/UploadItem.h \
    editor/EditorProperties.h \
    Grabber.h \
    Hooks.h \
    interfaces/IHook.h \
    interfaces/IItemCommand.h \
    interfaces/INetHandle.h \
    interfaces/IObserver.h \
    interfaces/IObserverListener.h \
    interfaces/IPlugin.h \
    interfaces/IProvider.h \
    interfaces/IProviderListener.h \
    interfaces/IScreenpic.h \
    interfaces/IServiceTask.h \
    interfaces/ISettings.h \
    interfaces/ISettingsListener.h \
    interfaces/ITranslation.h \
    k8json/k8json.h \
    NodeLog.h \
    Observers.h \
    PluginManager.h \
    plugins.h \
    Providers.h \
    qblowfish/qblowfish.h \
    qblowfish/qblowfish_p.h \
    qtsingleapplication/qtlocalpeer.h \
    qtsingleapplication/qtsingleapplication.h \
    RecentItem.h \
    RecentModel.h \
    RegionGrab.h \
    schat/DateTime.h \
    schat/id/ChatId.h \
    schat/id/ObjectId.h \
    schat/id/SecureRandom.h \
    schat/JSON.h \
    schat/schat.h \
    schat/Settings.h \
    schat/sglobal.h \
    schat/Translation.h \
    ServiceList.h \
    ServiceThread.h \
    ShareDnD.h \
    ShareHistoryDB.h \
    ShareNet.h \
    tasks/SaveTask.h \
    tasks/ThumbTask.h \
    TimeAgo.h \
    tools/OsInfo.h \
    tools/Ver.h \
    TrayIcon.h \
    ui/AboutPage.h \
    ui/BackdropWidget.h \
    ui/DialogFrame.h \
    ui/Flyout.h \
    ui/RecentItemDelegate.h \
    ui/RecentView.h \
    ui/ServersPage.h \
    ui/SettingsDialog.h \
    ui/SettingsPage.h \
    ui/Spinner.h \
    ui/TitleWidget.h \
    ui/TrayWindow.h \
    uploaders/Uploader.h \
    uploaders/UploadResult.h \
    version.h \
    Clipboard.h

SOURCES += \
    App.cpp \
    AppCore.cpp \
    AutoUpdate.cpp \
    base32/base32.cpp \
    capture/QtCapture.cpp \
    data/ImageCrop.cpp \
    data/ImageItem.cpp \
    data/UploadItem.cpp \
    editor/EditorProperties.cpp \
    Grabber.cpp \
    Hooks.cpp \
    k8json/k8json.cpp \
    main.cpp \
    Observers.cpp \
    PluginManager.cpp \
    Providers.cpp \
    qblowfish/qblowfish.cpp \
    qtsingleapplication/qtlocalpeer.cpp \
    qtsingleapplication/qtsingleapplication.cpp \
    RecentItem.cpp \
    RecentModel.cpp \
    RegionGrab.cpp \
    schat/DateTime.cpp \
    schat/id/ChatId.cpp \
    schat/id/ObjectId.cpp \
    schat/id/SecureRandom.cpp \
    schat/JSON.cpp \
    schat/Settings.cpp \
    schat/Translation.cpp \
    ServiceList.cpp \
    ServiceThread.cpp \
    ShareDnD.cpp \
    ShareHistoryDB.cpp \
    ShareNet.cpp \
    tasks/SaveTask.cpp \
    tasks/ThumbTask.cpp \
    TimeAgo.cpp \
    tools/OsInfo.cpp \
    tools/Ver.cpp \
    TrayIcon.cpp \
    ui/AboutPage.cpp \
    ui/BackdropWidget.cpp \
    ui/DialogFrame.cpp \
    ui/Flyout.cpp \
    ui/RecentItemDelegate.cpp \
    ui/RecentView.cpp \
    ui/ServersPage.cpp \
    ui/SettingsDialog.cpp \
    ui/SettingsPage.cpp \
    ui/Spinner.cpp \
    ui/TitleWidget.cpp \
    ui/TrayWindow.cpp \
    uploaders/Uploader.cpp \
    uploaders/UploadResult.cpp \

RESOURCES += ../res/app.qrc

win32 {
    RC_FILE = ../res/app.rc
    HEADERS += \
        capture/GDICapture.h \
        crashreport/CrashUpload.h \
        crashreport/ExceptionHandler.h \

    SOURCES += \
        Grabber_win.cpp \
        capture/GDICapture.cpp \
        crashreport/CrashUpload.cpp \
        crashreport/ExceptionHandler.cpp \
        tools/OsInfo_win.cpp \
        Clipboard_win.cpp \

    LIBS += -luser32 -lgdi32 -lshell32
    CONFIG -= embed_manifest_exe
    QMAKE_POST_LINK = $$quote(mt.exe -nologo -manifest \"$$replace(PWD,/,\\)\\..\\res\\app.manifest\" -outputresource:$(DESTDIR_TARGET);1)$$QMAKE_POST_LINK
}
else {
    SOURCES += Clipboard.cpp
}

unix:!macx: {
  SOURCES += tools/OsInfo_unix.cpp
}

macx: {
  SOURCES += tools/OsInfo_mac.cpp
}

contains(GLOBAL_SHORTCUTS, 1) {
    HEADERS += \
        libqxt/qxtglobal.h \
        libqxt/qxtglobalshortcut.h \
        libqxt/qxtglobalshortcut_p.h \
        GlobalShortcuts.h \
        ui/HotkeyEditor.h \
        ui/HotkeysPage.h \
        
    SOURCES += \
        libqxt/qxtglobal.cpp \
        libqxt/qxtglobalshortcut.cpp \
        ui/HotkeyEditor.cpp \
        ui/HotkeysPage.cpp \
        GlobalShortcuts.cpp \
        
    win32 {
        DEFINES += QXT_STATIC
        SOURCES += libqxt/qxtglobalshortcut_win.cpp
    }

    macx {
        SOURCES += libqxt/qxtglobalshortcut_mac.cpp
        LIBS += -framework Carbon
    }
}
else {
    DEFINES += NO_GLOBAL_SHORTCUTS
}

contains(DEBUG_LOG, 1) {
    SOURCES += NodeLog.cpp
}
else {
    DEFINES += NO_DEBUG_LOG
}

isEmpty(GIT_TIMESTAMP) {
    GIT_TIMESTAMP = $$system($$quote(git log -n 1 --format=format:"%at"))
    GIT_REVISION  = $$system($$quote(git rev-parse HEAD))

    lessThan(GIT_TIMESTAMP, 1):GIT_TIMESTAMP = 0
}

DEFINES += GIT_TIMESTAMP=$$GIT_TIMESTAMP
DEFINES += GIT_REVISION=\\\"$${GIT_REVISION}\\\"

include(plugins.pri)
include(editor/editor.pri)
include(common.pri)
