#ifndef ENTRYPLACEHOLDERS_H
#define ENTRYPLACEHOLDERS_H

#include <QObject>

#include "core/Global.h"
#include "core/Entry.h"

class FieldParser;

class EntryPlaceholders : public QObject
{
    Q_OBJECT

public:
    explicit EntryPlaceholders(QObject* parent = Q_NULLPTR);
    QString resolvePlaceholders(const QString& str, const Entry* entry);

private Q_SLOTS:
    void replaceField(const QString& field, int pos, int len);

private:
    FieldParser* m_parser;
    const Entry* m_entry;
    int m_posDiff;
    QString m_result;
    bool m_error;
};

#endif // ENTRYPLACEHOLDERS_H
