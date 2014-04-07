#include <QObject>
#include <QString>

#include "core/Global.h"

class FieldParser : public QObject
{
    Q_OBJECT

public:
    explicit FieldParser(QObject* parent = Q_NULLPTR);
    bool parse(const QString& str);

Q_SIGNALS:
    void field(QString str, int pos, int len);
    void rawString(QString str);
};
