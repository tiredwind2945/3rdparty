// Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
// SPDX-License-Identifier: BSD-3-Clause


#include "qscriptstring.h"


#include "qscriptstring_p.h"
#include "qscriptnameid_p.h"
#include "qscriptvalue_p.h"
#include "qscriptengine_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptcontext_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"

QT_BEGIN_NAMESPACE

/*!
  \since 4.4
  \class QScriptString

  \brief The QScriptString class acts as a handle to "interned" strings in a QScriptEngine.

  \ingroup script
  \mainclass

  QScriptString can be used to achieve faster (repeated)
  property getting/setting, and comparison of property names, of
  script objects.

  To get a QScriptString representation of a string, pass the string
  to QScriptEngine::toStringHandle(). The typical usage pattern is to
  register one or more pre-defined strings when setting up your script
  environment, then subsequently use the relevant QScriptString as
  argument to e.g. QScriptValue::property().

  Call the toString() function to obtain the string that a
  QScriptString represents.
*/

/*!
  \internal
*/
QScriptStringPrivate::QScriptStringPrivate()
    : nameId(0), engine(0), q_ptr(0)
{
    ref = 0;
}

/*!
  \internal
*/
QScriptStringPrivate *QScriptStringPrivate::create()
{
    return new QScriptStringPrivate();
}

/*!
  \internal
*/
QScriptStringPrivate *QScriptStringPrivate::get(const QScriptString &q)
{
    return const_cast<QScriptStringPrivate*>(q.d_func());
}

/*!
  \internal
*/
void QScriptStringPrivate::init(QScriptString &q, QScriptStringPrivate *d)
{
    Q_ASSERT(q.d_ptr == 0);
    q.d_ptr = d;
    q.d_ptr->ref.ref();
}

/*!
  Constructs an invalid QScriptString.
*/
QScriptString::QScriptString()
    : d_ptr(0)
{
}

/*!
  Constructs a new QScriptString that is a copy of \a other.
*/
QScriptString::QScriptString(const QScriptString &other)
    : d_ptr(other.d_ptr)
{
    if (d_ptr)
        d_ptr->ref.ref();
}

/*!
  Destroys this QScriptString.
*/
QScriptString::~QScriptString()
{
    if (d_ptr && !d_ptr->ref.deref()) {
        if (isValid()) {
            d_ptr->engine->uninternString(d_ptr);
        } else {
            // the engine has already been deleted
            delete d_ptr;
        }
        d_ptr = 0;
    }
}

/*!
  Assigns the \a other value to this QScriptString.
*/
QScriptString &QScriptString::operator=(const QScriptString &other)
{
    if (d_ptr == other.d_ptr)
        return *this;
    if (d_ptr && !d_ptr->ref.deref()) {
        if (isValid()) {
            d_ptr->engine->uninternString(d_ptr);
        } else {
            // the engine has already been deleted
            delete d_ptr;
        }
    }
    d_ptr = other.d_ptr;
    if (d_ptr)
        d_ptr->ref.ref();
    return *this;
}

/*!
  Returns true if this QScriptString is valid; otherwise
  returns false.
*/
bool QScriptString::isValid() const
{
    Q_D(const QScriptString);
    return (d && d->nameId);
}

/*!
  Returns true if this QScriptString is equal to \a other;
  otherwise returns false.
*/
bool QScriptString::operator==(const QScriptString &other) const
{
    Q_D(const QScriptString);
    return (d == other.d_func());
}

/*!
  Returns true if this QScriptString is not equal to \a other;
  otherwise returns false.
*/
bool QScriptString::operator!=(const QScriptString &other) const
{
    Q_D(const QScriptString);
    return (d != other.d_func());
}

/*!
  Returns the string that this QScriptString represents, or a
  null string if this QScriptString is not valid.

  \sa isValid()
*/
QString QScriptString::toString() const
{
    Q_D(const QScriptString);
    if (!d || !d->nameId)
        return QString();
    return d->nameId->s;
}

/*!
  Returns the string that this QScriptString represents, or a
  null string if this QScriptString is not valid.

  \sa toString()
*/
QScriptString::operator QString() const
{
    return toString();
}

QT_END_NAMESPACE

