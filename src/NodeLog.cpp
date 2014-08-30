/* $Id: NodeLog.cpp 3742 2013-07-10 02:22:58Z IMPOMEZIA $
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#include <QDateTime>
#include <QDir>
#include <QMutexLocker>

#include "NodeLog.h"

#if defined(Q_OS_WIN32)
# include "qt_windows.h"
#endif

typedef QLatin1Char   LC;
typedef QLatin1String LS;

bool NodeLog::m_colors              = true;
NodeLog *NodeLog::m_self            = 0;
NodeLog::Level NodeLog::m_level     = NodeLog::TraceLevel;
NodeLog::OutFlags NodeLog::m_output = NodeLog::FileOut | StdOut;

NodeLog::NodeLog()
  : m_stdout(stdout)
{
  m_self = this;

# if defined(Q_OS_WIN32)
  m_stdout.setCodec("CP" + QByteArray::number(GetConsoleOutputCP()));
# else
  m_stdout.setCodec("UTF-8");
# endif

  m_levels += LS("fatal");
  m_levels += LS("error");
  m_levels += LS("warn ");
  m_levels += LS("info ");
  m_levels += LS("debug");
  m_levels += LS("trace");
}


bool NodeLog::open(const QString &file, Level level)
{
  m_level = level;
  if (level == Disable)
    return false;

  QDir dir(QFileInfo(file).absolutePath());
  if (!dir.exists())
    dir.mkpath(dir.absolutePath());

  bool bom = false;

  m_file.setFileName(file);
  if (!m_file.exists())
    bom = true;

  if (!m_file.open(QFile::WriteOnly | QFile::Text | QFile::Append)) {
    m_output = StdOut;
    return false;
  }

  m_stream.setDevice(&m_file);
  m_stream.setGenerateByteOrderMark(bom);
  m_stream.setCodec("UTF-8");

  return true;
}


void NodeLog::add(Level level, const QString &tag, const QString &message)
{
  if (level == Disable)
    return;

  const QString t = time();

  QMutexLocker lock(&m_mutex);

  if ((m_output & FileOut) && m_file.isOpen())
    m_stream << QString(LS("%1 [%2] [%3] %4")).arg(t, m_levels.at(level), tag, message) << endl;

  if (!(m_output & StdOut))
    return;

  if (!m_colors) {
    m_stdout << QString(LS("%1 [%2] [%3] %4")).arg(t, m_levels.at(level), tag, message) << endl;
    return;
  }

# if defined(Q_OS_WIN32)
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hConsole, 3);
  m_stdout << t;
  m_stdout.flush();

  int color = 13;
  if (level == FatalLevel || level == ErrorLevel)
    color = 12;
  else if (level == WarnLevel)
    color = 14;
  else if (level == InfoLevel)
    color = 15;
  else if (level == TraceLevel)
    color = 9;

  SetConsoleTextAttribute(hConsole, color);
  m_stdout << LS(" [") << m_levels.at(level) << "]";
  m_stdout.flush();

  SetConsoleTextAttribute(hConsole, 11);
  m_stdout << LS(" [") << tag << "] ";
  m_stdout.flush();

  SetConsoleTextAttribute(hConsole, 7);
  m_stdout << message << endl;
  m_stdout.flush();

# else
  QString color = LS("1;35");
  if (level == FatalLevel || level == ErrorLevel)
     color = LS("1;31");
   else if (level == WarnLevel)
     color = LS("1;33");
   else if (level == InfoLevel)
     color = LS("1;37");
   else if (level == TraceLevel)
     color = LS("1;34");

  m_stdout << QString(LS("\033[0;36m%1 \033[%2m[%3] \033[1;36m[%4]\033[0m %5")).arg(t, color, m_levels.at(level), tag, message) << endl;
# endif
}


QString NodeLog::time() const
{
  return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
}
