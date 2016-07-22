/* Copyright (C) 2014-2016 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu Devices/

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
#ifndef HELPERS_H
#define HELPERS_H

#include <QObject>
#include <QMetaObject>

#define DELAY_CALL(TARGET, FUNCTION, ...) \
{ bool res = QMetaObject::invokeMethod(TARGET, #FUNCTION, Qt::QueuedConnection, __VA_ARGS__); \
        Q_ASSERT(res); Q_UNUSED(res); }

#define DELAY_CALL_NOARG(TARGET, FUNCTION) DELAY_CALL(TARGET, FUNCTION, QGenericArgument())

#define DELAY_EMIT DELAY_CALL
#define DELAY_EMIT_NOARG DELAY_CALL_NOARG

#define QML_WRITABLE_PROPERTY(type, name) \
    protected: \
        Q_PROPERTY (type name READ get_##name WRITE set_##name NOTIFY name##Changed) \
    private: \
        type m_##name; \
    public: \
        type get_##name () const { \
            return m_##name ; \
        } \
    public Q_SLOTS: \
        bool set_##name (type name) { \
            bool ret = false; \
            if ((ret = m_##name != name)) { \
                m_##name = name; \
                Q_EMIT name##Changed (m_##name); \
            } \
            return ret; \
        } \
    Q_SIGNALS: \
        void name##Changed (type name); \
    private:

#define QML_WRITABLE_PROPERTY_WITH_SIGNAL(type, name, signal) \
    protected: \
        Q_PROPERTY (type name READ get_##name WRITE set_##name NOTIFY name##Changed) \
    private: \
        type m_##name; \
    public: \
        type get_##name () const { \
            return m_##name ; \
        } \
    public Q_SLOTS: \
        bool set_##name (type name) { \
            bool ret = false; \
            if ((ret = m_##name != name)) { \
                m_##name = name; \
                Q_EMIT name##Changed (m_##name); \
                Q_EMIT signal(); \
            } \
            return ret; \
        } \
    Q_SIGNALS: \
        void name##Changed (type name); \
    private:

#define QML_READONLY_PROPERTY(type, name) \
    protected: \
        Q_PROPERTY (type name READ get_##name NOTIFY name##Changed) \
    private: \
        type m_##name; \
    public: \
        type get_##name () const { \
            return m_##name ; \
        } \
        bool update_##name (type name) { \
            bool ret = false; \
            if ((ret = m_##name != name)) { \
                m_##name = name; \
                Q_EMIT name##Changed (m_##name); \
            } \
            return ret; \
        } \
    Q_SIGNALS: \
        void name##Changed (type name); \
    private:

#define QML_CONSTANT_PROPERTY(type, name) \
    protected: \
        Q_PROPERTY (type name READ get_##name CONSTANT) \
    private: \
        type m_##name; \
    public: \
        type get_##name () const { \
            return m_##name ; \
        } \
    private:

#define QML_LIST_BY_REFS_PROPERTY(TYPE, NAME) \
    public: \
        Q_PROPERTY(QList<TYPE> NAME MEMBER m_##NAME) \
        Q_INVOKABLE int NAME##_count () { \
            return m_##NAME.count (); \
        } \
        Q_INVOKABLE void NAME##_clear () { \
                m_##NAME.clear (); \
        } \
        Q_INVOKABLE void NAME##_append (TYPE obj) { \
            if (obj.isValid() && !m_##NAME.contains(obj)) { \
                m_##NAME.append (obj); \
            } \
        } \
        Q_INVOKABLE TYPE NAME##_at (int idx) { \
            return m_##NAME.at (idx) ; \
        } \
        Q_INVOKABLE bool NAME##_contains(TYPE obj) { \
                return m_##NAME.contains(obj); \
        } \
        Q_INVOKABLE void NAME##_remove(TYPE obj) { \
               m_##NAME.removeAll(obj); \
        } \
        Q_INVOKABLE QList<TYPE> NAME##_getAll() { \
            return m_##NAME; \
        } \
    private: \
        QList<TYPE> m_##NAME;

#define QML_ENUM_CLASS(name, ...) \
    class name : public QObject { \
        Q_OBJECT \
        Q_ENUMS(Type) \
    public: \
        inline name (QObject *parent = 0) : QObject(parent)  { \
        } \
        enum Type { __VA_ARGS__ }; \
    };


#endif // HELPERS_H
