/* Copyright (C) 2015 Dan Chapman <dpniel@ubuntu.com>

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
#include "saslmechanisms.h"
#include <sasl/sasl.h>
#include "trgettext.h"

SaslMechanisms::SaslMechanisms(QObject *parent) :
    QObject(parent), sasl_init(0)
{
    // we don't need any callbacks for this
    sasl_init = sasl_client_init(nullptr);
}

QStringList SaslMechanisms::toList()
{
    QStringList res;
    res << trGettext("Auto detect"); // aka: AUTOMATIC

    if (sasl_init != SASL_OK) {
        qDebug() << "LIBSASL2 error: returning our defaults";
        // these two are supported by the large majority of servers
        // and we have AUTOMATIC ("Auto detect") anyway so that will find the "best" method at authentication time
        res << QLatin1String("PLAIN") << QLatin1String("LOGIN");
    } else {
        // Add global mechs to our return list
        const char **saslMechs;
        saslMechs = sasl_global_listmech();
        while (*saslMechs) {
            res << QLatin1String(*saslMechs);
            saslMechs++;
        }
        // TODO: should we have a blacklist of deprecated/obsoleted sasl mechs and remove them from this list??
        // for instance it is recommended not to use DIGEST-MD5 and the RFC was moved to HISTORIC and obsoleted in favor of SCRAM
        // obviously libsasl2 still has backwards compatibility for it, but should we prevent it??
        // http://tools.ietf.org/html/rfc6331
    }
    return res;
}

QObject *SaslMechanisms::factory(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine);
    SaslMechanisms *saslMech = new SaslMechanisms(reinterpret_cast<QObject*>(engine));
    Q_ASSERT(saslMech);
    return saslMech;
}

