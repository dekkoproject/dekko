/* Copyright (C) 2016 - 2017 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu devices

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include "Dekko.h"

int main(int argc, char** argv)
{
    // set pattern for loggin messages adding date to message text
    qSetMessagePattern("[%{time yyyyMMdd h:mm:ss.zzz t} %{if-category}[%{category}] >> %{endif}%{message}");

    Dekko dekko(argc, argv);

    if (!dekko.setup()) {
        return 0;
    }

    return dekko.exec();
}
