/*
 *  Copyright (C) 2012-2015 Dan Wheeler and Dropbox, Inc.
 *  Copyright (C) 2015 Felix Geyer <debfx@fobos.de>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "SequenceMatcher.h"

#include <QList>
#include <QPair>

static const QList<QPair<QString, QString>> SEQUENCES = {
    { "lower",  "abcdefghijklmnopqrstuvwxyz" },
    { "upper",  "ABCDEFGHIJKLMNOPQRSTUVWXYZ" },
    { "digits", "0123456789" },
};

QList<Zxcvbn::Match> Zxcvbn::SequenceMatcher::match(const QString& password)
{
    QList<Match> matches;

    for (const QPair<QString, QString> s : SEQUENCES) {
        const QString& sequenceName = s.first;
        const QString& sequence = s.second;

        for (int direction : {-1, 1}) {
            int i = 0;
            while (i < password.size()) {
                if (!sequence.contains(password.at(i))) {
                    i++;
                    continue;
                }

                int j = i + 1;
                int sequencePosition = sequence.indexOf(password.at(i));

                while (j < password.size()) {
                    int nextSequencePosition = (sequencePosition + direction) % sequence.size();

                    if (sequence.indexOf(password.at(j)) != nextSequencePosition) {
                        break;
                    }

                    j++;
                    sequencePosition = nextSequencePosition;
                }

                j--;

                if ((j - i + 1) > 1) {
                    matches.append({
                        { "sequence", "repeat" },
                        { "i", i },
                        { "j", j },
                        { "token", password.mid(i, j - i) },
                        { "sequenceName", sequenceName },
                        { "sequenceSpace", sequence.size() },
                        { "ascending", (direction == 1) },
                    });
                }

                i = j + 1;
            }
        }
    }

    return matches;
}
