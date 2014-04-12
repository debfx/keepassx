#include "EntryPlaceholders.h"

#include "core/FieldParser.h"

EntryPlaceholders::EntryPlaceholders(QObject* parent)
    : QObject(parent)
    , m_parser(new FieldParser(this))
{
    connect(m_parser, SIGNAL(field(QString,int,int)), SLOT(replaceField(QString,int,int)));
}

QString EntryPlaceholders::resolvePlaceholders(const QString& str, const Entry* entry)
{
    m_entry = entry;
    m_posDiff = 0;
    m_error = false;
    m_result = str;

    m_parser->parse(str);

    QString tmpResult = m_result;
    m_result.clear();

    if (!m_error) {
        return tmpResult;
    }
    else {
        return QString();
    }
}

void EntryPlaceholders::replaceField(const QString& field, int pos, int len)
{
    QString fieldLower = field.toLower();
    QString replacement;

    if (fieldLower == "title") {
        replacement = m_entry->title();
    }
    else if (fieldLower == "username") {
        replacement = m_entry->username();
    }
    else if (fieldLower == "url") {
        replacement = m_entry->url();
    }
    else if (fieldLower == "password") {
        replacement = m_entry->password();
    }
    else if (fieldLower == "notes") {
        replacement = m_entry->notes();
    }

    if (!replacement.isNull()) {
        m_result.replace(pos + m_posDiff, len, replacement);
        m_posDiff += (replacement.length() - len);
    }
    else {
        m_error = true;
    }
}
