#ifndef MAZOPTIONS_H
#define MAZOPTIONS_H

#include <QObject>

namespace leveldb {
class DB;
struct Options;
}

class MazOptions : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool createIfMissing READ createIfMissing WRITE setCreateIfMissing NOTIFY optionChanged)
    Q_PROPERTY(bool errorIfExists READ errorIfExists WRITE setErrorIfExists NOTIFY optionChanged)
    Q_PROPERTY(bool paranoidChecks READ paranoidChecks WRITE setParanoidChecks NOTIFY optionChanged)
    Q_PROPERTY(CompressionType compressionType READ compressionType WRITE setCompressionType NOTIFY optionChanged)
    Q_ENUMS(CompressionType)
public:
    explicit MazOptions(QObject *parent = 0);

    enum CompressionType{
        NoCompression     = 0x0,
        SnappyCompression = 0x1
    };

    bool createIfMissing() const;
    bool errorIfExists() const;
    bool paranoidChecks() const;
    CompressionType compressionType() const;

signals:
    void optionChanged();

public slots:
    void setCreateIfMissing(const bool value);
    void setErrorIfExists(const bool value);
    void setParanoidChecks(const bool value);
    void setCompressionType(const CompressionType type);

private:
    Q_DISABLE_COPY(MazOptions)
    bool m_createIfMissing;
    bool m_errorIfExists;
    bool m_paranoidChecks;
    CompressionType m_compressionType;
};

#endif // MAZOPTIONS_H
