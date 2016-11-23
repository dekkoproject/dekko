#include "MazOptions.h"
#include <leveldb/options.h>

MazOptions::MazOptions(QObject *parent) : QObject(parent),
    m_createIfMissing(true), m_errorIfExists(false), m_paranoidChecks(false),
    m_compressionType(CompressionType::SnappyCompression)
{
}

bool MazOptions::createIfMissing() const
{
    return m_createIfMissing;
}

bool MazOptions::errorIfExists() const
{
    return m_errorIfExists;
}

bool MazOptions::paranoidChecks() const
{
    return m_paranoidChecks;
}

MazOptions::CompressionType MazOptions::compressionType() const
{
    return m_compressionType;
}

void MazOptions::setCreateIfMissing(const bool value)
{
    if (value != m_createIfMissing) {
        m_createIfMissing = value;
        emit optionChanged();
    }
}

void MazOptions::setErrorIfExists(const bool value)
{
    if (value != m_errorIfExists) {
        m_errorIfExists = value;
        emit optionChanged();
    }
}

void MazOptions::setParanoidChecks(const bool value)
{
    if (value != m_paranoidChecks) {
        m_paranoidChecks = value;
        emit optionChanged();
    }
}

void MazOptions::setCompressionType(const MazOptions::CompressionType type)
{
    if (type != m_compressionType) {
        m_compressionType = type;
        emit optionChanged();
    }
}

