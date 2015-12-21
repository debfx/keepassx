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
        QCOMPARE(matches.at(0).value("i").toInt(), genPw.i);
        QCOMPARE(matches.at(0).value("j").toInt(), genPw.j);
        QCOMPARE(matches.at(0).value("token").toString(), pattern);
        QCOMPARE(matches.at(0).value("baseToken").toString(), QString("&"));
    }

    for (int length : {3, 12}) {
        for (QChar chr : {'a', 'Z', '4', '&'}) {
            QString pattern = QString(length + 1, chr);
            const QList<Zxcvbn::Match> matches = matcher.match(pattern);

            QCOMPARE(matches.size(), 1);
            QCOMPARE(matches.at(0).value("i").toInt(), 0);
            QCOMPARE(matches.at(0).value("j").toInt(), pattern.size() - 1);
            QCOMPARE(matches.at(0).value("token").toString(), pattern);
            QCOMPARE(matches.at(0).value("baseToken").toString(), QString(chr));
        }
    }

    {
        const QList<Zxcvbn::Match> matches = matcher.match("BBB1111aaaaa@@@@@@");
        QCOMPARE(matches.size(), 4);

        QCOMPARE(matches.at(0).value("i").toInt(), 0);
        QCOMPARE(matches.at(0).value("j").toInt(), 2);
        QCOMPARE(matches.at(0).value("token").toString(), QString("BBB"));
        QCOMPARE(matches.at(0).value("baseToken").toString(), QString("B"));

        QCOMPARE(matches.at(1).value("i").toInt(), 3);
        QCOMPARE(matches.at(1).value("j").toInt(), 6);
        QCOMPARE(matches.at(1).value("token").toString(), QString("1111"));
        QCOMPARE(matches.at(1).value("baseToken").toString(), QString("1"));

        QCOMPARE(matches.at(2).value("i").toInt(), 7);
        QCOMPARE(matches.at(2).value("j").toInt(), 11);
        QCOMPARE(matches.at(2).value("token").toString(), QString("aaaaa"));
        QCOMPARE(matches.at(2).value("baseToken").toString(), QString("a"));

        QCOMPARE(matches.at(3).value("i").toInt(), 12);
        QCOMPARE(matches.at(3).value("j").toInt(), 17);
        QCOMPARE(matches.at(3).value("token").toString(), QString("@@@@@@"));
        QCOMPARE(matches.at(3).value("baseToken").toString(), QString("@"));
    }

    {
        const QList<Zxcvbn::Match> matches = matcher.match("2818BBBbzsdf1111@*&@!aaaaaEUDA@@@@@@1729");
        QCOMPARE(matches.size(), 4);

        QCOMPARE(matches.at(0).value("i").toInt(), 4);
        QCOMPARE(matches.at(0).value("j").toInt(), 6);
        QCOMPARE(matches.at(0).value("token").toString(), QString("BBB"));
        QCOMPARE(matches.at(0).value("baseToken").toString(), QString("B"));

        QCOMPARE(matches.at(1).value("i").toInt(), 12);
        QCOMPARE(matches.at(1).value("j").toInt(), 15);
        QCOMPARE(matches.at(1).value("token").toString(), QString("1111"));
        QCOMPARE(matches.at(1).value("baseToken").toString(), QString("1"));

        QCOMPARE(matches.at(2).value("i").toInt(), 21);
        QCOMPARE(matches.at(2).value("j").toInt(), 25);
        QCOMPARE(matches.at(2).value("token").toString(), QString("aaaaa"));
        QCOMPARE(matches.at(2).value("baseToken").toString(), QString("a"));

        QCOMPARE(matches.at(3).value("i").toInt(), 30);
        QCOMPARE(matches.at(3).value("j").toInt(), 35);
        QCOMPARE(matches.at(3).value("token").toString(), QString("@@@@@@"));
        QCOMPARE(matches.at(3).value("baseToken").toString(), QString("@"));
    }

    {
        const QList<Zxcvbn::Match> matches = matcher.match("abab");
        QCOMPARE(matches.size(), 1);

        QCOMPARE(matches.at(0).value("i").toInt(), 0);
        QCOMPARE(matches.at(0).value("j").toInt(), 3);
        QCOMPARE(matches.at(0).value("token").toString(), QString("abab"));
        QCOMPARE(matches.at(0).value("baseToken").toString(), QString("ab"));
    }

    {
        const QList<Zxcvbn::Match> matches = matcher.match("aabaab");
        QCOMPARE(matches.size(), 1);

        QCOMPARE(matches.at(0).value("i").toInt(), 0);
        QCOMPARE(matches.at(0).value("j").toInt(), 5);
        QCOMPARE(matches.at(0).value("token").toString(), QString("aabaab"));
        QCOMPARE(matches.at(0).value("baseToken").toString(), QString("aab"));
    }

    {
        const QList<Zxcvbn::Match> matches = matcher.match("abababab");
        QCOMPARE(matches.size(), 1);

        QCOMPARE(matches.at(0).value("i").toInt(), 0);
        QCOMPARE(matches.at(0).value("j").toInt(), 7);
        QCOMPARE(matches.at(0).value("token").toString(), QString("abababab"));
        QCOMPARE(matches.at(0).value("baseToken").toString(), QString("ab"));
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
