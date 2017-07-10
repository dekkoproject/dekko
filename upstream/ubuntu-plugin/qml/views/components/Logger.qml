/* Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

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
import QtQuick 2.4
import QuickFlux 1.0
import Dekko.Components 1.0
import Dekko.Mail.API 1.0

AppListener {
    id: logListener
    property bool devLoggingEnabled: true

    Filter {
        type: LogKeys.logInfo
        onDispatched: {
            if (verboseLogging) {
                logMessage(message.location, "INFO", message.message)
            }
        }
    }
    Filter {
        type: LogKeys.logStatus
        onDispatched: {
            if (verboseLogging) {
                logMessage(message.location, "STATUS", message.message)
            }
        }
    }
    Filter {
        type: LogKeys.logWarning
        onDispatched: {
            logMessage(message.location, "WARNING", message.message)
        }
    }
    Filter {
        type: LogKeys.logError
        onDispatched: {
            logMessage(message.location, "ERROR", message.message)
        }
    }

    function logMessage(location, type, message) {
        if (devLoggingEnabled) {
            switch (type) {
            case "INFO":
                LogRecorder.logMessage(location, LogRecorder.INFO, message)
                break
            case "ERROR":
                LogRecorder.logMessage(location, LogRecorder.ERROR, message)
                break
            case "WARNING":
                LogRecorder.logMessage(location, LogRecorder.WARNING, message)
                break
            case "STATUS":
                LogRecorder.logMessage(location, LogRecorder.STATUS, message)
                break
            }
        }
        console.log("[%1] [%2] %3".arg(location).arg(type).arg(message))
    }
}

