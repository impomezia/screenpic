/*   Copyright (C) 2013-2014 Alexander Sedov <imp@schat.me>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QCryptographicHash>
#include <QFile>
#include <QMetaType>
#include <QProcess>
#include <QSettings>
#include <QStringList>
#include <QTextCodec>

#include "App.h"
#include "AppCore.h"
#include "AutoUpdate.h"
#include "sglobal.h"
#include "tools/Ver.h"
#include "uploaders/UploadResult.h"
#include "version.h"
#include "JSON.h"

App::App(int &argc, char **argv)
  : QtSingleApplication(argc, argv)
  , m_core(0)
{
  setApplicationName(PRODUCT_NAME);
  setApplicationVersion(VERSION_STRING);
  setOrganizationName(ORG_NAME);
  setOrganizationDomain(ORG_DOMAIN);
  setQuitOnLastWindowClosed(false);

# if defined(Q_OS_LINUX)
  setAttribute(Qt::AA_DontShowIconsInMenus, false);
# endif
# if QT_VERSION < 0x050000
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
# endif

  qRegisterMetaType<UploadResult>("UploadResult");

  connect(this, SIGNAL(messageReceived(QString)), SLOT(handleMessage(QString)));

  QFile css(":/style.css");
  if (css.open(QFile::ReadOnly))
    setStyleSheet(css.readAll());
}


App::~App()
{
  stop();
}


bool App::isRunning()
{
  QStringList args = arguments();
  args.removeFirst();

  if (args.isEmpty())
    return sendMessage(QString());

  if (args.contains("-release"))
    return release();

  const QString message = args.join(QLatin1String(", "));

  if (args.contains(QLatin1String("-exit"))) {
    sendMessage(message);
    return true;
  }

  if (args.contains(QLatin1String("-exec")))
    return false;

  return sendMessage(message);
}


bool App::selfUpdate()
{
  const QString appPath = applicationDirPath();
  QSettings s(appPath + LS("/screenpic.ini"), QSettings::IniFormat);
  s.setIniCodec("UTF-8");
  if (!s.value(AutoUpdate::kReady, false).toBool())
    return false;

  s.setValue(AutoUpdate::kReady, false);
  const Ver version = s.value(AutoUpdate::kVersion, QString()).toString();
  if (version.toUInt() == 0)
    return false;

  if (version < applicationVersion())
    return false;

  const QString file(QString(LS("%1/update/%2-%3.exe")).arg(appPath).arg(applicationName().toLower()).arg(version.toString()));
  if (!QFile::exists(file))
    return false;

  QProcess::startDetached(LC('"') + file + LC('"'), QStringList(LS("-update")), appPath);
  return true;
}


void App::restart()
{
  stop();

  if (selfUpdate()) {
    quit();
    return;
  }

  start();
}


void App::start()
{
  m_core = new AppCore(this);
}


void App::stop()
{
  if (m_core) {
    m_core->stop();
    delete m_core;
    m_core = 0;
  }
}


void App::handleMessage(const QString &message)
{
  const QStringList args = message.split(", ");
  if (args.contains("-exit"))
    quit();
}


bool App::release()
{
  QFile file(QString("%1/screenpic-%2.exe").arg(applicationDirPath()).arg(applicationVersion()));
  if (!file.open(QFile::ReadOnly))
    return true;

  const QString hash = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Sha1).toHex();

  QVariantMap data;
  data.insert("version", applicationVersion());
  data.insert("url",     QString("https://download.schat.me/screenpic/screenpic-%1.exe").arg(applicationVersion()));
  data.insert("size",    file.size());
  data.insert("hash",    hash);

  file.close();
  file.setFileName(applicationDirPath() + "/release.json");
  if (file.open(QFile::WriteOnly))
    file.write(JSON::generate(data, true));

  return true;
}
