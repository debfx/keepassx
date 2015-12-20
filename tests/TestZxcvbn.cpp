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

#include "TestZxcvbn.h"

#include <QTest>

#include "zxcvbn/RepeatMatcher.h"

QTEST_GUILESS_MAIN(TestZxcvbn)

void TestZxcvbn::testRepeatMatching()
{
    Zxcvbn::RepeatMatcher matcher;

    for (const QString& password : {"", "#"}) {
        const QList<Zxcvbn::Match> matches = matcher.match(password);

        QCOMPARE(matches.size(), 0);
    }

    // test single-character repeats
    QStringList prefixes = { "@", "y4@" };
    QStringList suffixes = { "u", "u%7" };
    QString pattern = "&&&&&";

    Q_FOREACH (const GeneratedPassword& genPw, genPws(pattern, prefixes, suffixes)) {
        const QList<Zxcvbn::Match> matches = matcher.match(genPw.password);

        QCOMPARE(matches.size(), 1);
        QCOMPARE(matches.at(0).value("baseToken").toString(), QString("&"));
        QCOMPARE(matches.at(0).value("i").toInt(), genPw.i);
        QCOMPARE(matches.at(0).value("j").toInt(), genPw.j);
    }

    for (int length : {3, 12}) {
        for (QChar chr : {'a', 'Z', '4', '&'}) {
            QString pattern = QString(length + 1, chr);
            const QList<Zxcvbn::Match> matches = matcher.match(pattern);

            QCOMPARE(matches.size(), 1);
            QCOMPARE(matches.at(0).value("baseToken").toString(), QString(chr));
            QCOMPARE(matches.at(0).value("i").toInt(), 0);
            QCOMPARE(matches.at(0).value("j").toInt(), pattern.size() - 1);
        }
    }
}

QList<GeneratedPassword> TestZxcvbn::genPws(const QString& pattern, QStringList prefixes, QStringList suffixes)
{
    if (!prefixes.contains("")) {
        prefixes.append("");
    }

    if (!suffixes.contains("")) {
        suffixes.append("");
    }

    QList<GeneratedPassword> result;

    for (const QString& prefix : prefixes) {
        for (const QString& suffix : suffixes) {
            GeneratedPassword generated;

            generated.password = prefix + pattern + suffix;
            generated.i = prefix.size();
            generated.j = prefix.size() + pattern.size() - 1;

            result.append(generated);
        }
    }

    return result;
}
