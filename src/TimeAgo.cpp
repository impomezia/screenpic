/*   $Id$
 *   Copyright (C) 2013 Alexander Sedov <imp@schat.me>
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

#include "DateTime.h"
#include "TimeAgo.h"

QString TimeAgo::toString(qint64 date)
{
  double seconds = DateTime::utc() - date;
  if (seconds < 0)
    seconds = 0;
  else
    seconds /= 1000;

  if (seconds < 45)
    return QObject::tr("less than a minute ago");
  else if (seconds < 90)
    return QObject::tr("about a minute ago");

  const double minutes = seconds / 60;
  if (minutes < 45)
    return QObject::tr("%n minutes ago", "", qRound(minutes));
  else if (minutes < 90)
    return QObject::tr("about an hour ago");

  const double hours = seconds / (60 * 60);
  if (hours < 24)
    return QObject::tr("about %n hours ago", "", qRound(hours));
  else if (hours < 42)
    return QObject::tr("a day ago");

  const double days = seconds / (60 * 60 * 24);
  if (days < 30)
    return QObject::tr("%n days ago", "", qRound(days));
  else if (days < 45)
    return QObject::tr("about a month ago");
  else if (days < 365)
    return QObject::tr("%n months ago", "", qRound(days / 30));

  const double years = seconds / (60 * 60 * 24 * 365);
  if (years < 1.5)
    return QObject::tr("about a year ago");

  return QObject::tr("%n years ago", "", qRound(years));
}
