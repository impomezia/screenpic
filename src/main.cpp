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

#include "App.h"

#if defined(Q_OS_WIN) && defined(Q_CC_MSVC)
# define WIN_CRASHREPORT
# include <QDir>
# include <QStringList>
# include "crashreport/CrashUpload.h"
#endif

#ifdef WIN_CRASHREPORT
# include "crashreport/ExceptionHandler.h"
#endif

int main(int argc, char *argv[])
{
# ifdef WIN_CRASHREPORT
  initExceptionHandler();
# endif

  App app(argc, argv);

# ifdef WIN_CRASHREPORT
  const QFileInfoList files = QDir(app.applicationDirPath()).entryInfoList(QStringList(QLatin1String("*.dmp")), QDir::Files);
  if (!files.isEmpty())
    new CrashUpload(files, &app);

  if (app.arguments().contains(QLatin1String("-report")))
    return app.exec();
# endif

  if (app.isRunning() || App::selfUpdate())
    return 0;

  app.start();
  return app.exec();
}
