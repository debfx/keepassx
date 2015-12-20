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

#include "RepeatMatcher.h"

#include <QDebug>
#include <QRegExp>

QList<Zxcvbn::Match> Zxcvbn::RepeatMatcher::match(const QString& password)
{
    QList<Match> matches;

    QRegExp greedy("(.+)\\1+", Qt::CaseSensitive, QRegExp::RegExp2);
    QRegExp lazy("(.+)\\1+", Qt::CaseSensitive, QRegExp::RegExp2);
    lazy.setMinimal(true);

    int lastIndex = 0;

    while (lastIndex < password.size()) {
        int greedyMatch = greedy.indexIn(password, lastIndex);
        int lazyMatch = lazy.indexIn(password, lastIndex);

        if (greedyMatch == -1) {
            break;
        }

        int matchIndex;
        int matchLength;
        QString baseToken;
        QString token;

        if (greedy.cap(0).size() > lazy.cap(0).size()) {
            matchIndex = greedyMatch;
            matchLength = greedy.cap(0).size();
            baseToken = greedy.cap(1);
            token = greedy.cap(0);
        }
        else {
            matchIndex = lazyMatch;
            matchLength = lazy.cap(0).size();
            baseToken = lazy.cap(1);
            token = lazy.cap(0);
        }

        int repeatCount = matchLength / baseToken.size();

        // TODO: base_matches, base_analysis.guesses
        matches.append({
            { "pattern", "repeat" },
            { "i", matchIndex },
            { "j", matchIndex + matchLength - 1 },
            { "token", token },
            { "baseToken", baseToken },
            { "repeatCount", repeatCount },
        });

        lastIndex = matchIndex + matchLength;
    }

    return matches;
}
