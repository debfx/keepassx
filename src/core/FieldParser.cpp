#include "FieldParser.h"

FieldParser::FieldParser(QObject* parent)
    : QObject(parent)
{
}

bool FieldParser::parse(const QString& str)
{
    bool inField = false;
    int start = 0;

    for (int pos = 0; pos < str.length(); pos++) {
        if (inField) {
            if (str[pos] == '}') {
                // field(): pos and len contain { and }
                Q_EMIT field(str.mid(start, pos - start), start - 1, pos - start + 2);
                inField = false;
                start = pos + 1;
            }
            else if (str[pos] == '{') {
                return false;
            }
        }
        else {
            if (str[pos] == '{') {
                if (str.midRef(pos, 3) == "{{}") {
                    Q_EMIT rawString("{");
                    pos += 2;
                    start = pos + 1;
                }
                else if (str.midRef(pos, 3) == "{}}") {
                    Q_EMIT rawString("}");
                    pos += 2;
                    start = pos + 1;
                }
                else {
                    if (start != pos) {
                        Q_EMIT rawString(str.mid(start, pos - start));
                    }
                    inField = true;
                    start = pos + 1;
                }
            }
            else if (str[pos] == '}') {
                return false;
            }
        }
    }

    if (inField) {
        return false;
    }
    else if (start < str.length()) {
        Q_EMIT rawString(str.mid(start));
    }

    return true;
}
