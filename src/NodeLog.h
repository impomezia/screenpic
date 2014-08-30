/* $Id: NodeLog.h 3722 2013-07-02 23:05:19Z IMPOMEZIA $
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

#ifndef NODELOG_H_
#define NODELOG_H_

#include <QFile>
#include <QMutex>
#include <QStringList>
#include <QTextStream>

#ifndef NO_DEBUG_LOG
class NodeLog
{
public:
  enum Level {
    Disable = -1,
    FatalLevel,
    ErrorLevel,
    WarnLevel,
    InfoLevel,
    DebugLevel,
    TraceLevel
  };

  enum OutFlag {
    NoOut   = 0,
    FileOut = 1,
    StdOut  = 2
  };

  Q_DECLARE_FLAGS(OutFlags, OutFlag)

  NodeLog();
  bool open(const QString &file, Level level);
  inline static Level level()                     { return m_level; }
  inline static NodeLog *i()                      { return m_self; }
  inline static void setColors(bool colors)       { m_colors = colors; }
  inline static void setLevel(Level level)        { m_level = level; }
  inline static void setOutFlags(OutFlags output) { m_output = output; }
  void add(Level level, const QString &tag, const QString &message);

  class Stream : public QTextStream
  {
  public:
    explicit Stream(Level level, const QString &tag)
      : QTextStream()
      , m_tag(tag)
      , m_level(level)
    {
      setString(&m_buffer);
    }

    ~Stream() {
      NodeLog::i()->add(m_level, m_tag, m_buffer);
    }

  private:
    const QString &m_tag;
    Level m_level;
    QString m_buffer;
  };

private:
  QString time() const;

  QFile m_file;             ///< Файл журнала.
  QMutex m_mutex;           ///< Мьютекс защищающий запись.
  QStringList m_levels;     ///< Текстовые обозначения уровней журналирования.
  QTextStream m_stdout;     ///< Текстовый поток для вывода в stdout;
  QTextStream m_stream;     ///< Текстовый поток для записи в журнал.
  static bool m_colors;     ///< \b true если используется цвета для вывода в stdout;
  static Level m_level;     ///< Уровень ведения журнала.
  static NodeLog *m_self;   ///< Указатель на себя.
  static OutFlags m_output; ///< Битовая маска каналов для вывода логов.
};

Q_DECLARE_OPERATORS_FOR_FLAGS(NodeLog::OutFlags)
#endif

#ifndef NO_DEBUG_LOG
# define LOG_FATAL(x) if (NodeLog::level() >= NodeLog::FatalLevel) NodeLog::Stream(NodeLog::FatalLevel, TAG) << x ;
# define LOG_ERROR(x) if (NodeLog::level() >= NodeLog::ErrorLevel) NodeLog::Stream(NodeLog::ErrorLevel, TAG) << x ;
# define LOG_WARN(x)  if (NodeLog::level() >= NodeLog::WarnLevel)  NodeLog::Stream(NodeLog::WarnLevel,  TAG) << x ;
# define LOG_INFO(x)  if (NodeLog::level() >= NodeLog::InfoLevel)  NodeLog::Stream(NodeLog::InfoLevel,  TAG) << x ;
# define LOG_DEBUG(x) if (NodeLog::level() >= NodeLog::DebugLevel) NodeLog::Stream(NodeLog::DebugLevel, TAG) << x ;
# define LOG_TRACE(x) if (NodeLog::level() >= NodeLog::TraceLevel) NodeLog::Stream(NodeLog::TraceLevel, TAG) << x ;
#else
# define LOG_FATAL(x)
# define LOG_ERROR(x)
# define LOG_WARN(x)
# define LOG_INFO(x)
# define LOG_DEBUG(x)
# define LOG_TRACE(x)
#endif

#endif /* NODELOG_H_ */
